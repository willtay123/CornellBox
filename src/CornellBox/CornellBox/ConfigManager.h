#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Entity.h"
#include "Camera.h"
#include "Light.h"

using std::ifstream;
using std::string;
using std::istringstream;

class ConfigManager {
private:
	static XMFLOAT4 _worldCubePosition;
	static float _worldCubeScale;
	static XMFLOAT4 _worldCubeRotation;

	static XMFLOAT4 _cubePosition;
	static float _cubeScale;
	static XMFLOAT4 _cubeRotation;
	static int _cubeShaderIndex;

	static XMFLOAT4 _sphere1Position;
	static float _sphere1Scale;
	static XMFLOAT4 _sphere1Rotation;
	static int _sphere1ShaderIndex;

	static XMFLOAT4 _sphere2Position;
	static float _sphere2Scale;
	static XMFLOAT4 _sphere2Rotation;
	static int _sphere2ShaderIndex;

	static XMFLOAT4 _inkPosition;
	static float _inkScale;
	static XMFLOAT4 _inkRotation;

	static XMFLOAT4 _cameraPosition;
	static XMFLOAT4 _cameraAt;
	static XMFLOAT4 _cameraUp;

	static XMFLOAT4 _lightPosition;
	static XMFLOAT4 _lightColour;
	
	ConfigManager();

public:
	
	static HRESULT LoadConfig(const WCHAR* const filename);

	static void InitialiseCamera(Camera& camera);
	static void InitialiseLight(Light& light);

	static void InitialiseWorldCube(Entity& worldCube);
	static void InitialiseCube(Entity& cube);
	static void InitialiseSphere1(Entity& sphere);
	static void InitialiseSphere2(Entity& sphere);
	static void InitialiseInk(Entity& ink);
};

