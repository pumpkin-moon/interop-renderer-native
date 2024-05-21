#pragma once
#include "imgui.h"
#include "Mat3x2.h"
#include <d3d11.h>

typedef struct sLineStyle
{
	unsigned int color;
	float thickness;
} sLineStyle;

typedef struct sCircle
{
	ImVec2 position;
	float radius;
	sLineStyle style;

	static sCircle Unpack(byte* data, int& offset, Mat3x2& matrix);
} sCircle;

typedef struct sEllipse
{
	ImVec2 position;
	ImVec2 radius;
	sLineStyle style;

	static sEllipse Unpack(byte* data, int& offset, Mat3x2& matrix);
} sEllipse;

typedef struct sLine
{
	ImVec2 from;
	ImVec2 to;
	sLineStyle style;

	static sLine Unpack(byte* data, int& offset, Mat3x2& matrix);
} sLine;

typedef struct sPolyline
{
	int numPoints;
	ImVec2* points;
	sLineStyle style;

	static sPolyline Unpack(byte* data, int& offset, Mat3x2& matrix);
} sPolyline;

typedef struct sTriangle
{
	ImVec2 a;
	ImVec2 b;
	ImVec2 c;
	sLineStyle style;

	static sTriangle Unpack(byte* data, int& offset, Mat3x2& matrix);
} sTriangle;

typedef struct sQuad
{
	ImVec2 a;
	ImVec2 b;
	ImVec2 c;
	ImVec2 d;
	sLineStyle style;

	static sQuad Unpack(byte* data, int& offset, Mat3x2& matrix);
} sQuad;

typedef struct sPolygon
{
	int numPoints;
	ImVec2* points;
	sLineStyle style;

	static sPolygon Unpack(byte* data, int& offset, Mat3x2& matrix);
} sPolygon;

typedef struct sImage
{
	ImVec2 a;
	ImVec2 b;
	ImVec2 c;
	ImVec2 d;
	ID3D11ShaderResourceView* srv;

	static sImage Unpack(byte* data, int& offset, Mat3x2& matrix);
} sImage;