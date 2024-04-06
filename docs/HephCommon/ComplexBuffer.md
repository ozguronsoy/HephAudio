## ComplexBuffer class
```c++
#include <ComplexBuffer.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Methods](#methods)



### Description
Class for storing [Complex](/docs/HephCommon/Complex.md) data.
<br><br>


### Methods

```c++
ComplexBuffer();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>


```c++
ComplexBuffer(size_t elementCount);
```
Creates an instance and initializes it with the provided values.
- **elementCount:** Number of elements buffer will contain.
<br><br><br><br>

```c++
ComplexBuffer(const std::initializer_list<heph_float>& rhs);
```
Creates an instance and initializes it with the provided values.
- **rhs:** Elements.
<br><br><br><br>

```c++
ComplexBuffer(const std::initializer_list<Complex>& rhs);
```
Creates an instance and initializes it with the provided values.
- **rhs:** Elements.
<br><br><br><br>

```c++
ComplexBuffer(std::nullptr_t rhs);
```
Same as the default constructor.
<br><br><br><br>

```c++
ComplexBuffer(const FloatBuffer& rhs);
```
Creates an instance and copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
<br><br><br><br>

```c++
ComplexBuffer(const ComplexBuffer& rhs);
```
Creates an instance and copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
<br><br><br><br>

```c++
ComplexBuffer(ComplexBuffer&& rhs) noexcept;
```
Creates an instance and moves the ownership of the contents of the ``rhs`` to it.
- **rhs:** The instance whose contents will be moved.
<br><br><br><br>

```c++
~ComplexBuffer();
```
Releases the resources.
<br><br><br><br>

```c++
Complex& operator[](size_t elementIndex) const;
```
Gets the reference to the element at the provided index.
- **elementIndex:** Index of the element.
- **returns:** Reference to the desired element.
<br><br><br><br>

```c++
ComplexBuffer operator-() const;
```
Creates a new instance that stores the inverted version of the buffer.
- **returns:** Inverted buffer.
<br><br><br><br>

```c++
ComplexBuffer& operator=(const std::initializer_list<heph_float>& rhs);
```
Releases the resources of the current buffer, then copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator=(const std::initializer_list<Complex>& rhs);
```
Releases the resources of the current buffer, then copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator=(std::nullptr_t rhs);
```
Releases the resources of the current buffer.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator=(const FloatBuffer& rhs);
```
Releases the resources of the current buffer, then copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator=(const ComplexBuffer& rhs);
```
Releases the resources of the current buffer, then copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator=(ComplexBuffer&& rhs) noexcept;
```
Releases the resources of the current buffer, then moves the ownership of the contents of ``rhs``.
- **rhs:** The instance whose contents will be moved.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer operator+(heph_float rhs) const;
```
Adds a constant to each element and returns the result in a new instance.
- **rhs:** Constant value that will be added to each element.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator+(const Complex& rhs) const;
```
Adds a constant to each element and returns the result in a new instance.
- **rhs:** Constant value that will be added to each element.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator+(const ComplexBuffer& rhs) const;
```
Performs a point-by-point summation and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator+(const FloatBuffer& rhs) const;
```
Performs a point-by-point summation and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer& operator+=(heph_float rhs);
```
Adds a constant to each element and stores the result in the current instance.
- **rhs:** Constant value that will be added to each element.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator+=(const Complex& rhs);
```
Adds a constant to each element and stores the result in the current instance.
- **rhs:** Constant value that will be added to each element.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator+=(const ComplexBuffer& rhs);
```
Performs a point-by-point summation and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator+=(const FloatBuffer& rhs);
```
Performs a point-by-point summation and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer operator-(heph_float rhs) const;
```
Subtracts a constant from each element and returns the result in a new instance.
- **rhs:** Constant value that will be subtracted from each element.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator-(const Complex& rhs) const;
```
Subtracts a constant from each element and returns the result in a new instance.
- **rhs:** Constant value that will be subtracted from each element.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator-(const ComplexBuffer& rhs) const;
```
Performs a point-by-point subtraction and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator-(const FloatBuffer& rhs) const;
```
Performs a point-by-point subtraction and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer& operator-=(heph_float rhs);
```
Subtracts a constant from each element and stores the result in the current instance.
- **rhs:** Constant value that will be subtracted from each element.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator-=(const Complex& rhs);
```
Subtracts a constant from each element and stores the result in the current instance.
- **rhs:** Constant value that will be subtracted from each element.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator-=(const ComplexBuffer& rhs);
```
Performs a point-by-point subtraction and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator-=(const FloatBuffer& rhs);
```
Performs a point-by-point subtraction and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer operator*(heph_float rhs) const;
```
Multiplies a constant with each element and returns the result in a new instance.
- **rhs:** Constant value that will be multiplied with each element.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator*(const Complex& rhs) const;
```
Multiplies a constant with each element and returns the result in a new instance.
- **rhs:** Constant value that will be multiplied with each element.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator*(const ComplexBuffer& rhs) const;
```
Performs a point-by-point multiplication and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator*(const FloatBuffer& rhs) const;
```
Performs a point-by-point multiplication and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer& operator*=(heph_float rhs);
```
Multiplies a constant from each element and stores the result in the current instance.
- **rhs:** Constant value that will be multiplied with each element.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator*=(const Complex& rhs);
```
Multiplies a constant from each element and stores the result in the current instance.
- **rhs:** Constant value that will be multiplied with each element.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator*=(const ComplexBuffer& rhs);
```
Performs a point-by-point multiplication and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator*=(const FloatBuffer& rhs);
```
Performs a point-by-point multiplication and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer operator/(heph_float rhs) const;
```
Divides each element by a constant and returns the result in a new instance.
- **rhs:** Constant value that each element will be divided with.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator/(const Complex& rhs) const;
```
Divides each element by a constant and returns the result in a new instance.
- **rhs:** Constant value that each element will be divided with.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator/(const ComplexBuffer& rhs) const;
```
Performs a point-by-point division and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer operator/(const FloatBuffer& rhs) const;
```
Performs a point-by-point division and returns the result in a new instance. 
- **rhs:** Buffer that will be used in the operation.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer& operator/=(heph_float rhs);
```
Divides each element by a constant and returns the result in the current instance.
- **rhs:** Constant value that each element will be divided with.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator/=(const Complex& rhs);
```
Divides each element by a constant and returns the result in the current instance.
- **rhs:** Constant value that each element will be divided with.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator/=(const ComplexBuffer& rhs);
```
Performs a point-by-point division and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer& operator/=(const FloatBuffer& rhs);
```
Performs a point-by-point division and stores the result in the current instance.
- **rhs:** Buffer that will be used in the operation.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer operator<<(size_t rhs) const;
```
Shifts the buffer to the left and returns the result in a new instance. 
The elements at the end of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``element_5 -> element_2``.
- **rhs:** Number of elements to shift.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer& operator<<=(size_t rhs) const;
```
Shifts the buffer to the left and returns the result in the current instance. 
The elements at the end of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``element_5 -> element_2``.
- **rhs:** Number of elements to shift.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
ComplexBuffer operator>>(size_t rhs) const;
```
Shifts the buffer to the right and returns the result in a new instance. 
The elements at the beginning of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``element_2 -> element_5``.
- **rhs:** Number of elements to shift.
- **returns:**  A new buffer that stores the operation result.
<br><br><br><br>

```c++
ComplexBuffer& operator>>=(size_t rhs) const;
```
Shifts the buffer to the right and returns the result in the current instance. 
The elements at the beginning of the buffer will be set to zero.<br>
Example: for ``rhs = 3``, ``element_2 -> element_5``.
- **rhs:** Number of elements to shift.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
bool operator==(std::nullptr_t rhs) const;
```
Checks whether the buffer is empty.
- **returns:** ``true`` if the buffer is empty, otherwise ``false``.
<br><br><br><br>

```c++
bool operator==(const ComplexBuffer& rhs) const;
```
Compares the contents of two buffers.
- **rhs:** Instance that will be compared to.
- **returns:** ``true`` if the contents are equal, otherwise ``false``.
<br><br><br><br>

```c++
bool operator!=(std::nullptr_t rhs) const;
```
Checks whether the buffer is not empty.
- **returns:** ``true`` if the buffer is not empty, otherwise ``false``.
<br><br><br><br>

```c++
bool operator!=(const ComplexBuffer& rhs) const;
```
Compares the contents of two buffers.
- **rhs:** Instance that will be compared to.
- **returns:** ``true`` if the contents are not equal, otherwise ``false``.
<br><br><br><br>

```c++
size_t Size() const;
```
Calculates the size, in bytes, of the buffer.
- **returns:** Size of the buffer in bytes.
<br><br><br><br>

```c++
Complex& At(size_t frameIndex) const;
```
Same as the ``[]`` operator but checks the boundaries.
<br><br><br><br>

```c++
size_t FrameCount() const;
```
Gets the number of elements the buffer contains.
- **returns:** Number of elements the buffer contains.
<br><br><br><br>

```c++
ComplexBuffer GetSubBuffer(size_t frameIndex, size_t frameCount) const;
```
Gets the specified part of the buffer.
- **frameIndex:** Index of the first element of the sub-buffer.
- **frameCount:** Number of elements the sub-buffer will contain.
- **returns:** Sub-buffer.
<br><br><br><br>

```c++
void Append(const ComplexBuffer& buffer);
```
Adds the provided elements at the end of the current buffer.
- **buffer:** Elements that will be appended.
<br><br><br><br>

```c++
void Insert(const ComplexBuffer& buffer, size_t frameIndex);
```
Adds the provided elements to the provided index of the current buffer.
- **buffer:** Elements that will be inserted.
- **frameIndex:** Index where the buffer will be inserted.
<br><br><br><br>

```c++
void Cut(size_t frameIndex, size_t frameCount);
```
Removes the specified part of the buffer.
- **frameIndex:** Index of the first element that will be removed.
- **frameCount:** Number of elements to remove.
<br><br><br><br>

```c++
void Replace(const ComplexBuffer& buffer, size_t frameIndex);
```
Replaces the specified part of the buffer with the provided one.
- **buffer:** Elements that will be added.
- **frameIndex:** Index of the first element that will be replaced.
<br><br><br><br>

```c++
void Replace(const ComplexBuffer& buffer, size_t frameIndex, size_t frameCount);
```
Replaces the specified part of the buffer with the provided one.
- **buffer:** Elements that will be added.
- **frameIndex:** Index of the first element that will be replaced.
- **frameCount:** Number of elements to replace.
<br><br><br><br>

```c++
void Reset();
```
Sets the value of all elements to zero.
<br><br><br><br>

```c++
void Resize(size_t newFrameCount);
```
Changes the size of the buffer. 
If the buffer is expanded, new elements will be initialized to zero. 
- **newFrameCount:** New size of the buffer in terms of elements.
<br><br><br><br>

```c++
void Empty();
```
Releases the resources and sets all of the fields to their default values.
<br><br><br><br>

```c++
ComplexBuffer Convolve(const ComplexBuffer& h) const;
```
Calculates the convolution.
- **h:** rhs.
- **returns:** Convolution result.
<br><br><br><br>

```c++
ComplexBuffer Convolve(const ComplexBuffer& h, ConvolutionMode convolutionMode) const;
```
Calculates the convolution.
- **h:** rhs.
- **convolutionMode:** Convolution mode.
- **returns:** Convolution result.
<br><br><br><br>

```c++
Complex* Begin() const;
```
Gets the address of the first element.
- **returns:** Address of the first element if there are any, otherwise ``nullptr``.
<br><br><br><br>

```c++
Complex* End() const;
```
Gets the address of the end of the buffer.
- **returns:** Address of the end of the buffer.
<br><br><br><br>
