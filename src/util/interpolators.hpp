#ifndef INTERPOLATORS_H
#define INTERPOLATORS_H

namespace Interpolators
{
	enum Method
	{
		/**
		 * Special option which is used to indicate a custom interpolation method.
		 * Useful for serialization and will throw an exception if used as an interpolation method.
		 */
		Hold = 0xFF,
		/** Always returns the start value. */
		Constant = 0,
		/** Interpolation using a linear function. */
		Linear = 1,
		/** Interpolation using the cosine of the time. */
		Cosine = 2,
		/** Interpolation using the square of the time. */
		Square = 3,
		/** Interpolation using the cube of the time. */
		Cubic = 4,
		/** Interpolation using the square root of the time. */
		SquareRoot = 5,
	};

	float Interp(float start, float end, float time, Method method);
	float Interp(float time, Method method);

	float IInterp(float start, float end, float value, Method method);
	float IInterp(float value, Method method);

	float fun_Constant(float start, float end, float time);
	float fun_IConstant(float start, float end, float value);

	float fun_Linear(float start, float end, float time);
	float fun_ILinear(float start, float end, float value);

	float fun_Cosine(float start, float end, float time);
	float fun_ICosine(float start, float end, float value);

	float fun_Square(float start, float end, float time);
	float fun_ISquare(float start, float end, float value);

	float fun_Cubic(float start, float end, float time);
	float fun_ICubic(float start, float end, float value);

	float fun_SquareRoot(float start, float end, float time);
	float fun_ISquareRoot(float start, float end, float value);

	Method parseMethod(const char* method);
	bool tryParseMethod(const char* method, Method& outMethod);
};

#endif // INTERPOLATORS_H