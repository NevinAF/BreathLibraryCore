#include "pch.h"
#include "interpolators.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include <math.h>

#define PI 3.14159265358979323846f

float Interpolators::Interp(float start, float end, float time, Method method)
{
	switch (method)
	{
	case Interpolators::Hold:
		return start;
	case Interpolators::Constant:
		return Interpolators::fun_Constant(start, end, time);
	case Interpolators::Linear:
		return Interpolators::fun_Linear(start, end, time);
	case Interpolators::Cosine:
		return Interpolators::fun_Cosine(start, end, time);
	case Interpolators::Square:
		return Interpolators::fun_Square(start, end, time);
	case Interpolators::Cubic:
		return Interpolators::fun_Cubic(start, end, time);
	case Interpolators::SquareRoot:
		return Interpolators::fun_SquareRoot(start, end, time);
	default:
		EDebug::Error("Invalid interpolation method.");
	}
}

float Interpolators::Interp(float time, Method method)
{
	return Interpolators::Interp(0.0f, 1.0f, time, method);
}

float Interpolators::fun_Constant(float start, float end, float time)
{
	return start;
}

float Interpolators::fun_IConstant(float start, float end, float value)
{
	return start;
}

float Interpolators::fun_Linear(float start, float end, float time)
{
	return start + (end - start) * time;
}

float Interpolators::fun_ILinear(float start, float end, float value)
{
	return (value - start) / (end - start);
}

float Interpolators::fun_Cosine(float start, float end, float time)
{
	float t = (1.0f - cosf(time * PI)) * 0.5f;
	return start + (end - start) * t;
}

float Interpolators::fun_ICosine(float start, float end, float value)
{
	float t = (value - start) / (end - start);
	return acosf(1.0f - 2.0f * t) / PI;
}

float Interpolators::fun_Square(float start, float end, float time)
{
	return start + (end - start) * time * time;
}

float Interpolators::fun_ISquare(float start, float end, float value)
{
	float t = (value - start) / (end - start);
	return sqrtf(t);
}

float Interpolators::fun_Cubic(float start, float end, float time)
{
	return start + (end - start) * time * time * time;
}

float Interpolators::fun_ICubic(float start, float end, float value)
{
	float t = (value - start) / (end - start);
	return cbrtf(t);
}

float Interpolators::fun_SquareRoot(float start, float end, float time)
{
	return start + (end - start) * sqrtf(time);
}

float Interpolators::fun_ISquareRoot(float start, float end, float value)
{
	float t = (value - start) / (end - start);
	return t * t;
}