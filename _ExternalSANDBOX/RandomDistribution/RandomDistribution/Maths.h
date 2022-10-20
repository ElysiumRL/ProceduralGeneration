#pragma once

#include <cstdlib>
#include <ctime>
#include <vector>
#include <cassert>

class Random
{
public:

	static inline void GenerateSeed() { srand((unsigned int)time(NULL)); }

	static inline float RangeFloat(float _min, float _max) { return _min + (float)(rand()) / ((float)(RAND_MAX / (_max - _min))); }

};

class MathUtils
{
public:
	template<typename T>
	static T Sum(std::vector<T> array)
	{
		T retval = T();

		for (auto& elem : array)
		{
			retval += elem;
		}
		return retval;
	}

	static std::vector<float> ArrayMul(std::vector<float> array, float multiplier)
	{
		std::vector<float> retval = array;

		for (int i = 0; i < array.size(); i++)
		{
			retval[i] *= multiplier;
		}
		return retval;
	}

	static std::vector<float> ArrayDiv(std::vector<float> array, float divider)
	{
		std::vector<float> retval = array;
		assert(divider != 0.0f);

		for (int i = 0; i < array.size(); i++)
		{
			retval[i] /= divider;
		}
		return retval;
	}



};