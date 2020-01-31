#pragma once
#include <DirectXColors.h>
#include <DirectXMath.h>

using namespace DirectX;

struct VertexData {
	XMFLOAT3 Pos;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	XMFLOAT3 BiNormal;
};