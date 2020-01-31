#include "ShaderManager.h"

Shader ShaderManager::_shaders[9] = { { nullptr, nullptr} };
int ShaderManager::_usableAsRandom[5] = { 0, 1, 2, 3, 6 };
int ShaderManager::_usableAsRandomCount = 5;

ShaderManager::ShaderManager() {

}

HRESULT ShaderManager::CompileShaderFromFile(WCHAR* const szFileName, const LPCSTR szEntryPoint, const LPCSTR szShaderModel, ID3DBlob** const ppBlobOut) {
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr)) {
		if (pErrorBlob) {
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT ShaderManager::SetupVertexShader(ID3D11Device* const device, const wchar_t* const filename, const int shaderStoreIndex, ID3DBlob** const pVSBlob) {
	HRESULT hr = S_OK;

	hr = CompileShaderFromFile(const_cast<wchar_t*>(filename), "main", "vs_4_0", &(*pVSBlob));
	if (FAILED(hr)) {
		MessageBox(nullptr,
			L"The VS file cannot be compiled.  Please run this executable from the directory that contains the VS file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = device->CreateVertexShader((*pVSBlob)->GetBufferPointer(), (*pVSBlob)->GetBufferSize(), nullptr, &_shaders[shaderStoreIndex].vertexShader);
	if (FAILED(hr))
		(*pVSBlob)->Release();

	return hr;
}

HRESULT ShaderManager::SetupPixelShader(ID3D11Device* const device, const wchar_t* const filename, const int shaderStoreIndex, ID3DBlob** const pPSBlob) {
	HRESULT hr = S_OK;

	hr = CompileShaderFromFile(const_cast<wchar_t*>(filename), "main", "ps_4_0", &(*pPSBlob));
	if (FAILED(hr)) {
		MessageBox(nullptr,
			L"The PS file cannot be compiled.  Please run this executable from the directory that contains the PS file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = device->CreatePixelShader((*pPSBlob)->GetBufferPointer(), (*pPSBlob)->GetBufferSize(), nullptr, &_shaders[shaderStoreIndex].pixelShader);
	if (FAILED(hr))
		(*pPSBlob)->Release();

	return hr;
}

HRESULT ShaderManager::InitialiseShaders(ID3D11Device* const device, ID3D11DeviceContext* const context) {
	HRESULT hr;

	srand(time(0));

	ID3D11InputLayout* vertexLayout;

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	#pragma region Gouraud
	// Create the Vertex Shader
	ID3DBlob* pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"Gouraud_VS.hlsl", 0, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	ID3DBlob* pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"Gouraud_PS.hlsl", 0, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Phong
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"Phong_VS.hlsl", 1, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"Phong_PS.hlsl", 1, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Texture_Mapping
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"TextureMapping_VS.hlsl", 2, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"TextureMapping_PS.hlsl", 2, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Bump_Mapping
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"BumpMapping_VS.hlsl", 3, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"BumpMapping_PS.hlsl", 3, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Displacement_Mapping
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"DisplacementMapping_VS.hlsl", 4, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"DisplacementMapping_PS.hlsl", 4, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Toon_Shading
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"ToonShading_VS.hlsl", 5, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"ToonShading_PS.hlsl", 5, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Transparency
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"Transparency_VS.hlsl", 6, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"Transparency_PS.hlsl", 6, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Environment_Mapping
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"EnvironmentMapping_VS.hlsl", 7, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"EnvironmentMapping_PS.hlsl", 7, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	#pragma region Glowing
	// Create the Vertex Shader
	pVSBlob = nullptr;
	hr = ShaderManager::SetupVertexShader(device, L"Glowing_VS.hlsl", 8, &pVSBlob);

	if (FAILED(hr)) {
		pVSBlob->Release();
		return hr;
	}

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &vertexLayout);

	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	context->IASetInputLayout(vertexLayout);

	// Create the Pixel Shader
	pPSBlob = nullptr;
	hr = SetupPixelShader(device, L"Glowing_PS.hlsl", 8, &pPSBlob);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	#pragma endregion

	return hr;
}

int ShaderManager::GetRandomShaderIndex() {
	int indexToReturn = 0;

	const int usableIndex = (rand() % (_usableAsRandomCount));
	indexToReturn = _usableAsRandom[usableIndex];

	return indexToReturn;
}

Shader ShaderManager::GetShader(const int index) {
	return _shaders[index];
}

void ShaderManager::Cleanup() {
	for (int i = 0; i < 9; i++) {
		if (_shaders[i].vertexShader) _shaders[i].vertexShader->Release();
		if (_shaders[i].pixelShader) _shaders[i].pixelShader->Release();
	}
}
