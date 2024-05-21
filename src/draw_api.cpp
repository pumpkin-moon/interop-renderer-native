#include "draw_api.hpp"
#include "immediate_api.hpp"

DrawAPI DrawAPI::Create()
{
	DrawAPI api;
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

void DrawAPI::DrawCircle(ImVec2 center, float radius, LineStyle style)
{
	ImmediateAPI::GetDrawList()->AddCircle(center, radius, style.color.value, 0, style.thickness);
}

void DrawAPI::DrawEllipse(ImVec2 center, ImVec2 radius, float rotation, LineStyle style)
{
	ImmediateAPI::GetDrawList()->AddEllipse(center, radius, style.color.value, rotation, 0, style.thickness);
}

void DrawAPI::DrawLine(ImVec2 from, ImVec2 to, LineStyle style)
{
	ImmediateAPI::GetDrawList()->AddLine(from, to, style.color.value, style.thickness);
}

void DrawAPI::DrawPolyline(ImVec2* points, int numPoints, LineStyle style)
{
	ImmediateAPI::GetDrawList()->AddPolyline(points, numPoints, style.color.value, ImDrawFlags_None, style.thickness);
}

void DrawAPI::DrawTriangle(ImVec2 a, ImVec2 b, ImVec2 c, LineStyle style)
{
	ImmediateAPI::GetDrawList()->AddTriangle(a, b, c, style.color.value, style.thickness);
}

void DrawAPI::DrawQuad(ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d, LineStyle style)
{
	ImmediateAPI::GetDrawList()->AddQuad(a, b, c, d, style.color.value, style.thickness);
}

void DrawAPI::DrawPolygon(ImVec2* points, int numPoints, LineStyle style)
{
	ImmediateAPI::GetDrawList()->AddPolyline(points, numPoints, style.color.value, ImDrawFlags_Closed, style.thickness);
}

void DrawAPI::DrawImage(Image image, ImVec2 a, ImVec2 b, ImVec2 c, ImVec2 d)
{
	ImmediateAPI::GetDrawList()->AddImageQuad(image.resource, a, b, c, d);
}
