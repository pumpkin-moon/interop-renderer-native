#pragma once
#include "d3d11.h"

union Color {
	unsigned int value;

	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
};

struct LineStyle {
	Color color;
	float thickness;
};

struct Image
{
	ID3D11ShaderResourceView* resource;
	int width;
	int height;
	int channels;
};