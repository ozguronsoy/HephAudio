## Spatializer class
```c++
#include <Spatializer.h>
using namespace HephAudio;
```

> [Description](#description)<br>
[Macros](#macros)<br>
[Methods](#methods)


<br><br>
| Supported Platforms |
| ------------------- | 
| Windows             | 
| Linux               | 
| macOS               | 


<br><br>


### Description
Implements reading the SOFA files and HRTF interpolation for spatialization.
<br><br>



### Macros

```c++
#define HEPHAUDIO_SPATIALIZER_AZIMUTH_MIN
#define HEPHAUDIO_SPATIALIZER_AZIMUTH_MAX
```
Defines the azimuth range in degrees [0, 360)
<br><br>

```c++
#define HEPHAUDIO_SPATIALIZER_ELEVATION_MIN
#define HEPHAUDIO_SPATIALIZER_ELEVATION_MAX
```
Defines the elevation range in degrees [-90, 90]
<br><br>

### Fields

```c++
Spatializer();
```
Creates an instance and initializes it with the default values. 
Reads the default SOFA file if found. If not, call the ``ReadSofaFile`` method.
<br><br><br><br>

```c++
Spatializer(const std::string& sofaFilePath);
```
Creates an instance and initializes it with the provided values.
- **sofaFilePath:** path of the SOFA file.
<br><br><br><br>

```c++
uint32_t GetSampleRate() const;
```
Gets the sampling rate of the read SOFA file.
- **returns:** sampling rate read from the SOFA file, or "0" if no file was read.
<br><br><br><br>

```c++
void ReadSofaFile(const std::string& sofaFilePath);
```
Reads the SOFA file for using the HRTF data.
- **sofaFilePath:** path of the SOFA file.
<br><br><br><br>

```c++
void Process(AudioBuffer& buffer,
             double azimuth_deg,
             double elevation_deg);
```
Spatializes the provided audio data.
- **buffer:** audio data.
- **azimuth_deg:** desired azimuth angle in degrees.
- **elevation_deg:** desired elevation angle in degrees.
<br><br><br><br>
