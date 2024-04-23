#include "interpolators.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include <math.h>
#include <string.h>

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
		return -1719.04f;
	}
}

float Interpolators::IInterp(float start, float end, float value, Method method)
{
	switch (method)
	{
	case Interpolators::Hold:
		return start;
	case Interpolators::Constant:
		return Interpolators::fun_IConstant(start, end, value);
	case Interpolators::Linear:
		return Interpolators::fun_ILinear(start, end, value);
	case Interpolators::Cosine:
		return Interpolators::fun_ICosine(start, end, value);
	case Interpolators::Square:
		return Interpolators::fun_ISquare(start, end, value);
	case Interpolators::Cubic:
		return Interpolators::fun_ICubic(start, end, value);
	case Interpolators::SquareRoot:
		return Interpolators::fun_ISquareRoot(start, end, value);
	default:
		EDebug::Error("Invalid interpolation method.");
		return -1719.04f;
	}
}

float Interpolators::Interp(float time, Method method)
{
	return Interpolators::Interp(0.0f, 1.0f, time, method);
}

float Interpolators::IInterp(float value, Method method)
{
	return Interpolators::IInterp(0.0f, 1.0f, value, method);
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
	float t = (1.0f - (float)cos(time * PI)) * 0.5f;
	return start + (end - start) * t;
}

float Interpolators::fun_ICosine(float start, float end, float value)
{
	float t = (value - start) / (end - start);
	return (float)acos(1.0f - 2.0f * t) / PI;
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
	return (float)cbrt(t);
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

Interpolators::Method Interpolators::parseMethod(const char* method)
{
	if (strcmp(method, "Hold") == 0)
		return Interpolators::Hold;
	else if (strcmp(method, "Constant") == 0)
		return Interpolators::Constant;
	else if (strcmp(method, "Linear") == 0)
		return Interpolators::Linear;
	else if (strcmp(method, "Cosine") == 0)
		return Interpolators::Cosine;
	else if (strcmp(method, "Square") == 0)
		return Interpolators::Square;
	else if (strcmp(method, "Cubic") == 0)
		return Interpolators::Cubic;
	else if (strcmp(method, "SquareRoot") == 0)
		return Interpolators::SquareRoot;
	else
	{
		EDebug::Error("Invalid interpolation method.");
		return Interpolators::Hold;
	}
}

bool Interpolators::tryParseMethod(const char* method, Method& result)
{
	if (strcmp(method, "Hold") == 0)
		result = Interpolators::Hold;
	else if (strcmp(method, "Constant") == 0)
		result = Interpolators::Constant;
	else if (strcmp(method, "Linear") == 0)
		result = Interpolators::Linear;
	else if (strcmp(method, "Cosine") == 0)
		result = Interpolators::Cosine;
	else if (strcmp(method, "Square") == 0)
		result = Interpolators::Square;
	else if (strcmp(method, "Cubic") == 0)
		result = Interpolators::Cubic;
	else if (strcmp(method, "SquareRoot") == 0)
		result = Interpolators::SquareRoot;
	else
		return false;

	return true;
}