#pragma once
#include <d3d11_1.h>
#include <directxmath.h>

#include "ShaderManager.h"
#include "Model.h"

using namespace DirectX;

class Entity {
private:
	XMFLOAT4 _position;
	XMFLOAT4 _scale;
	XMFLOAT4 _rotation;
	Model* _model;
	int _shaderIndex;

public:
	Entity();
	explicit Entity(const Model& model);
	Entity(const Entity& other);
	~Entity();

	Entity& operator=(const Entity& rhs);

	void SetPosition(const float x, const float y, const float z);
	XMFLOAT4 GetPosition() const;
	XMMATRIX GetPositionAsMatrix() const;

	void SetScale(const float xScale, const float yScale, const float zScale);
	void SetScale(const float scale);
	XMFLOAT4 GetScale() const;
	XMMATRIX GetScaleAsMatrix() const;

	void SetRotation(const float xRot, const float yRot, const float zRot);
	XMFLOAT4 GetRotation() const;
	XMMATRIX GetRotationAsMatrix() const;

	XMMATRIX GetTransformMatrix() const;

	void SetModel(const Model& model);
	const Model& GetModel() const;

	void SetShaderIndex(const int index);

	void Draw(ID3D11DeviceContext* const context, ID3D11Buffer* const buffer) const;
};

