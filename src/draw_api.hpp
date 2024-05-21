#pragma once
#include "imgui.h"
#include "drawing.hpp"

typedef struct DrawAPI
{
	void (*drawCircle)(ImVec2 center, float radius, LineStyle style);
	void (*drawEllipse)(ImVec2 center, ImVec2 radius, float rotation, LineStyle style);
	void (*drawLine)(ImVec2 from, ImVec2 to, LineStyle style);
	void (*drawPolyline)(ImVec2* points, int numPoints, LineStyle style);
	void (*drawTriangle)(ImVec2 a, ImVec2 b, ImVec2 c, LineStyle style);
	void (*drawQuad)(ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d, LineStyle style);
	void (*drawPolygon)(ImVec2* points, int numPoints, LineStyle style);
	void (*drawImage)(Image image, ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d);

	static DrawAPI Create();
	static void DrawCircle(ImVec2 center, float radius, LineStyle style);
	static void DrawEllipse(ImVec2 center, ImVec2 radius, float rotation, LineStyle style);
	static void DrawLine(ImVec2 from, ImVec2 to, LineStyle style);
	static void DrawPolyline(ImVec2* points, int numPoints, LineStyle style);
	static void DrawTriangle(ImVec2 a, ImVec2 b, ImVec2 c, LineStyle style);
	static void DrawQuad(ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d, LineStyle style);
	static void DrawPolygon(ImVec2* points, int numPoints, LineStyle style);
	static void DrawImage(Image image, ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d);
} DrawAPI;