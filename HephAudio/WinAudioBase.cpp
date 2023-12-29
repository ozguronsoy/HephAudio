#include "WinAudioBase.h"
#include "../HephCommon/HeaderFiles/HephException.h"
#include <combaseapi.h>

using namespace HephCommon;

namespace HephAudio
{
	namespace Native
	{
		WinAudioBase::WinAudioBase() : NativeAudio() {}
		void WinAudioBase::InitializeCOM() const
		{
			HRESULT hres = CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
			if (FAILED(hres))
			{
				if (hres == RPC_E_CHANGED_MODE)
				{
					hres = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE | COINIT_SPEED_OVER_MEMORY);
					if (!FAILED(hres))
					{
						return;
					}
				}
				RAISE_AND_THROW_HEPH_EXCEPTION(this, HephException(hres, "WinAudioBase::InitializeCOM", "An error occurred whilst initializing COM."));
			}
		}
	}
}