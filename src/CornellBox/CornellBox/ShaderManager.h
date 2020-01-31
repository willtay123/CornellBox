#pragma once
#include <d3dcompiler.h>
#include <ctime>
#include <cstdlib>
#include "Shader.h"

class ShaderManager {
private:
	ShaderManager();

	static Shader _shaders[9];
	static int _usableAsRandom[5];
	static int _usableAsRandomCount;

	static HRESULT CompileShaderFromFile(WCHAR* const szFileName, const LPCSTR szEntryPoint, const LPCSTR szShaderModel, ID3DBlob** const ppBlobOut);
	static HRESULT SetupVertexShader(ID3D11Device* const device, const wchar_t* const filename, const int shaderStoreIndex, ID3DBlob** const pVSBlob);
	static HRESULT SetupPixelShader(ID3D11Device* const device, const wchar_t* const filename, const int shaderStoreIndex, ID3DBlob** const pPSBlob);

public:
	static HRESULT InitialiseShaders(ID3D11Device* const device, ID3D11DeviceContext* const context);
	static int GetRandomShaderIndex();
	static Shader GetShader(const int index);

	static void Cleanup();
};

