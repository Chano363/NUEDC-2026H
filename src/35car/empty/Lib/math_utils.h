#ifndef _MATH_UTILS_H
#define _MATH_UTILS_H

#include <stdint.h>

float WrapAngle180(float angle);
float WrapAngle360(float angle);
float LimitValue(float value, float min, float max);
int32_t LimitIntValue(int32_t value, int32_t min, int32_t max);
float AbsFloat(float value);
float SignFloat(float value);
float MapValue(float x, float in_min, float in_max, float out_min, float out_max);

#endif