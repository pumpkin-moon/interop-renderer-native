#include "drawing.hpp"

DrawListAPI DrawListAPI::Create()
{
	DrawListAPI api;

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

void DrawListAPI::DrawCircle(ImVec2 center, float radius, LineStyle style)
{
	ImmediateAPI::drawList->AddCircle(center, radius, style.color.value, 0, style.thickness);
}

void DrawListAPI::DrawEllipse(ImVec2 center, ImVec2 radius, LineStyle style)
{
	ImmediateAPI::drawList->AddEllipse(center, radius, style.color.value, 0, style.thickness);
}

void DrawListAPI::DrawLine(ImVec2 from, ImVec2 to, LineStyle style)
{
	ImmediateAPI::drawList->AddLine(from, to, style.color.value, style.thickness);
}

void DrawListAPI::DrawPolyline(ImVec2* points, int numPoints, LineStyle style)
{
	ImmediateAPI::drawList->AddPolyline(points, numPoints, style.color.value, ImDrawFlags_None, style.thickness);
}

void DrawListAPI::DrawTriangle(ImVec2 a, ImVec2 b, ImVec2 c, LineStyle style)
{
	ImmediateAPI::drawList->AddTriangle(a, b, c, style.color.value, style.thickness);
}

void DrawListAPI::DrawQuad(ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d, LineStyle style)
{
	ImmediateAPI::drawList->AddQuad(a, b, c, d, style.color.value, style.thickness);
}

void DrawListAPI::DrawPolygon(ImVec2* points, int numPoints, LineStyle style)
{
	ImmediateAPI::drawList->AddPolyline(points, numPoints, style.color.value, ImDrawFlags_Closed, style.thickness);
}

void DrawListAPI::DrawImage(Image image, ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d)
{
	ImmediateAPI::drawList->AddImageQuad(image.resource, a, b, c, d);
}
