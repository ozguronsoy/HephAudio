## AudioChannelLayout struct
```c++
#include <AudioChannelLayout.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Enums](#enums)<br>
[Fields](#fields)<br>
[Methods](#methods)


### Description
Holds information about how audio samples are stored in an audio frame.
<br><br>


### Macros
```c++
#define HEPHAUDIO_CH_MASK_MONO
#define HEPHAUDIO_CH_MASK_STEREO
#define HEPHAUDIO_CH_MASK_2_POINT_1
#define HEPHAUDIO_CH_MASK_2_1
#define HEPHAUDIO_CH_MASK_SURROUND
#define HEPHAUDIO_CH_MASK_3_POINT_1
#define HEPHAUDIO_CH_MASK_4_POINT_0
#define HEPHAUDIO_CH_MASK_2_2
#define HEPHAUDIO_CH_MASK_QUAD
#define HEPHAUDIO_CH_MASK_4_POINT_1
#define HEPHAUDIO_CH_MASK_5_POINT_0
#define HEPHAUDIO_CH_MASK_5_POINT_0_BACK
#define HEPHAUDIO_CH_MASK_5_POINT_1
#define HEPHAUDIO_CH_MASK_5_POINT_1_BACK
#define HEPHAUDIO_CH_MASK_6_POINT_0
#define HEPHAUDIO_CH_MASK_6_POINT_0_FRONT
#define HEPHAUDIO_CH_MASK_HEXAGONAL
#define HEPHAUDIO_CH_MASK_6_POINT_1
#define HEPHAUDIO_CH_MASK_6_POINT_1_BACK
#define HEPHAUDIO_CH_MASK_6_POINT_1_FRONT
#define HEPHAUDIO_CH_MASK_7_POINT_0
#define HEPHAUDIO_CH_MASK_7_POINT_0_FRONT
#define HEPHAUDIO_CH_MASK_7_POINT_1
#define HEPHAUDIO_CH_MASK_7_POINT_1_WIDE
#define HEPHAUDIO_CH_MASK_7_POINT_1_WIDE_BACK
#define HEPHAUDIO_CH_MASK_OCTAGONAL
```
Commonly used channel masks.
<br><br>

```c++
#define HEPHAUDIO_CH_LAYOUT_MONO
#define HEPHAUDIO_CH_LAYOUT_STEREO 
#define HEPHAUDIO_CH_LAYOUT_2_POINT_1
#define HEPHAUDIO_CH_LAYOUT_2_1 
#define HEPHAUDIO_CH_LAYOUT_SURROUND 
#define HEPHAUDIO_CH_LAYOUT_3_POINT_1
#define HEPHAUDIO_CH_LAYOUT_4_POINT_0
#define HEPHAUDIO_CH_LAYOUT_2_2 
#define HEPHAUDIO_CH_LAYOUT_QUAD 
#define HEPHAUDIO_CH_LAYOUT_4_POINT_1 
#define HEPHAUDIO_CH_LAYOUT_5_POINT_0 
#define HEPHAUDIO_CH_LAYOUT_5_POINT_0_BACK
#define HEPHAUDIO_CH_LAYOUT_5_POINT_1 
#define HEPHAUDIO_CH_LAYOUT_5_POINT_1_BACK
#define HEPHAUDIO_CH_LAYOUT_6_POINT_0 
#define HEPHAUDIO_CH_LAYOUT_6_POINT_0_FRONT
#define HEPHAUDIO_CH_LAYOUT_HEXAGONAL 
#define HEPHAUDIO_CH_LAYOUT_6_POINT_1 
#define HEPHAUDIO_CH_LAYOUT_6_POINT_1_BACK
#define HEPHAUDIO_CH_LAYOUT_6_POINT_1_FRONT
#define HEPHAUDIO_CH_LAYOUT_7_POINT_0
#define HEPHAUDIO_CH_LAYOUT_7_POINT_0_FRONT
#define HEPHAUDIO_CH_LAYOUT_7_POINT_1
#define HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE
#define HEPHAUDIO_CH_LAYOUT_7_POINT_1_WIDE_BACK
#define HEPHAUDIO_CH_LAYOUT_OCTAGONAL
```
Commonly used channel layouts.
<br><br>


### Enums

```c++
enum class AudioChannelMask : uint32_t
{
	Unknown,
	FrontLeft,
	FrontRight,
	FrontCenter,
	LowFrequency,
	BackLeft,
	BackRight,
	FrontLeftOfCenter,
	FrontRightOfCenter,
	BackCenter,
	SideLeft,
	SideRight,
	TopCenter,
	TopFrontLeft,
	TopFrontCenter,
	TopFrontRight,
	TopBackLeft,
	TopBackCenter,
	TopBackRight
};
```
Bitmask for audio channels.
<br><br>


### Fields

```c++
struct AudioChannelLayout
{
	uint16_t count;
	AudioChannelMask mask;
};
```

- ``count``
<br><br>
Number of channels.
<br><br>

- ``mask``
<br><br>
Number of bits set must be equal to the number of channels.
<br><br>

### Methods

```c++
constexpr AudioChannelLayout();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
constexpr AudioChannelLayout(uint16_t count, AudioChannelMask mask);
```
Creates an instance and initializes it with the provided values.
<br><br><br><br>

```c++
constexpr bool operator==(const AudioChannelLayout& rhs) const;
```
Compares the contents of two instances.
- **rhs:** Instance that will be compared to.
- **returns:** ``true`` if the contents are equal, otherwise ``false``.
<br><br><br><br>

```c++
constexpr bool operator!=(const AudioChannelLayout& rhs) const;
```
Compares the contents of two instances.
- **rhs:** Instance that will be compared to.
- **returns:** ``true`` if the contents are not equal, otherwise ``false``.
<br><br><br><br>

```c++
static constexpr AudioChannelMask DefaultChannelMask(uint16_t channelCount);
```
Gets the default channel mask for the provided channel count.
- **channelCount:** Number of channels.
- **returns:** The default channel mask.
<br><br><br><br>

```c++
static constexpr AudioChannelLayout DefaultChannelLayout(uint16_t channelCount);
```
Gets the default channel layout for the provided channel count.
- **channelCount:** Number of channels.
- **returns:** The default channel layout.
