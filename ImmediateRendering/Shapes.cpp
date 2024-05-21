#include "Shapes.h"

sCircle sCircle::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto circle = *(sCircle*)(data + offset);

    circle.position = matrix.transform(circle.position);
    circle.radius *= matrix.values[0];
    circle.style.thickness *= matrix.values[0];

    offset += sizeof(sCircle);
    return circle;
}

sEllipse sEllipse::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto ellipse = *(sEllipse*)(data + offset);

    ellipse.position = matrix.transform(ellipse.position);
    ellipse.radius.x *= matrix.values[0];
    ellipse.radius.y *= matrix.values[0];
    ellipse.style.thickness *= matrix.values[0];

    offset += sizeof(sEllipse);
    return ellipse;
}

sLine sLine::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto line = *(sLine*)(data + offset);

    line.from = matrix.transform(line.from);
    line.to = matrix.transform(line.to);
    line.style.thickness *= matrix.values[0];

    offset += sizeof(sLine);
    return line;
}

sPolyline sPolyline::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto numPoints = *(int*)(data + offset);
    offset += sizeof(int);

    auto points = (ImVec2*)(data + offset);
    offset += sizeof(ImVec2) * numPoints;

    for (int i = 0; i < numPoints; ++i)
    {
        points[i] = matrix.transform(points[i]);
    }

    auto style = *(sLineStyle*)(data + offset);
    style.thickness *= matrix.values[0];
    offset += sizeof(sLineStyle);

    return { numPoints, points, style };
}

sTriangle sTriangle::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto triangle = *(sTriangle*)(data + offset);

    triangle.a = matrix.transform(triangle.a);
    triangle.b = matrix.transform(triangle.b);
    triangle.c = matrix.transform(triangle.c);
    triangle.style.thickness *= matrix.values[0];

    offset += sizeof(sTriangle);
    return triangle;
}

sQuad sQuad::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto quad = *(sQuad*)(data + offset);

    quad.a = matrix.transform(quad.a);
    quad.b = matrix.transform(quad.b);
    quad.c = matrix.transform(quad.c);
    quad.d = matrix.transform(quad.d);
    quad.style.thickness *= matrix.values[0];

    offset += sizeof(sQuad);
    return quad;
}

sPolygon sPolygon::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto numPoints = *(int*)(data + offset);
    offset += sizeof(int);

    auto points = (ImVec2*)(data + offset);
    offset += sizeof(ImVec2) * numPoints;

    for (int i = 0; i < numPoints; ++i)
    {
        points[i] = matrix.transform(points[i]);
    }

    auto style = *(sLineStyle*)(data + offset);
    style.thickness *= matrix.values[0];
    offset += sizeof(sLineStyle);

    return { numPoints, points, style };
}

sImage sImage::Unpack(byte* data, int& offset, Mat3x2& matrix)
{
    auto image = *(sImage*)(data + offset);

    image.a = matrix.transform(image.a);
    image.b = matrix.transform(image.b);
    image.c = matrix.transform(image.c);
    image.d = matrix.transform(image.d);

    offset += sizeof(sImage);
    return image;
}
