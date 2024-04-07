## AndroidAudioBase class
```c++
#include <NativeAudio/AndroidAudioBase.h>
using namespace HephAudio::Native;
```
**Inheritance:** *[NativeAudio](/docs/HephAudio/NativeAudio/NativeAudio.md)* -> *AndroidAudioBase*

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)<br>
[Protected Fields](#protected-fields)<br>
[Protected Methods](#protected-methods)


### Description
Base class for the classes that interact with the android native audio APIs. This class implements the audio device enumeration.
<br><br>

### Fields

```c++
class AndroidAudioBase : public NativeAudio
{
	static JavaVM* jvm;
}
```

- ``jvm``
<br><br>
Pointer to the java virtual machine.
Set by the ``JNI_OnLoad`` method. This method is declared with the weak attribute and can be overrided.
<br><br>



### Methods
```c++
AndroidAudioBase();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
virtual ~AndroidAudioBase();
```
Releases the resources.
<br><br><br><br>


### Protected Fields
```c++
class AndroidAudioBase : public NativeAudio
{
	uint32_t deviceApiLevel;
}
```

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
std::string JStringToString(JNIEnv* env, jstring jStr) const;
```
Converts JNI string to C string.
- **env:** Pointer to the java virtual machine.
- **jStr:** JNI string.
- **returns:** C string.
<br><br><br><br>
