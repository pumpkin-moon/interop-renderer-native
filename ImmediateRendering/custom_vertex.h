#pragma once

struct CUSTOMVERTEX
{
    float x, y, z, rhw;    // from the D3DFVF_XYZRHW flag
    unsigned int color;    // from the D3DFVF_DIFFUSE flag
};