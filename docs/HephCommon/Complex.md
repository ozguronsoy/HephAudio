## Complex struct
```c++
#include <Complex.h>
using namespace HephCommon;
```

> [Description](#description)<br>
[Fields](#fields)<br>
[Methods](#methods)<br>
[Literals](#literals)

### Description
Struct for representing complex numbers.
<br><br>



### Fields

```c++
struct Complex final
{
    heph_float real;
    heph_float imaginary;
}
```

- **real**
<br><br>
Real part of the complex number.
<br><br>

- **imaginary**
<br><br>
Imaginary part of the complex number.
<br><br>



### Methods

```c++
constexpr Complex();
```
Creates an instance and initializes it with the default values.
<br><br><br><br>

```c++
constexpr Complex(heph_float real,
                  heph_float imaginary);
```
Creates an instance and initializes it with the provided values.
- **real:** Real part of the complex number.
- **imaginary:** Imaginary part of the complex number.
<br><br><br><br>

```c++
constexpr Complex(const Complex& rhs);
```
Creates an instance and copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
<br><br><br><br>

```c++
constexpr Complex& operator=(const Complex& rhs);
```
Copies the contents of the ``rhs``.
- **rhs:** The instance whose contents will be copied.
- **returns:** Reference to the current instance.
<br><br><br><br>

```c++
constexpr Complex operator+() const;
```
Returns a copy of the current instance.
- **returns:** Copy of the current instance.
<br><br><br><br>

```c++
constexpr Complex operator-() const;
```
Returns a new instance with inverted values.
- **returns:** New instance with inverted values.
<br><br><br><br>

```c++
constexpr Complex operator+(const Complex& rhs) const;
```
Sums two complex numbers and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator+=(const Complex& rhs);
```
Sums two complex numbers and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr Complex operator-(const Complex& rhs) const;
```
Subtracts two complex numbers and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator-=(const Complex& rhs);
```
Subtracts two complex numbers and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr Complex operator*(const Complex& rhs) const;
```
Multiplies two complex numbers and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator*=(const Complex& rhs);
```
Multiplies two complex numbers and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr Complex operator/(const Complex& rhs) const;
```
Divides two complex numbers and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator/=(const Complex& rhs);
```
Divides two complex numbers and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr bool operator==(const Complex& rhs) const;
```
Compares two complex numbers.
- **rhs:** Right hand side.
- **returns:** ``true`` if the complex numbers are equal, otherwise ``false``.
<br><br><br><br>

```c++
constexpr bool operator!=(const Complex& rhs) const;
```
Compares two complex numbers.
- **rhs:** Right hand side.
- **returns:** ``true`` if the complex numbers are not equal, otherwise ``false``.
<br><br><br><br>

```c++
constexpr Complex operator+(heph_float rhs) const;
```
Sums with a real number and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator+=(heph_float rhs);
```
Sums with a real number and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr Complex operator-(heph_float rhs) const;
```
Subtracts a real number and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator-=(heph_float rhs);
```
Subtracts a real number and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr Complex operator*(heph_float rhs) const;
```
Multiplies with a real number and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator*=(heph_float rhs);
```
Multiplies with a real number and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr Complex operator/(heph_float rhs) const;
```
Divides with a real number and stores the result in a new instance.
- **rhs:** Right hand side.
- **returns:** New instance with the operation result.
<br><br><br><br>

```c++
constexpr Complex& operator/=(heph_float rhs);
```
Divides with a real number and stores the result in the current instance.
- **rhs:** Right hand side.
- **returns:** Reference to current instance.
<br><br><br><br>

```c++
constexpr Complex Conjugate() const;
```
Calculates the complex conjugate of the number.
- **returns:** Complex conjugate.
<br><br><br><br>

```c++
constexpr heph_float MagnitudeSquared() const;
```
Calculates the squared magnitude of the complex number.
- **returns:** Magnitude squared.
<br><br><br><br>

```c++
heph_float Magnitude() const;
```
Calculates the magnitude of the complex number.
- **returns:** Magnitude.
<br><br><br><br>

```c++
heph_float Phase() const;
```
Calculates the phase angle of the complex number in radians.
- **returns:** Phase angle in radians.
<br><br><br><br>


### Literals

```c++
1_j
1.5_j
1_J
1.5_J
```
Creates a ``Complex`` instance with the real part set to 0, and the imaginary part set to the value.
