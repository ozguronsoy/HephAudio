#include "Complex.h"
#include "HephMath.h"

namespace HephCommon
{
	double Complex::Magnitude() const
	{
		return std::sqrt(this->MagnitudeSquared());
	}
	double Complex::Phase() const
	{
		return std::atan2(this->imag, this->real);
	}
}