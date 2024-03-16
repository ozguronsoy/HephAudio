## AndroidAudioBase class
```c++
#include <NativeAudio/AndroidAudioBase.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *AndroidAudioBase*

> [Description](#description)<br>
[Methods](#methods)<br>
[Protected Parameters](#protected-parameters)<br>
[Protected Methods](#protected-methods)


### Description
Base class for the classes that interact with the android native audio APIs.
<br><br>

### Methods
```c++
AndroidAudioBase(JavaVM* jvm);
```
Creates an instance and initializes it with the provided values.
- **jvm:** Pointer to the java virtual machine.
<br><br><br><br>

```c++
virtual ~AndroidAudioBase();
```
Releases the resources.
<br><br><br><br>


### Protected Parameters
```c++
class AndroidAudioBase : public NativeAudio
{
	JavaVM* jvm;
	uint32_t deviceApiLevel;
}
```

- ``jvm``
<br><br>
Pointer to the java virtual machine.
<br><br>

- ``deviceApiLevel``
<br><br>
The API level of the current android device.
<br><br>

### Protected Methods
```c++
void GetEnv(JNIEnv** pEnv) const;
```
Gets the pointer to the jni environment.
- **pEnv:** Double pointer to the JNI environment.
<br><br><br><br>


```c++
HephCommon::StringBuffer JStringToString(JNIEnv* env, jstring jStr) const;
```
Converts JNI string to C string.
- **env:** Pointer to the java virtual machine.
- **jStr:** JNI string.
- **returns:** C string.
<br><br><br><br>
