#include "ConfigManager.h"

XMFLOAT4 ConfigManager::_worldCubePosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
float ConfigManager::_worldCubeScale = 1.0f;
XMFLOAT4 ConfigManager::_worldCubeRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

XMFLOAT4 ConfigManager::_cubePosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
float ConfigManager::_cubeScale = 1.0f;
XMFLOAT4 ConfigManager::_cubeRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
int ConfigManager::_cubeShaderIndex = 0;

XMFLOAT4 ConfigManager::_sphere1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
float ConfigManager::_sphere1Scale = 1.0f;
XMFLOAT4 ConfigManager::_sphere1Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
int ConfigManager::_sphere1ShaderIndex = 0;

XMFLOAT4 ConfigManager::_sphere2Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
float ConfigManager::_sphere2Scale = 1.0f;
XMFLOAT4 ConfigManager::_sphere2Rotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
int ConfigManager::_sphere2ShaderIndex = 0;

XMFLOAT4 ConfigManager::_inkPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
float ConfigManager::_inkScale = 1.0f;
XMFLOAT4 ConfigManager::_inkRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

XMFLOAT4 ConfigManager::_cameraPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
XMFLOAT4 ConfigManager::_cameraAt = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
XMFLOAT4 ConfigManager::_cameraUp = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

XMFLOAT4 ConfigManager::_lightPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
XMFLOAT4 ConfigManager::_lightColour = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);


HRESULT ConfigManager::LoadConfig(const WCHAR* const filename) {
	// NOTE: It's ugly, I know, its meant to be
	HRESULT hr = S_OK;

	ifstream is;
	is.open(filename);

	if (is.is_open()) {
		// read file
		string line;
		while (getline(is, line)) {
			// Skip line if empty
			if (line.length() == 0)
				continue;

			istringstream iss(line);
			char first;
			iss >> first;

			// Skip line if comment
			if (first == '#')
				continue;

			if (first == 'n') {
				// Entity block
				string objectName;
				iss >> objectName;
				char ch;
			
				if (objectName == "Room") {
					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _worldCubePosition.x >> _worldCubePosition.y >> _worldCubePosition.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _worldCubeScale;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _worldCubeRotation.x >> _worldCubeRotation.y >> _worldCubeRotation.z;
				}
				else if (objectName == "Ink") {
					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _inkPosition.x >> _inkPosition.y >> _inkPosition.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _inkScale;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _inkRotation.x >> _inkRotation.y >> _inkRotation.z;
				}
				else if (objectName == "Cube") {
					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _cubePosition.x >> _cubePosition.y >> _cubePosition.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _cubeScale;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _cubeRotation.x >> _cubeRotation.y >> _cubeRotation.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _cubeShaderIndex;
				}
				else if (objectName == "Sphere1") {
					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere1Position.x >> _sphere1Position.y >> _sphere1Position.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere1Scale;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere1Rotation.x >> _sphere1Rotation.y >> _sphere1Rotation.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere1ShaderIndex;
				}
				else if (objectName == "Sphere2") {
					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere2Position.x >> _sphere2Position.y >> _sphere2Position.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere2Scale;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere2Rotation.x >> _sphere2Rotation.y >> _sphere2Rotation.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _sphere2ShaderIndex;
				}
				else if (objectName == "Camera") {
					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _cameraPosition.x >> _cameraPosition.y >> _cameraPosition.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _cameraAt.x >> _cameraAt.y >> _cameraAt.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _cameraUp.x >> _cameraUp.y >> _cameraUp.z;
				}
				else if (objectName == "Light") {
					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _lightPosition.x >> _lightPosition.y >> _lightPosition.z;

					getline(is, line);
					iss = istringstream(line);
					iss >> ch >> _lightColour.x >> _lightColour.y >> _lightColour.z >> _lightColour.w;
				}
				else {
					hr = E_FAIL;
				}
			}
		}
	}
	else {
		hr = E_FAIL;
	}

	return hr;
}

void ConfigManager::InitialiseCamera(Camera& camera) {
	camera.Set(_cameraPosition, _cameraAt, _cameraUp);
}

void ConfigManager::InitialiseLight(Light& light) {
	light.SetPosition(_lightPosition.x, _lightPosition.y, _lightPosition.z);
	light.SetColour(_lightColour.x, _lightColour.y, _lightColour.z, _lightColour.w);
}

void ConfigManager::InitialiseWorldCube(Entity& worldCube) {
	worldCube.SetPosition(_worldCubePosition.x, _worldCubePosition.y, _worldCubePosition.z);
	worldCube.SetScale(_worldCubeScale);
	worldCube.SetRotation(_worldCubeRotation.x, _worldCubeRotation.y, _worldCubeRotation.z);
}

void ConfigManager::InitialiseCube(Entity& cube) {
	cube.SetPosition(_cubePosition.x, _cubePosition.y, _cubePosition.z);
	cube.SetScale(_cubeScale);
	cube.SetRotation(_cubeRotation.x, _cubeRotation.y, _cubeRotation.z);
	cube.SetShaderIndex(_cubeShaderIndex);
}

void ConfigManager::InitialiseSphere1(Entity& sphere) {
	sphere.SetPosition(_sphere1Position.x, _sphere1Position.y, _sphere1Position.z);
	sphere.SetScale(_sphere1Scale);
	sphere.SetRotation(_sphere1Rotation.x, _sphere1Rotation.y, _sphere1Rotation.z);
	sphere.SetShaderIndex(_sphere1ShaderIndex);
}

void ConfigManager::InitialiseSphere2(Entity& sphere) {
	sphere.SetPosition(_sphere2Position.x, _sphere2Position.y, _sphere2Position.z);
	sphere.SetScale(_sphere2Scale);
	sphere.SetRotation(_sphere2Rotation.x, _sphere2Rotation.y, _sphere2Rotation.z);
	sphere.SetShaderIndex(_sphere2ShaderIndex);
}

void ConfigManager::InitialiseInk(Entity& ink) {
	ink.SetPosition(_inkPosition.x, _inkPosition.y, _inkPosition.z);
	ink.SetScale(_inkScale);
	ink.SetRotation(_inkRotation.x, _inkRotation.y, _inkRotation.z);
}
