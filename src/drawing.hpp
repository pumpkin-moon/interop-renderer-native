#pragma once
#include "d3d11.h"

union Color {
	unsigned int value;

	struct {
		byte r;
		byte g;
		byte b;
		byte a;
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

struct CodepointRange
{
	int min;
	int max;
};

struct Rect
{
	int x, y, w, h;
};

struct Font
{
	byte* bitmapData;
	int bitmapWidth;
	int bitmapHeight;
	Rect* glyphRects;
	CodepointRange glyphRange;
};

struct Glyph
{
	unsigned int codepoint;
	float advance;
	float leftBearing;
	byte* bitmap;
	int x;
	int y;
	int width;
	int height;
};

struct Shader
{
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
};