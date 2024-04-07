## File class
```c++
#include <File.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Enums](#enums)<br>
[Methods](#methods)


### Description
Wrapper class for file I/O operations.
<br><br>


### Enums

```c++
enum class FileOpenMode
{
    Read = 1,
    Write = 2,
    Append = 4,
    Overwrite = 8
};
```
<br><br>


### Methods

```c++
File();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
File(const std::string& filePath,
     FileOpenMode openMode);
```
Creates an instance and initializes it with the provided values.
- **filePath:** Path of the file.
- **openMode:** File open mode.
<br><br><br><br>

```c++
File(File&& rhs) noexcept;
```
Creates an instance and moves the ownership of the contents of the ``rhs`` to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
~File();
```
Closes the file and releases the resources.
<br><br><br><br>

```c++
File& operator=(File&& rhs) noexcept;
```
Releases the resources of the current buffer, then moves the ownership of the contents of ``rhs``.
- **rhs:** The instance whose contents will be moved.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
void Open(const std::string& filePath,
          FileOpenMode openMode);
```
Opens the file.
- **rhs:** Path of the file.
- **openMode:** File open mode.
<br><br><br><br>

```c++
void Close();
```
Closes the file.
<br><br><br><br>

```c++
void Flush();
```
Flushes the file.
<br><br><br><br>

```c++
bool IsOpen() const;
```
Checks whether a file is currently open.
- **returns:** ``true`` if a file is currently open, otherwise ``false``.
<br><br><br><br>

```c++
FILE* GetInternalFilePtr() const;
```
Gets the C FILE pointer.
- **returns:** FILE pointer.
<br><br><br><br>

```c++
uint64_t FileSize() const;
```
Gets the file size in bytes.
- **returns:** File size in bytes.
<br><br><br><br>

```c++
std::string FilePath() const;
```
Gets the full file path.
- **returns:** File path.
<br><br><br><br>

```c++
std::string FileName() const;
```
Gets the name of the file with extension.
- **returns:** File name.
<br><br><br><br>

```c++
std::string FileExtension() const;
```
Gets the extension of the file.
- **returns:** File extension.
<br><br><br><br>

```c++
uint64_t GetOffset() const;
```
Gets the current file position.
- **returns:** File offset.
<br><br><br><br>

```c++
void SetOffset(uint64_t offset) const;
```
Sets the file position.
- **offset:** File position in bytes.
<br><br><br><br>

```c++
void IncreaseOffset(uint64_t offset) const;
```
Increases the file position.
- **offset:** Offset that will be added to the current position in bytes.
<br><br><br><br>

```c++
void DecreaseOffset(uint64_t offset) const;
```
Decreases the file position.
- **offset:** Offset that will be subtracted from the current position in bytes.
<br><br><br><br>

```c++
void Read(void* pData,
          uint8_t dataSize,
          Endian endian) const;
```
Reads data from the file.
- **pData:** Buffer that the data will be read to. Must be allocated by the user.
- **dataSize:** Size of the data that will be read in bytes.
- **endian:** Endianness of the data that will be read.
<br><br><br><br>

```c++
void ReadToBuffer(void* pBuffer,
                  uint8_t elementSize,
                  uint32_t elementCount) const;
```
Reads data from the file.
- **pBuffer:** Buffer that the data will be read to. Must be allocated by the user.
- **elementSize:** Size of the each element that will be read in bytes.
- **elementCount:** Number of elements to read.
<br><br><br><br>

```c++
void Write(const void* pData,
           uint8_t dataSize,
           Endian endian) const;
```
Writes data to the file.
- **pData:** Data to write.
- **dataSize:** Size of the data that will be written.
- **endian:** Endianness of the data that will be written.
<br><br><br><br>

```c++
void WriteFromBuffer(const void* pData,
                     uint8_t elementSize,
                     uint32_t elementCount) const;
```
Writes data to the file.
- **pData:** Data to write.
- **elementSize:** Size of the each element that will be written in bytes.
- **elementCount:** Number of elements to write.
<br><br><br><br>

```c++
static bool FileExists(std::string filePath);
```
Checks whether a file exists at the provided path.
- **filePath:** Path that will be checked.
- **returns:** ``true`` if the file exists, otherwise ``false``.
<br><br><br><br>

```c++
static std::string GetFileName(const std::string& filePath);
```
Gets the name of the file with extension.
- **filePath:** File path.
- **returns:** File name.
<br><br><br><br>

```c++
static std::string GetFileExtension(const std::string& filePath);
```
Gets the extension of the file.
- **filePath:** File path.
- **returns:** File extension.
<br><br><br><br>
