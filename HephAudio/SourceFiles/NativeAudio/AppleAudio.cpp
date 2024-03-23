#if defined(__APPLE__)
#include "NativeAudio/AppleAudio.h"
#include "AudioProcessor.h"
#include "../HephCommon/HeaderFiles/File.h"
#include "../HephCommon/HeaderFiles/ConsoleLogger.h"
#include "../HephCommon/HeaderFiles/StopWatch.h"
#include <CoreFoundation/CFString.h>

#if !defined(MAC_OS_VERSION_12_0)
#define kAudioObjectPropertyElementMain kAudioObjectPropertyElementMaster
#else
#define kAudioObjectPropertyElementMain 0
#endif

#define APPLE_INTERNAL_SPEAKERS_SOURCE_ID 1769173099
#define APPLE_HEADPHONES_SOURCE_ID 1751412846
#define APPLE_INTERNAL_MIC_SOURCE_ID 1768778083
#define APPLE_EXTERNAL_MIC_SOURCE_ID 1701669219
#define APPLE_ENUMERATE_DEVICE_EXCPT(r, appleAudio, method, message) result = r; if (result != kAudioHardwareNoError) { if (deviceIDs != nullptr) { free(deviceIDs); } RAISE_HEPH_EXCEPTION(appleAudio, HephException(result, method, message)); return NativeAudio::DEVICE_ENUMERATION_FAIL; }
#define APPLE_ENUMERATE_DEVICE_SOURCES_EXCPT(r, appleAudio, method, message) result = r; if (result != kAudioHardwareNoError) { free(deviceSources); RAISE_HEPH_EXCEPTION(appleAudio, HephException(result, method, message)); return NativeAudio::DEVICE_ENUMERATION_FAIL; }
#define APPLE_EXCPT(r, appleAudio, method, message) result = r; if (result != kAudioHardwareNoError) { RAISE_AND_THROW_HEPH_EXCEPTION(appleAudio, HephException(result, method, message)); }

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		AppleAudio::AppleAudio() : NativeAudio(), renderProcID(nullptr), captureProcID(nullptr)
		{
			this->EnumerateAudioDevices();
			this->deviceThread = std::thread(&AppleAudio::CheckAudioDevices, this);
		}
		AppleAudio::~AppleAudio()
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG("Destructing AppleAudio...", HEPH_CL_INFO);

			disposing = true;

			JoinDeviceThread();

			if (renderProcID != nullptr)
			{
				const AudioDeviceID deviceID = StringBuffer::StringToUI32(renderDeviceId.Split('S')[0]);
				AudioDeviceStop(deviceID, renderProcID);
				AudioDeviceDestroyIOProcID(deviceID, renderProcID);
			}

			if (captureProcID != nullptr)
			{
				const AudioDeviceID deviceID = StringBuffer::StringToUI32(captureDeviceId.Split('S')[0]);
				AudioDeviceStop(deviceID, captureProcID);
				AudioDeviceDestroyIOProcID(deviceID, captureProcID);
			}

			HEPHAUDIO_LOG("AppleAudio destructed in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AppleAudio::SetMasterVolume(heph_float volume)
		{
			if (isRenderInitialized)
			{
				const AudioDeviceID deviceID = StringBuffer::StringToUI32(renderDeviceId.Split('S')[0]);
				const Float32 v32 = volume;

				AudioObjectPropertyAddress propertyList;
				propertyList.mSelector = kAudioDevicePropertyVolumeScalar;
				propertyList.mScope = kAudioObjectPropertyScopeOutput;

				uint32_t channel = 1;

				do
				{
					propertyList.mElement = channel;
					channel++;
				} while (AudioObjectSetPropertyData(deviceID, &propertyList, 0, nullptr, sizeof(Float32), &v32) == kAudioHardwareNoError);
			}
		}
		heph_float AppleAudio::GetMasterVolume() const
		{
			if (isRenderInitialized)
			{
				const AudioDeviceID deviceID = StringBuffer::StringToUI32(renderDeviceId.Split('S')[0]);
				UInt32 size = sizeof(Float32);
				Float32 volume;
				OSStatus result;

				AudioObjectPropertyAddress propertyList;
				propertyList.mSelector = kAudioDevicePropertyVolumeScalar;
				propertyList.mScope = kAudioObjectPropertyScopeOutput;
				propertyList.mElement = 1;

				APPLE_EXCPT(AudioObjectGetPropertyData(deviceID, &propertyList, 0, nullptr, &size, &volume), this, "AppleAudio::GetMasterVolume", "An error occurred whilst getting the master volume.");

				return volume;
			}
			return -1.0f;
		}
		void AppleAudio::InitializeRender(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing render with the default device..." : (char*)("Initializing render (" + device->name + ")..."), HEPH_CL_INFO);

			StopRendering();

			OSStatus result;

			renderDeviceId = device != nullptr ? device->id : GetDefaultAudioDevice(AudioDeviceType::Render).id;

			std::vector<StringBuffer> renderDeviceSourceIDs = renderDeviceId.Split('S');
			const AudioObjectID deviceID = StringBuffer::StringToUI32(renderDeviceSourceIDs[0]);

			UInt32 size;
			AudioObjectPropertyAddress propertyList;
			propertyList.mScope = kAudioObjectPropertyScopeOutput;
			propertyList.mElement = kAudioObjectPropertyElementMain;

			if (renderDeviceSourceIDs.size() == 2)
			{
				propertyList.mSelector = kAudioDevicePropertyDataSource;
				const AudioObjectID sourceID = StringBuffer::StringToUI32(renderDeviceSourceIDs[1]);
				APPLE_EXCPT(AudioObjectSetPropertyData(deviceID, &propertyList, 0, nullptr, sizeof(AudioObjectID), &sourceID), this, "AppleAudio::InitializeRender", "An error occurred whilst setting the audio device source.");
			}

			AudioStreamBasicDescription streamDesc;
			ToStreamDesc(format, streamDesc);
			size = sizeof(streamDesc);
			propertyList.mSelector = kAudioDevicePropertyStreamFormatMatch;
			APPLE_EXCPT(AudioObjectGetPropertyData(deviceID, &propertyList, 0, nullptr, &size, &streamDesc), this, "AppleAudio::InitializeRender", "An error occurred whilst getting the stream description.");
			FromStreamDesc(renderFormat, streamDesc);
			propertyList.mSelector = kAudioDevicePropertyStreamFormat;
			APPLE_EXCPT(AudioObjectSetPropertyData(deviceID, &propertyList, 0, nullptr, size, &streamDesc), this, "AppleAudio::InitializeRender", "An error occurred whilst setting the stream description.");

			APPLE_EXCPT(AudioDeviceCreateIOProcID(deviceID, &AppleAudio::RenderCallback, this, &renderProcID), this, "AppleAudio::InitializeRender", "An error occurred whilst creating the IO proc.");
			APPLE_EXCPT(AudioDeviceStart(deviceID, renderProcID), this, "AppleAudio::InitializeRender", "An error occurred whilst starting the audio device.");

			isRenderInitialized = true;

			HEPHAUDIO_LOG("Render initialized in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AppleAudio::StopRendering()
		{
			if (isRenderInitialized)
			{
				const AudioDeviceID deviceID = StringBuffer::StringToUI32(renderDeviceId.Split('S')[0]);
				OSStatus result;
				isRenderInitialized = false;
				renderDeviceId = "";
				APPLE_EXCPT(AudioDeviceStop(deviceID, renderProcID), this, "AppleAudio::StopRendering", "An error occurred whilst stopping the render.");
				APPLE_EXCPT(AudioDeviceDestroyIOProcID(deviceID, renderProcID), this, "AppleAudio::StopRendering", "An error occurred whilst destroying the IO proc.");
				renderProcID = nullptr;
				HEPHAUDIO_LOG("Stopped rendering.", HEPH_CL_INFO);
			}
		}
		void AppleAudio::InitializeCapture(AudioDevice* device, AudioFormatInfo format)
		{
			HEPHAUDIO_STOPWATCH_RESET;
			HEPHAUDIO_LOG(device == nullptr ? "Initializing capture with the default device..." : (char*)("Initializing capture (" + device->name + ")..."), HEPH_CL_INFO);

			StopCapturing();

			OSStatus result;

			captureDeviceId = device != nullptr ? device->id : GetDefaultAudioDevice(AudioDeviceType::Capture).id;

			std::vector<StringBuffer> captureDeviceSourceIDs = captureDeviceId.Split('S');
			const AudioObjectID deviceID = StringBuffer::StringToUI32(captureDeviceSourceIDs[0]);

			UInt32 size;
			AudioObjectPropertyAddress propertyList;
			propertyList.mScope = kAudioObjectPropertyScopeInput;
			propertyList.mElement = kAudioObjectPropertyElementMain;

			if (captureDeviceSourceIDs.size() == 2)
			{
				propertyList.mSelector = kAudioDevicePropertyDataSource;
				const AudioObjectID sourceID = StringBuffer::StringToUI32(captureDeviceSourceIDs[1]);
				APPLE_EXCPT(AudioObjectSetPropertyData(deviceID, &propertyList, 0, nullptr, sizeof(AudioObjectID), &sourceID), this, "AppleAudio::InitializeCapture", "An error occurred whilst setting the audio device source.");
			}

			AudioStreamBasicDescription streamDesc;
			ToStreamDesc(format, streamDesc);
			size = sizeof(streamDesc);
			propertyList.mSelector = kAudioDevicePropertyStreamFormatMatch;
			APPLE_EXCPT(AudioObjectGetPropertyData(deviceID, &propertyList, 0, nullptr, &size, &streamDesc), this, "AppleAudio::InitializeCapture", "An error occurred whilst getting the stream description.");
			FromStreamDesc(captureFormat, streamDesc);
			propertyList.mSelector = kAudioDevicePropertyStreamFormat;
			APPLE_EXCPT(AudioObjectSetPropertyData(deviceID, &propertyList, 0, nullptr, size, &streamDesc), this, "AppleAudio::InitializeCapture", "An error occurred whilst setting the stream description.");

			APPLE_EXCPT(AudioDeviceCreateIOProcID(deviceID, &AppleAudio::CaptureCallback, this, &captureProcID), this, "AppleAudio::InitializeCapture", "An error occurred whilst creating the IO proc.");
			APPLE_EXCPT(AudioDeviceStart(deviceID, captureProcID), this, "AppleAudio::InitializeCapture", "An error occurred whilst starting the audio device.");

			isCaptureInitialized = true;

			HEPHAUDIO_LOG("Capture initialized in " + HephCommon::StringBuffer::ToString(HEPHAUDIO_STOPWATCH_DT(HEPH_SW_MILLI), 4) + " ms.", HEPH_CL_INFO);
		}
		void AppleAudio::StopCapturing()
		{
			if (isCaptureInitialized)
			{
				const AudioDeviceID deviceID = StringBuffer::StringToUI32(captureDeviceId.Split('S')[0]);
				OSStatus result;
				isCaptureInitialized = false;
				captureDeviceId = "";
				APPLE_EXCPT(AudioDeviceStop(deviceID, captureProcID), this, "AppleAudio::StopCapturing", "An error occurred whilst stopping the capture.");
				APPLE_EXCPT(AudioDeviceDestroyIOProcID(deviceID, captureProcID), this, "AppleAudio::StopCapturing", "An error occurred whilst destroying the IO proc.");
				captureProcID = nullptr;
				HEPHAUDIO_LOG("Stopped capturing.", HEPH_CL_INFO);
			}
		}
		bool AppleAudio::EnumerateAudioDevices()
		{
			AudioObjectPropertyAddress propertyList;
			AudioObjectID defaultRenderID;
			AudioObjectID defaultCaptureID;
			AudioObjectID* deviceIDs = nullptr;
			OSStatus result;
			UInt32 size;

			this->audioDevices.clear();

			propertyList.mScope = kAudioObjectPropertyScopeGlobal;
			propertyList.mElement = kAudioObjectPropertyElementMain;

			size = sizeof(AudioObjectID);
			propertyList.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
			APPLE_ENUMERATE_DEVICE_EXCPT(AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyList, 0, nullptr, &size, &defaultRenderID), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the default audio render devices.");

			propertyList.mSelector = kAudioHardwarePropertyDefaultInputDevice;
			APPLE_ENUMERATE_DEVICE_EXCPT(AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyList, 0, nullptr, &size, &defaultCaptureID), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the default audio capture devices.");


			propertyList.mSelector = kAudioHardwarePropertyDevices;
			APPLE_ENUMERATE_DEVICE_EXCPT(AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyList, 0, nullptr, &size), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst enumerating the audio devices.");
			deviceIDs = (AudioObjectID*)malloc(size);
			if (deviceIDs == nullptr)
			{
				RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AppleAudio::EnumerateAudioDevices", "Insufficient memory."));
				return AppleAudio::DEVICE_ENUMERATION_FAIL;
			}

			APPLE_ENUMERATE_DEVICE_EXCPT(AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyList, 0, nullptr, &size, deviceIDs), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst enumerating the audio devices.");
			const size_t deviceCount = size / sizeof(AudioObjectID);

			CFStringRef cfs;
			CFIndex cfsLength;
			for (size_t i = 0; i < deviceCount; i++)
			{
				auto addSources = [this, &propertyList, &size, &deviceIDs, &cfs, &cfsLength, &result, &i, &defaultRenderID, &defaultCaptureID](AudioDeviceType deviceType) -> bool
				{
					propertyList.mSelector = kAudioDevicePropertyDataSources;
					APPLE_ENUMERATE_DEVICE_EXCPT(AudioObjectGetPropertyDataSize(deviceIDs[i], &propertyList, 0, nullptr, &size), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the device sources.");
					const size_t deviceSourceCount = size / sizeof(UInt32);

					if (deviceSourceCount == 0)
						return AppleAudio::DEVICE_ENUMERATION_SUCCESS;

					UInt32* deviceSources = (UInt32*)malloc(size);
					if (deviceSources == nullptr)
					{
						RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AppleAudio::EnumerateAudioDevices", "Insufficient memory."));
						return AppleAudio::DEVICE_ENUMERATION_FAIL;
					}
					APPLE_ENUMERATE_DEVICE_EXCPT(AudioObjectGetPropertyData(deviceIDs[i], &propertyList, 0, nullptr, &size, deviceSources), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the device sources.");

					for (size_t j = 0; j < deviceSourceCount; j++)
					{
						AudioDevice device;
						device.id = StringBuffer::ToString(deviceIDs[i]) + 'S' + StringBuffer::ToString(deviceSources[j]);

						AudioValueTranslation avt;
						avt.mInputData = deviceSources + j;
						avt.mInputDataSize = sizeof(UInt32);
						avt.mOutputData = &cfs;
						avt.mOutputDataSize = sizeof(CFStringRef);

						propertyList.mSelector = kAudioDevicePropertyDataSourceNameForIDCFString;
						size = sizeof(AudioValueTranslation);
						APPLE_ENUMERATE_DEVICE_SOURCES_EXCPT(AudioObjectGetPropertyData(deviceIDs[i], &propertyList, 0, nullptr, &size, &avt), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the device source id.");
						cfsLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfs), kCFStringEncodingUTF8);
						const StringBuffer sourceName = '\0'_b * cfsLength;
						if (!CFStringGetCString(cfs, sourceName.c_str(), cfsLength + 1, kCFStringEncodingUTF8))
						{
							free(deviceSources);
							CFRelease(cfs);
							RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the device source id."));
							return AppleAudio::DEVICE_ENUMERATION_FAIL;
						}
						CFRelease(cfs);
						device.name = sourceName.SubString(0, strlen(sourceName.c_str())); // get rid of the extra '\0' characters

						device.type = deviceType;
						device.isDefault = (j == 0 && (deviceIDs[i] == defaultRenderID || deviceIDs[i] == defaultCaptureID));

						this->audioDevices.push_back(device);
					}

					free(deviceSources);

					return AppleAudio::DEVICE_ENUMERATION_SUCCESS;
				};

				propertyList.mSelector = kAudioDevicePropertyDataSourceNameForIDCFString;
				propertyList.mScope = kAudioObjectPropertyScopeInput;
				if (AudioObjectHasProperty(deviceIDs[i], &propertyList))
				{
					if (addSources(AudioDeviceType::Capture) == AppleAudio::DEVICE_ENUMERATION_FAIL)
					{
						free(deviceIDs);
						return AppleAudio::DEVICE_ENUMERATION_FAIL;
					}
				}
				else
				{
					propertyList.mScope = kAudioObjectPropertyScopeOutput;
					if (AudioObjectHasProperty(deviceIDs[i], &propertyList))
					{
						if (addSources(AudioDeviceType::Render) == AppleAudio::DEVICE_ENUMERATION_FAIL)
						{
							free(deviceIDs);
							return AppleAudio::DEVICE_ENUMERATION_FAIL;
						}
					}
					else
					{
						AudioDevice device;
						device.id = StringBuffer::ToString((uint32_t)deviceIDs[i]);
						device.type = AudioDeviceType::Null;

						propertyList.mSelector = kAudioObjectPropertyName;
						propertyList.mScope = kAudioObjectPropertyScopeGlobal;
						size = sizeof(CFStringRef);
						APPLE_ENUMERATE_DEVICE_EXCPT(AudioObjectGetPropertyData(deviceIDs[i], &propertyList, 0, nullptr, &size, &cfs), this, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the device name.");
						cfsLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfs), kCFStringEncodingUTF8);
						device.name = '\0'_b * cfsLength;
						if (!CFStringGetCString(cfs, device.name.c_str(), cfsLength + 1, kCFStringEncodingUTF8))
						{
							free(deviceIDs);
							CFRelease(cfs);
							RAISE_HEPH_EXCEPTION(this, HephException(HEPH_EC_INSUFFICIENT_MEMORY, "AppleAudio::EnumerateAudioDevices", "An error occurred whilst getting the device name."));
							return AppleAudio::DEVICE_ENUMERATION_FAIL;
						}
						CFRelease(cfs);
						device.name = device.name.SubString(0, strlen(device.name.c_str())); // get rid of the extra '\0' characters
						device.isDefault = (deviceIDs[i] == defaultRenderID || deviceIDs[i] == defaultCaptureID);

						this->audioDevices.push_back(device);
					}
				}
			}

			free(deviceIDs);

			return AppleAudio::DEVICE_ENUMERATION_SUCCESS;
		}
		void AppleAudio::ToStreamDesc(const AudioFormatInfo& format, AudioStreamBasicDescription& streamDesc) const
		{
			streamDesc.mSampleRate = renderFormat.sampleRate;
			streamDesc.mBitsPerChannel = renderFormat.bitsPerSample;
			streamDesc.mChannelsPerFrame = renderFormat.channelCount;
			streamDesc.mBytesPerFrame = renderFormat.FrameSize();
			streamDesc.mFramesPerPacket = 1;
			streamDesc.mBytesPerPacket = streamDesc.mBytesPerFrame * streamDesc.mFramesPerPacket;
			streamDesc.mReserved = 0;
			streamDesc.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;

			switch (format.formatTag)
			{
			case HEPHAUDIO_FORMAT_TAG_PCM:
			{
				streamDesc.mFormatID = kAudioFormatLinearPCM;
				if (format.bitsPerSample != 8)
				{
					streamDesc.mFormatFlags |= kAudioFormatFlagIsSignedInteger;
				}
			}
			break;
			case HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT:
				streamDesc.mFormatID = kAudioFormatLinearPCM;
				streamDesc.mFormatFlags |= kAudioFormatFlagIsFloat;
				break;
			case HEPHAUDIO_FORMAT_TAG_ALAW:
				streamDesc.mFormatID = kAudioFormatALaw;
				break;
			case HEPHAUDIO_FORMAT_TAG_MULAW:
				streamDesc.mFormatID = kAudioFormatULaw;
				break;
			case HEPHAUDIO_FORMAT_TAG_FLAC:
				streamDesc.mFormatID = kAudioFormatFLAC;
				break;
			case HEPHAUDIO_FORMAT_TAG_ALAC:
				streamDesc.mFormatID = kAudioFormatAppleLossless;
				break;
			case HEPHAUDIO_FORMAT_TAG_MPEGLAYER3:
				streamDesc.mFormatID = kAudioFormatMPEGLayer3;
				break;
			case HEPHAUDIO_FORMAT_TAG_MPEG4_AAC:
				streamDesc.mFormatID = kAudioFormatMPEG4AAC;
				break;
			case HEPHAUDIO_FORMAT_TAG_OPUS:
				streamDesc.mFormatID = kAudioFormatOpus;
				break;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AppleAudio", "Invalid audio format."));
			}
		}
		void AppleAudio::FromStreamDesc(AudioFormatInfo& format, const AudioStreamBasicDescription& streamDesc) const
		{
			format.sampleRate = streamDesc.mSampleRate;
			format.bitsPerSample = streamDesc.mBitsPerChannel;
			format.channelCount = streamDesc.mChannelsPerFrame;

			switch (streamDesc.mFormatID)
			{
			case kAudioFormatLinearPCM:
				format.formatTag = ((streamDesc.mFormatFlags & kAudioFormatFlagIsFloat) == kAudioFormatFlagIsFloat) ? HEPHAUDIO_FORMAT_TAG_IEEE_FLOAT : HEPHAUDIO_FORMAT_TAG_PCM;
				break;
			case kAudioFormatALaw:
				format.formatTag = HEPHAUDIO_FORMAT_TAG_ALAW;
				break;
			case kAudioFormatULaw:
				format.formatTag = HEPHAUDIO_FORMAT_TAG_MULAW;
				break;
			case kAudioFormatFLAC:
				format.formatTag = HEPHAUDIO_FORMAT_TAG_FLAC;
				break;
			case kAudioFormatAppleLossless:
				format.formatTag = HEPHAUDIO_FORMAT_TAG_ALAC;
				break;
			case kAudioFormatMPEGLayer3:
				format.formatTag = HEPHAUDIO_FORMAT_TAG_MPEGLAYER3;
				break;
			case kAudioFormatMPEG4AAC:
				format.formatTag = HEPHAUDIO_FORMAT_TAG_MPEG4_AAC;
				break;
			case kAudioFormatOpus:
				format.formatTag = HEPHAUDIO_FORMAT_TAG_OPUS;
				break;
			default:
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(HEPH_EC_FAIL, "AppleAudio", "Invalid audio format."));
			}

			format.bitRate = AudioFormatInfo::CalculateBitrate(format);
		}
		OSStatus AppleAudio::RenderCallback(AudioDeviceID device, const AudioTimeStamp* now, const AudioBufferList* indata,
			const AudioTimeStamp* intime, AudioBufferList* outdata, const AudioTimeStamp* outtime, void* udata)
		{
			AppleAudio* appleAudio = (AppleAudio*)udata;
			if (!appleAudio->disposing && appleAudio->isRenderInitialized)
			{
				for (size_t i = 0; i < outdata->mNumberBuffers; i++)
				{
					AudioBuffer buffer(outdata->mBuffers[i].mDataByteSize / appleAudio->renderFormat.FrameSize(), appleAudio->renderFormat);
					appleAudio->Mix(buffer, buffer.FrameCount());
					memcpy(outdata->mBuffers[i].mData, buffer.Begin(), outdata->mBuffers[i].mDataByteSize);
				}
			}
			return kAudioHardwareNoError;
		}
		OSStatus AppleAudio::CaptureCallback(AudioDeviceID device, const AudioTimeStamp* now, const AudioBufferList* indata,
			const AudioTimeStamp* intime, AudioBufferList* outdata, const AudioTimeStamp* outtime, void* udata)
		{
			AppleAudio* appleAudio = (AppleAudio*)udata;
			if (!appleAudio->disposing && appleAudio->isCaptureInitialized && !appleAudio->isCapturePaused && appleAudio->OnCapture)
			{
				size_t frameCount = 0;
				for (size_t i = 0; i < indata->mNumberBuffers; i++)
				{
					frameCount += indata->mBuffers[i].mDataByteSize / appleAudio->captureFormat.FrameSize();
				}

				size_t offset = 0;
				AudioBuffer buffer(frameCount, appleAudio->captureFormat);
				for (size_t i = 0; i < indata->mNumberBuffers; i++)
				{
					memcpy((uint8_t*)buffer.Begin() + offset, indata->mBuffers[i].mData, indata->mBuffers[i].mDataByteSize);
					offset += indata->mBuffers[i].mDataByteSize;
				}

				AudioProcessor::ConvertToInnerFormat(buffer);
				AudioCaptureEventArgs captureEventArgs(appleAudio, buffer);
				appleAudio->OnCapture(&captureEventArgs, nullptr);
			}
			return kAudioHardwareNoError;
		}
	}
}
#endif