#include "Light.h"



Light::Light() :
	_position(0.0f, 0.0f, 0.0f, 0.0f),
	_lightColour(255.0f, 255.0f, 255.0f, 255.0f) {
}

Light::Light(const XMFLOAT4& position) :
	_position(position),
	_lightColour(255.0f, 255.0f, 255.0f, 255.0f) {
}

Light::Light(const float xPos, const float yPos, const float zPos) :
	_position(xPos, yPos, zPos, 0.0f),
	_lightColour(255.0f, 255.0f, 255.0f, 255.0f) {
}

Light::Light(const Light& other) :
	_position(other._position),
	_lightColour(other._lightColour) {

}

Light::~Light() {
}

Light& Light::operator=(const Light& rhs) {
	if (this != &rhs) {
		_position = rhs._position;
		_lightColour = rhs._lightColour;
	}
	return *this;
}

XMFLOAT4 Light::GetPosition() const {
	return _position;
}

XMVECTOR Light::GetPositionAsVector() const {
	return XMLoadFloat4(&_position);
}

XMFLOAT4 Light::GetColour() const {
	return _lightColour;
}

XMVECTOR Light::GetColourAsVector() const {
	return XMLoadFloat4(&_lightColour);
}
