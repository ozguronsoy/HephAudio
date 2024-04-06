## AudioChannelMixingLookupTables class
```c++
#include <AudioChannelMixingLookupTables.h>
using namespace HephAudio;
```


### Description
Contains the lookup tables used for up-mixing/down-mixing. 
<br><br>

> [!NOTE]
> This class only contains static methods and cannot be instantiated.



### Fields

```c++
class AudioChannelMixingLookupTables
{
    static hephaudio_channel_mixing_lookup_table_t _mono_table;
    static hephaudio_channel_mixing_lookup_table_t _stereo_table;
    static hephaudio_channel_mixing_lookup_table_t _3_channels_table;
    static hephaudio_channel_mixing_lookup_table_t _other_channels_table;
};
```
