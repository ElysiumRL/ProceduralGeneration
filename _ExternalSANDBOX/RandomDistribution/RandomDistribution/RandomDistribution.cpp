#include <iostream>
#include <cstdlib>
#include "Maths.h"
#include <vector>

class Vector3
{
public:
	float x;
	float y;
	float z;
	Vector3() = default;
	Vector3(float _x, float _y, float _z);

public:

	inline float Length() { return sqrt(x * x + y * y + z * z); }
	inline float Area2D() { return x * y; }
	inline float Area3D() { return x * y * z; }
	inline float Perimeter2D() { return 2 * x + 2 * y; }
};

class Box
{
public:
	Vector3 center;
	Vector3 size;
};




Vector3::Vector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

class RandomDistributionSettings
{
public:
	Vector3 squareSize{};
	int roomNum;
	int minRoomSize;
	int maxRoomSize;
	RandomDistributionSettings();
};

RandomDistributionSettings::RandomDistributionSettings()
{
	squareSize = Vector3(0, 0, 0);
	roomNum = 0;
	minRoomSize = 0;
	maxRoomSize = 0;

}

int main()
{
	RandomDistributionSettings settings;
	settings.minRoomSize = 3;
	settings.maxRoomSize = 6;
	settings.roomNum = 6;
	settings.squareSize = Vector3(9,5,0);

	Random::GenerateSeed();

	std::vector<float> allRoomSizes = std::vector<float>();

	for (int i = 0; i < settings.roomNum; i++)
	{
		float randomSize = Random::RangeFloat((float)settings.minRoomSize, (float)settings.maxRoomSize);
		allRoomSizes.push_back(randomSize);
	}

	for (auto& room : allRoomSizes)
	{
		std::cout << "Room Size : " << room << std::endl;
	}
	std::vector<float> allRoomSizesNormalized;

	auto totalSize = MathUtils::Sum<float>(allRoomSizes);
	std::cout << "\n" << "Total : " << totalSize << std::endl;

	allRoomSizesNormalized = MathUtils::ArrayDiv(allRoomSizes, totalSize);

	for (auto& room : allRoomSizesNormalized)
	{
		std::cout << "Room Size Normalized : " << room << std::endl;
	}

	std::cout << "\n" << "Total : " << MathUtils::Sum<float>(allRoomSizesNormalized) << std::endl;

	std::vector<float> allRoomSizesRealSize;

	allRoomSizesRealSize = MathUtils::ArrayMul(allRoomSizesNormalized, settings.squareSize.Area2D());

	for (auto& room : allRoomSizesRealSize)
	{
		std::cout << "Room Size Real Size : " << room << std::endl;
	}

	std::cout << "\n" << "Total : " << MathUtils::Sum<float>(allRoomSizesRealSize) << std::endl;

	system("pause");

}
