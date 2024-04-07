## UserEventArgs class
```c++
#include <UserEventArgs.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Methods](#methods)



### Description
Class for storing extra event arguments as key/value pairs. Wrapper class for an unordered map.

> [!IMPORTANT]
> This class only stores the pointers to the arguments.
> So it's your responsibility to ensure that the arguments still exist when handling the events.

<br><br>



### Methods

```c++
void* operator[](const std::string& key) const;
```
Gets the argument with the provided key.
- **key:** Key to find the desired argument.
- **returns:** Pointer to the argument.
<br><br><br><br>

```c++
size_t Size() const;
```
Gets the number of arguments stored.
- **returns:** Number of arguments stored.
<br><br><br><br>

```c++
bool Exists(const std::string& key) const;
```
Checks whether an argument exists with the provided key.
- **key:** Key that will be checked.
- **returns:** ``true`` if the argument exists, otherwise ``false``.
<br><br><br><br>

```c++
void Add(const std::string& key,
         void* pUserArg);
```
Adds a new argument.
- **key:** Key of the argument.
- **pUserArg:** Pointer to the argument that will be added.
<br><br><br><br>

```c++
void Remove(const std::string& key);
```
Removes the argument with the provided key.
- **key:** Key of the argument that will be removed.
<br><br><br><br>

```c++
void Clear();
```
Removes all of the arguments.
