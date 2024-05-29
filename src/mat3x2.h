#pragma once
#include "imgui.h"

typedef struct Mat3x2
{
	float values[3 * 2];

	ImVec2 transform(ImVec2 point)
	{
		auto result = ImVec2(values[0] * point.x, values[1] * point.x);
		result = ImVec2(result.x + values[2] * point.y, result.y + values[3] * point.y);
		return ImVec2(result.x + values[4], result.y + values[5]);
	}

	ImVec2 transformNormal(ImVec2 normal)
	{
		auto result = ImVec2(values[0] * normal.x, values[1] * normal.x);
		return ImVec2(result.x + values[2] * normal.y, result.y + values[3] * normal.y);
	}
} Mat3x2;