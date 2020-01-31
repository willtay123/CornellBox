#pragma once
#include <d3d11_1.h>

struct Shader {
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	Shader();
	Shader(ID3D11VertexShader* pVertexShader, ID3D11PixelShader* pPixelShader);
	Shader(const Shader& other);
	Shader& operator=(const Shader& rhs);
};

