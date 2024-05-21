#pragma once
#include "imgui.h"

typedef struct Circle
{
    ImVec2 pos;
    float radius;
    float thickness;
    unsigned int color;
} Circle;

typedef struct Line
{
    ImVec2 from;
    ImVec2 to;
    float thickness;
    unsigned int color;
} Line;

typedef struct Text
{
    ImVec2 pos;
    char* text;
    unsigned int color;
} Text;

typedef struct Image
{
    ID3D11ShaderResourceView* resource;
    ImVec2 position;
    ImVec2 size;
} Image;