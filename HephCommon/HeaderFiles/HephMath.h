#pragma once
#include "HephCommonShared.h"
#include <cmath>
#include <cinttypes>
#include <float.h>

#define HEPH_MATH_PI 3.14159265358979323846
#define HEPH_MATH_E 2.71828182845904523536

#define HEPH_MATH_SGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

#define HEPH_MATH_MIN(a, b) ((a) > (b) ? (b) : (a))
#define HEPH_MATH_MAX(a, b) ((a) > (b) ? (a) : (b))

#define HEPH_MATH_RAD_TO_DEG(x) ((x) * (180.0 / HEPH_MATH_PI))
#define HEPH_MATH_DEG_TO_RAD(x) ((x) * (HEPH_MATH_PI / 180.0))