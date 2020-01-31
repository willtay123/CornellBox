#include "Entity.h"



Entity::Entity() :
	_position(0,0,0,0),
	_scale(0,0,0,0),
	_rotation(0,0,0,0),
	_model(nullptr),
	_shaderIndex(0) {
}

Entity::Entity(const Model& model) : 
	_position(0, 0, 0, 0),
	_scale(0, 0, 0, 0),
	_rotation(0, 0, 0, 0),
	_model(new Model(model)),
	_shaderIndex(0) {
}

Entity::Entity(const Entity& other) : 
	_position(other._position),
	_scale(other._scale),
	_rotation(other._rotation),
	_model(new Model(*other._model)),
	_shaderIndex(0) {

}

Entity::~Entity() {
	delete _model;
}

Entity& Entity::operator=(const Entity& rhs) {
	if (this != &rhs) {
		// Clear old data
		delete _model;

		// Copy over data
		_position = rhs._position;
		_scale = rhs._scale;
		_rotation = rhs._rotation;
		_model = new Model(*(rhs._model));
		_shaderIndex = rhs._shaderIndex;
	}
	return *this;
}

void Entity::SetPosition(const float x, const float y, const float z) {
	_position = XMFLOAT4(x, y, z, 0);
}

XMFLOAT4 Entity::GetPosition() const {
	return _position;
}

XMMATRIX Entity::GetPositionAsMatrix() const {
	const XMMATRIX matrix = XMMatrixTranslation(_position.x, _position.y, _position.z);
	return matrix;
}

void Entity::SetScale(const float xScale, const float yScale, const float zScale) {
	_scale = XMFLOAT4(xScale, yScale, zScale, 0);
}

void Entity::SetScale(const float scale) {
	_scale = XMFLOAT4(scale, scale, scale, 0);
}

XMFLOAT4 Entity::GetScale() const {
	return _scale;
}

XMMATRIX Entity::GetScaleAsMatrix() const {
	const XMMATRIX matrix = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	return matrix;
}

void Entity::SetRotation(const float xRot, const float yRot, const float zRot) {
	_rotation = XMFLOAT4(xRot, yRot, zRot, 0);
}

XMFLOAT4 Entity::GetRotation() const {
	return _rotation;
}

XMMATRIX Entity::GetRotationAsMatrix() const {
	const XMMATRIX matrix = XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
	return matrix;
}

XMMATRIX Entity::GetTransformMatrix() const {
	// Translate Rotate Scale
	const XMMATRIX matrix = GetScaleAsMatrix() * GetRotationAsMatrix() * GetPositionAsMatrix();
	return matrix;
}

void Entity::SetModel(const Model& model) {
	_model = new Model(model);
}

const Model& Entity::GetModel() const {
	return *_model;
}

void Entity::SetShaderIndex(const int index) {
	_shaderIndex = index;
}

void Entity::Draw(ID3D11DeviceContext* const context, ID3D11Buffer* const buffer) const {
	//unsigned int numOfVertices = _model->NumOfVertices();
	const unsigned int numOfIndices = _model->NumOfIndices();
	Shader shader = ShaderManager::GetShader(_shaderIndex);

	// Set Buffers
	const UINT stride = _model->GetVertexStride();
	const UINT offset = 0;
	context->IASetVertexBuffers(0, 1, _model->GetVertexBufferIDAddress(), &stride, &offset);
	context->IASetIndexBuffer(_model->GetIndexBufferIDAddress(), DXGI_FORMAT_R16_UINT, offset);

	context->VSSetShader(shader.vertexShader, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &buffer);
	context->PSSetConstantBuffers(0, 1, &buffer);
	context->PSSetShader(shader.pixelShader, nullptr, 0);
	context->DrawIndexed(numOfIndices, 0, 0);
}