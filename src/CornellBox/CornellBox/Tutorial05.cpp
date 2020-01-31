//--------------------------------------------------------------------------------------
// File: Tutorial05.cpp
//
// This application demonstrates animation using matrix transformations
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729722.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

#include "Keyboard.h"
#include "VertexData.h"
#include "Model.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "ShaderManager.h"
#include "ConfigManager.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
using std::vector;

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct ConstantBuffer {
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMVECTOR vEyePos;
	XMVECTOR vLightPos;
	XMVECTOR vLightCol;
};

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE					g_hInst = nullptr;
HWND						g_hWnd = nullptr;
D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*				g_pd3dDevice = nullptr;
ID3D11Device1*				g_pd3dDevice1 = nullptr;
ID3D11DeviceContext*		g_pImmediateContext = nullptr;
ID3D11DeviceContext1*		g_pImmediateContext1 = nullptr;
IDXGISwapChain*				g_pSwapChain = nullptr;
IDXGISwapChain1*			g_pSwapChain1 = nullptr;
ID3D11RenderTargetView*		g_pRenderTargetView = nullptr;
ID3D11Texture2D*			g_pDepthStencil = nullptr;
ID3D11DepthStencilView*		g_pDepthStencilView = nullptr;
ID3D11InputLayout*			g_pVertexLayout = nullptr;
ID3D11Buffer*				g_pVertexBuffer = nullptr;
ID3D11Buffer*				g_pIndexBuffer = nullptr;
ID3D11Buffer*				g_pConstantBuffer = nullptr;

ID3D11RasterizerState*		g_pInverseRasterizerState = nullptr;
ID3D11RasterizerState*		g_pRasterizerState = nullptr;
ID3D11RasterizerState*		g_pNoCullRasterizerState = nullptr;

ID3D11BlendState*			g_pBlendDesc = nullptr;
ID3D11BlendState*			g_pNoBlendDesc = nullptr;

ID3D11ShaderResourceView*	g_pWoodTextureRV = nullptr;
ID3D11ShaderResourceView*	g_pStoneTextureRV = nullptr;
ID3D11ShaderResourceView*	g_pStoneNormalTextureRV = nullptr;
ID3D11ShaderResourceView*	g_pSkymapTextureRV = nullptr;

ID3D11SamplerState*			g_pWoodSampler = nullptr;
ID3D11SamplerState*			g_pStoneSampler = nullptr;
ID3D11SamplerState*			g_pSkymapSampler = nullptr;

XMMATRIX					g_Projection;

int							g_lightMode = 0;
float						g_lightAngle = 0;
float						g_speedFactor = 1;
float						g_inkLevel = -4.1f;

Camera* camera;
Entity* entities[5] = { nullptr };
vector<int> normalEntityIndices;
vector<int> bumpEntityIndices;
vector<int> transparentEntityIndices;
Light* lights[1] = { nullptr };

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();
void ResetScene();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE const hInstance, _In_opt_ HINSTANCE const hPrevInstance, _In_ LPWSTR const lpCmdLine, _In_ int const nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(InitDevice())) {
		CleanupDevice();
		return 0;
	}

	// Main message loop
	MSG msg = { nullptr };
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Render();
		}
	}

	CleanupDevice();

	return static_cast<int>(msg.wParam);
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(const HINSTANCE hInstance, const int nCmdShow) {
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return static_cast<HRESULT>(E_FAIL);

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 5",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice() {
	HRESULT hr = S_OK;

	hr = ConfigManager::LoadConfig(L"Configs/general.config");
	if (FAILED(hr))
		return hr;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	const UINT width = rc.right - rc.left;
	const UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);

		if (hr == E_INVALIDARG) {
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		}

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr)) {
			IDXGIAdapter* adapter = nullptr;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr)) {
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
				adapter->Release();
			}
			dxgiDevice->Release();
		}
	}
	if (FAILED(hr))
		return hr;

	// Create swap chain
	IDXGIFactory2* dxgiFactory2 = nullptr;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));
	if (dxgiFactory2) {
		// DirectX 11.1 or later
		hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1));
		if (SUCCEEDED(hr)) {
			g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1));
		}

		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = width;
		sd.Height = height;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice, g_hWnd, &sd, nullptr, nullptr, &g_pSwapChain1);
		if (SUCCEEDED(hr)) {
			hr = g_pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&g_pSwapChain));
		}

		dxgiFactory2->Release();
	}
	else {
		// DirectX 11.0 systems
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = g_hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		hr = dxgiFactory->CreateSwapChain(g_pd3dDevice, &sd, &g_pSwapChain);
	}

	// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
	dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);
	dxgiFactory->Release();
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(width);
	vp.Height = static_cast<FLOAT>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	// Rasterizer
	D3D11_RASTERIZER_DESC rastDesc;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FrontCounterClockwise = true;
	rastDesc.DepthBias = 0;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 0.0f;
	rastDesc.DepthClipEnable = false;
	rastDesc.ScissorEnable = false;
	rastDesc.MultisampleEnable = false;
	rastDesc.AntialiasedLineEnable = false;

	hr = g_pd3dDevice->CreateRasterizerState(&rastDesc, &g_pInverseRasterizerState);

	rastDesc.CullMode = D3D11_CULL_FRONT;

	hr = g_pd3dDevice->CreateRasterizerState(&rastDesc, &g_pRasterizerState);

	rastDesc.CullMode = D3D11_CULL_NONE;

	hr = g_pd3dDevice->CreateRasterizerState(&rastDesc, &g_pNoCullRasterizerState);

	// Blend Desc
	D3D11_BLEND_DESC dsBlend;
	dsBlend.AlphaToCoverageEnable = false;
	dsBlend.IndependentBlendEnable = false;
	dsBlend.RenderTarget[0].BlendEnable = true;
	dsBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	dsBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	dsBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	dsBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	dsBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	dsBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	dsBlend.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	hr = g_pd3dDevice->CreateBlendState(&dsBlend, &g_pBlendDesc);

	dsBlend.RenderTarget[0].BlendEnable = false;
	hr = g_pd3dDevice->CreateBlendState(&dsBlend, &g_pNoBlendDesc);


	//-------------------------------------------------------------------------------------------------------------------------

	// Compile the vertex shader
	ShaderManager::InitialiseShaders(g_pd3dDevice, g_pImmediateContext);

	//-------------------------------------------------------------------------------------------------------------------------

	//ASSIMP HERE
	Assimp::Importer importer;
	
	// Sphere
	const aiScene* const sphereScene = importer.ReadFile("Models/Sphere.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	aiMesh* mesh = sphereScene->mMeshes[0];

	// Sphere Vertices
	std::vector<VertexData>sphere_vertices;
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		VertexData vertex;
		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
		vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;		
		
		vertex.BiNormal.x = mesh->mBitangents[i].x;
		vertex.BiNormal.y = mesh->mBitangents[i].y;
		vertex.BiNormal.z = mesh->mBitangents[i].z;

		vertex.Tangent.x = mesh->mTangents[i].x;
		vertex.Tangent.y = mesh->mTangents[i].y;
		vertex.Tangent.z = mesh->mTangents[i].z;

		sphere_vertices.push_back(vertex);
	}

	// Sphere Indices
	std::vector<WORD>sphere_indices;
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		const aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			sphere_indices.push_back(static_cast<WORD>(face.mIndices[j]));
	}

	// Cube
	const aiScene* const cubeScene = importer.ReadFile("Models/Cube.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	mesh = cubeScene->mMeshes[0];

	// Cube Vertices
	std::vector<VertexData>cubeMesh_vertices;
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		VertexData vertex;
		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
		vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		vertex.BiNormal.x = mesh->mBitangents[i].x;
		vertex.BiNormal.y = mesh->mBitangents[i].y;
		vertex.BiNormal.z = mesh->mBitangents[i].z;

		vertex.Tangent.x = mesh->mTangents[i].x;
		vertex.Tangent.y = mesh->mTangents[i].y;
		vertex.Tangent.z = mesh->mTangents[i].z;

		cubeMesh_vertices.push_back(vertex);
	}

	// Cube Indices
	std::vector<WORD>cubeMesh_indices;
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		const aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			cubeMesh_indices.push_back(static_cast<WORD>(face.mIndices[j]));
	}

	// Plane
	const aiScene* const planeScene = importer.ReadFile("Models/Plane.obj", aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	mesh = planeScene->mMeshes[0];

	// Plane Vertices
	std::vector<VertexData>planeMesh_vertices;
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		VertexData vertex;
		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
		vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		vertex.BiNormal.x = mesh->mBitangents[i].x;
		vertex.BiNormal.y = mesh->mBitangents[i].y;
		vertex.BiNormal.z = mesh->mBitangents[i].z;

		vertex.Tangent.x = mesh->mTangents[i].x;
		vertex.Tangent.y = mesh->mTangents[i].y;
		vertex.Tangent.z = mesh->mTangents[i].z;

		planeMesh_vertices.push_back(vertex);
	}

	// plane Indices
	std::vector<WORD>planeMesh_indices;
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		const aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
			planeMesh_indices.push_back(static_cast<WORD>(face.mIndices[j]));
	}

	//END ASSIMP

	// Create models
	Model cubeModel(cubeMesh_vertices, cubeMesh_indices);
	cubeModel.BindVertexBuffer(g_pd3dDevice, g_pImmediateContext, sizeof(VertexData), 0);
	cubeModel.BindIndexBuffer(g_pd3dDevice, g_pImmediateContext, 0);

	Model sphereModel(sphere_vertices, sphere_indices);
	sphereModel.BindVertexBuffer(g_pd3dDevice, g_pImmediateContext, sizeof(VertexData), 0);
	sphereModel.BindIndexBuffer(g_pd3dDevice, g_pImmediateContext, 0);

	Model planeModel(planeMesh_vertices, planeMesh_indices);
	planeModel.BindVertexBuffer(g_pd3dDevice, g_pImmediateContext, sizeof(VertexData), 0);
	planeModel.BindIndexBuffer(g_pd3dDevice, g_pImmediateContext, 0);

	// --Create entity from model
	// Room
	entities[0] = new Entity(cubeModel);
	entities[0]->SetShaderIndex(7);
	
	// Ink
	entities[1] = new Entity(planeModel);
	entities[1]->SetShaderIndex(6);

	// Cube
	entities[2] = new Entity(cubeModel);

	// Sphere1
	entities[3] = new Entity(sphereModel);

	// Sphere2
	entities[4] = new Entity(sphereModel);

	// Initialize the camera
	camera = new Camera();

	// Lights
	lights[0] = new Light();

	ResetScene();

	// Textures
	hr = CreateDDSTextureFromFile(g_pd3dDevice, L"textures/Wood.dds", nullptr, &g_pWoodTextureRV);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(g_pd3dDevice, L"textures/stones.dds", nullptr, &g_pStoneTextureRV);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(g_pd3dDevice, L"textures/stones_NM_height.dds", nullptr, &g_pStoneNormalTextureRV);
	if (FAILED(hr))
		return hr;

	hr = CreateDDSTextureFromFile(g_pd3dDevice, L"textures/Skymap.dds", nullptr, &g_pSkymapTextureRV);
	if (FAILED(hr))
		return hr;

	// Samplers
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = g_pd3dDevice->CreateSamplerState(&samplerDesc, &g_pWoodSampler);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateSamplerState(&samplerDesc, &g_pStoneSampler);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateSamplerState(&samplerDesc, &g_pSkymapSampler);
	if (FAILED(hr))
		return hr;

	//---------------------------------------------------------------------------

	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	//-------------------------------------------------------------------------------------------------------------------------

	

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / static_cast<FLOAT>(height), 0.01f, 100.0f);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice() {
	if (g_pImmediateContext) g_pImmediateContext->ClearState();

	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();

	ShaderManager::Cleanup();

	if (g_pWoodTextureRV) g_pWoodTextureRV->Release();
	if (g_pWoodSampler) g_pWoodSampler->Release();

	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain1) g_pSwapChain1->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext1) g_pImmediateContext1->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice1) g_pd3dDevice1->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

			// Note that this tutorial does not handle resizing (WM_SIZE) requests,
			// so we created the window without the resize border.

		case WM_KEYDOWN:
			Keyboard::KeyCallback(wParam, PRESS);
			break;

		case WM_KEYUP:
			Keyboard::KeyCallback(wParam, RELEASE);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void HandleInput(const float deltaTime) {
	const float moveAmount = 3.0f * deltaTime;
	// CTRL + W
	if (Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(87) || Keyboard::KeyPressed(VK_UP))) {
		camera->Move(moveAmount, 0.0f, 0.0f);
	}
	// CTRL + A
	if (Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(65) || Keyboard::KeyPressed(VK_LEFT))) {
		camera->Move(0.0f, -moveAmount, 0.0f);
	}
	// CTRL + S
	if (Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(83) || Keyboard::KeyPressed(VK_DOWN))) {
		camera->Move(-moveAmount, 0.0f, 0.0f);
	}
	// CTRL + D
	if (Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(68) || Keyboard::KeyPressed(VK_RIGHT))) {
		camera->Move(0.0f, moveAmount, 0.0f);
	}
	// CTRL + PageUp
	if (Keyboard::KeyPressed(VK_CONTROL) && Keyboard::KeyPressed(VK_PRIOR)) {
		camera->Move(0.0f, 0.0f, moveAmount);
	}
	// CTRL + PageDown
	if (Keyboard::KeyPressed(VK_CONTROL) && Keyboard::KeyPressed(VK_NEXT)) {
		camera->Move(0.0f, 0.0f, -moveAmount);
	}

	const float angle = 90.0f * deltaTime;
	const float angleAsRad = XMConvertToRadians(angle);
	// W
	if (!Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(87) || Keyboard::KeyPressed(VK_UP))) {
		camera->RotateByForwardAndTurn(angleAsRad, 0.0f);
	}
	// A
	if (!Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(65) || Keyboard::KeyPressed(VK_LEFT))) {
		camera->RotateByForwardAndTurn(0.0f, -angleAsRad);
	}
	// S
	if (!Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(83) || Keyboard::KeyPressed(VK_DOWN))) {
		camera->RotateByForwardAndTurn(-angleAsRad, 0.0f);
	}
	// D
	if (!Keyboard::KeyPressed(VK_CONTROL) && (Keyboard::KeyPressed(68) || Keyboard::KeyPressed(VK_RIGHT))) {
		camera->RotateByForwardAndTurn(0.0f, angleAsRad);
	}

	// t
	if (!Keyboard::KeyPressed(VK_SHIFT) && Keyboard::KeyDown(0x54)) {
		g_speedFactor -= 0.2;
	}

	// T
	if (Keyboard::KeyPressed(VK_SHIFT) && Keyboard::KeyDown(0x54)) {
		g_speedFactor += 0.2;
	}

	// f
	const float yLimit = 4.1f;
	const float levelChange = 1.0f * deltaTime;
	if (!Keyboard::KeyPressed(VK_SHIFT) && Keyboard::KeyPressed(0x46)) {
		g_inkLevel -= levelChange;
		if (g_inkLevel < -yLimit) {
			g_inkLevel = -yLimit;
		}
		entities[1]->SetPosition(0.0f, g_inkLevel, 0.0f);
	}

	// F
	if (Keyboard::KeyPressed(VK_SHIFT) && Keyboard::KeyPressed(0x46)) {
		g_inkLevel += levelChange;
		if (g_inkLevel > yLimit) {
			g_inkLevel = yLimit;
		}
		entities[1]->SetPosition(0.0f, g_inkLevel, 0.0f);
	}

	// r
	if (Keyboard::KeyPressed(0x52)) {
		// --Reset the scene
		ResetScene();
	}

	// Move to camera position 1
	if (Keyboard::KeyDown(VK_F1)) {
		const XMFLOAT4 pos = XMFLOAT4(6.0f, 0.0f, 0.0f, 0.0f);
		const XMFLOAT4 at = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		const XMFLOAT4 up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		camera->Set(pos, at, up);
	}

	// Move to camera position 2
	if (Keyboard::KeyDown(VK_F2)) {
		const XMFLOAT4 pos = XMFLOAT4(0.0f, 6.0f, 0.0f, 0.0f);
		const XMFLOAT4 at = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		const XMFLOAT4 up = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
		camera->Set(pos, at, up);
	}

	// Randomize shaders
	if (Keyboard::KeyDown(VK_F5)) {
		normalEntityIndices.clear();
		bumpEntityIndices.clear();
		transparentEntityIndices.clear();

		for (int i = 2; i < 5; i++) {
			const int shaderIndex = ShaderManager::GetRandomShaderIndex();
			entities[i]->SetShaderIndex(shaderIndex);

			if (shaderIndex < 3) {
				normalEntityIndices.push_back(i);
			}
			else if (shaderIndex == 3) {
				bumpEntityIndices.push_back(i);
			}
			else if (shaderIndex == 6) {
				transparentEntityIndices.push_back(i);
			}
		}
	}

	// Change lighting modes
	if (Keyboard::KeyDown(VK_F6)) {
		g_lightMode++;
		if (g_lightMode > 1) {
			g_lightMode = 0;
			lights[0]->SetPosition(0.0f, 5.0f, 0.0f);
		}
	}

	// Change shadow type
	if (Keyboard::KeyDown(VK_F7)) {

	}

	// Change level render mode
	if (Keyboard::KeyDown(VK_F8)) {

	}
}

void ResetScene() {
	// Reset values from configManager
	ConfigManager::InitialiseWorldCube(*entities[0]);
	ConfigManager::InitialiseInk(*entities[1]);
	ConfigManager::InitialiseCube(*entities[2]);
	ConfigManager::InitialiseSphere1(*entities[3]);
	ConfigManager::InitialiseSphere2(*entities[4]);
	ConfigManager::InitialiseCamera(*camera);
	ConfigManager::InitialiseLight(*lights[0]);

	// Reset light mode
	g_lightMode = 0;

	// Reset speed factor
	g_speedFactor = 1;

	// Reset render vectors
	normalEntityIndices.clear();
	normalEntityIndices.push_back(4);

	bumpEntityIndices.clear();
	bumpEntityIndices.push_back(2);

	transparentEntityIndices.clear();
	transparentEntityIndices.push_back(3);

	// Reset ink level
	g_inkLevel = -4.1f;
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render() {
	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE) {
		t += static_cast<float>(XM_PI) * 0.0125f;
	}
	else {
		static ULONGLONG timeStart = 0;
		const ULONGLONG timeCur = GetTickCount64();
		t = (timeCur - timeStart) / 1000.0f;
		timeStart = timeCur;
	}

	// Handle the input
	HandleInput(static_cast<float>(t));

	// Clear the back buffer
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);

	// Clear the depth buffer to 1.0 (max depth)
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Calculate light position
	if (g_lightMode == 1) {
		// Light circles around origin
		g_lightAngle += (90.0f * t) * g_speedFactor;
		const float rotateAngle = XMConvertToRadians(g_lightAngle);

		const XMMATRIX orbitMatrix = XMMatrixRotationY(rotateAngle);

		XMVECTOR lightPos = XMVectorSet(3.0f, 5.0f, 0.0f, 1.0f);
		lightPos = XMVector3Transform(lightPos, orbitMatrix);

		const XMFLOAT4 posAsFloat = XMFLOAT4(lightPos.m128_f32);

		lights[0]->SetPosition(posAsFloat.x, posAsFloat.y, posAsFloat.z);
	}


	// Update the constant buffer
	ConstantBuffer cb;
	cb.mView = XMMatrixTranspose(camera->GetMatrix());
	cb.mProjection = XMMatrixTranspose(g_Projection);
	const XMFLOAT4 eyePos = camera->GetPosition();
	cb.vEyePos = XMVectorSet(eyePos.x, eyePos.y, eyePos.z, eyePos.w);
	cb.vLightPos = lights[0]->GetPositionAsVector();
	cb.vLightCol = lights[0]->GetColourAsVector();

	// --Render the entities

	// World Cube
	g_pImmediateContext->RSSetState(g_pInverseRasterizerState);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pSkymapTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSkymapSampler);

	const XMMATRIX worldMat = entities[0]->GetTransformMatrix();
	cb.mWorld = XMMatrixTranspose(worldMat);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	entities[0]->Draw(g_pImmediateContext, g_pConstantBuffer);

	// Normal renders
	g_pImmediateContext->RSSetState(g_pRasterizerState);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pWoodTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pWoodSampler);
	for (int i = 0; i < normalEntityIndices.size(); i++) {
		const int entityIndex = normalEntityIndices[i];

		const XMMATRIX mat = entities[entityIndex]->GetTransformMatrix();
		cb.mWorld = XMMatrixTranspose(mat);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		entities[entityIndex]->Draw(g_pImmediateContext, g_pConstantBuffer);
	}
	
	// Bump Map Renders
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pStoneTextureRV);
	g_pImmediateContext->PSSetShaderResources(1, 1, &g_pStoneNormalTextureRV);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pStoneSampler);
	for (int i = 0; i < bumpEntityIndices.size(); i++) {
		const int entityIndex = bumpEntityIndices[i];

		const XMMATRIX mat = entities[entityIndex]->GetTransformMatrix();
		cb.mWorld = XMMatrixTranspose(mat);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		entities[entityIndex]->Draw(g_pImmediateContext, g_pConstantBuffer);
	}

	// Transparent Renders
	float temp[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_pImmediateContext->OMSetBlendState(g_pBlendDesc, temp, 0xffffffff);
	for (int i = 0; i < transparentEntityIndices.size(); i++) {
		const int entityIndex = transparentEntityIndices[i];

		const XMMATRIX mat = entities[entityIndex]->GetTransformMatrix();
		cb.mWorld = XMMatrixTranspose(mat);
		g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		entities[entityIndex]->Draw(g_pImmediateContext, g_pConstantBuffer);
	}
	g_pImmediateContext->OMSetBlendState(g_pNoBlendDesc, temp, 0xffffffff);


	// --No Cull Rast
	g_pImmediateContext->RSSetState(g_pNoCullRasterizerState);

	// Plane
	g_pImmediateContext->OMSetBlendState(g_pBlendDesc, temp, 0xffffffff);

	const XMMATRIX inkMat = entities[1]->GetTransformMatrix();
	cb.mWorld = XMMatrixTranspose(inkMat);
	cb.vLightCol = XMVectorSet(0.6f, 0.6f, 1.0f, 1.0f);
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	entities[1]->Draw(g_pImmediateContext, g_pConstantBuffer);

	g_pImmediateContext->OMSetBlendState(g_pNoBlendDesc, temp, 0xffffffff);

	// Present our back buffer to our front buffer
	g_pSwapChain->Present(0, 0);
}


