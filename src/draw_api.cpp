#include "draw_api.hpp"
#include "immediate_api.hpp"

Mat3x2 cameraMatrix = { 1, 0, 0, 1, 0 ,0 };

DrawAPI DrawAPI::Create()
{
	DrawAPI api;
	api.setMatrix = SetMatrix;
	api.drawCircle = DrawCircle;
	api.drawEllipse = DrawEllipse;
	api.drawLine = DrawLine;
	api.drawPolyline = DrawPolyline;
	api.drawTriangle = DrawTriangle;
	api.drawQuad = DrawQuad;
	api.drawPolygon = DrawPolygon;
	api.drawImage = DrawImage;
	return api;
}

void DrawAPI::SetMatrix(Mat3x2 matrix)
{
	cameraMatrix = matrix;
}

void DrawAPI::DrawCircle(ImVec2 center, float radius, LineStyle style)
{
	center = cameraMatrix.transform(center);
	radius *= cameraMatrix.values[0];
	style.thickness *= cameraMatrix.values[0];

	ImmediateAPI::GetDrawList()->AddCircle(center, radius, style.color.value, 0, style.thickness);
}

void DrawAPI::DrawEllipse(ImVec2 center, ImVec2 radius, float rotation, LineStyle style)
{
	center = cameraMatrix.transform(center);
	radius.x *= cameraMatrix.values[0];
	radius.y *= cameraMatrix.values[0];
	style.thickness *= cameraMatrix.values[0];

	ImmediateAPI::GetDrawList()->AddEllipse(center, radius, style.color.value, rotation, 0, style.thickness);
}

void DrawAPI::DrawLine(ImVec2 from, ImVec2 to, LineStyle style)
{
	from = cameraMatrix.transform(from);
	to = cameraMatrix.transform(to);
	style.thickness *= cameraMatrix.values[0];

	ImmediateAPI::GetDrawList()->AddLine(from, to, style.color.value, style.thickness);
}

void DrawAPI::DrawPolyline(ImVec2* points, int numPoints, LineStyle style)
{
	for (int i = 0; i < numPoints; ++i) 
	{
		points[i] = cameraMatrix.transform(points[i]);
	}

	style.thickness *= cameraMatrix.values[0];

	ImmediateAPI::GetDrawList()->AddPolyline(points, numPoints, style.color.value, ImDrawFlags_None, style.thickness);
}

void DrawAPI::DrawTriangle(ImVec2 a, ImVec2 b, ImVec2 c, LineStyle style)
{
	a = cameraMatrix.transform(a);
	b = cameraMatrix.transform(b);
	c = cameraMatrix.transform(c);
	style.thickness *= cameraMatrix.values[0];

	ImmediateAPI::GetDrawList()->AddTriangle(a, b, c, style.color.value, style.thickness);
}

void DrawAPI::DrawQuad(ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d, LineStyle style)
{
	a = cameraMatrix.transform(a);
	b = cameraMatrix.transform(b);
	c = cameraMatrix.transform(c);
	d = cameraMatrix.transform(d);
	style.thickness *= cameraMatrix.values[0];

	ImmediateAPI::GetDrawList()->AddQuad(a, b, c, d, style.color.value, style.thickness);
}

void DrawAPI::DrawPolygon(ImVec2* points, int numPoints, LineStyle style)
{
	for (int i = 0; i < numPoints; ++i)
	{
		points[i] = cameraMatrix.transform(points[i]);
	}

	style.thickness *= cameraMatrix.values[0];

	ImmediateAPI::GetDrawList()->AddPolyline(points, numPoints, style.color.value, ImDrawFlags_Closed, style.thickness);
}

void DrawAPI::DrawImage(Image image, ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d)
{
	a = cameraMatrix.transform(a);
	b = cameraMatrix.transform(b);
	c = cameraMatrix.transform(c);
	d = cameraMatrix.transform(d);

	ImmediateAPI::GetDrawList()->AddImageQuad(image.resource, a, b, c, d);
}
