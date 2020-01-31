#include "Camera.h"


Camera::Camera() :
	_position(XMFLOAT4(0,0,0,0)),
	_direction(XMFLOAT4(1,0,0,0)),
	_up(XMFLOAT4(0,1,0,0)) {

}

Camera::Camera(const XMFLOAT4& eye, const XMFLOAT4& at, const XMFLOAT4& up) :
	_position(eye),
	_direction(XMFLOAT4(1,0,0,0)),
	_up(up) {
	Set(eye, at, up);
}

Camera::Camera(const Camera& other) :
	_position(other._position),
	_direction(other._direction),
	_up(other._up) {

}

Camera::~Camera() {

}

void Camera::Set(const XMFLOAT4& position, const XMFLOAT4& lookingAt, const XMFLOAT4& up) {
	_position = position;
	
	XMVECTOR const direction = XMVectorSubtract(XMLoadFloat4(&lookingAt), XMLoadFloat4(&position));
	_direction = XMFLOAT4(XMVector4Normalize(direction).m128_f32);
	
	_up = up;
}

XMMATRIX Camera::GetMatrix() const {
	XMVECTOR eye = XMVectorSet(_position.x, _position.y, _position.z, 0);
	XMVECTOR at = XMVectorSet(
		_position.x + _direction.x,
		_position.y + _direction.y,
		_position.z + _direction.z, 
		0
	);
	XMVECTOR up = XMVectorSet(_up.x, _up.y, _up.z, 0);
	XMMATRIX const matrix = XMMatrixLookAtLH(eye, at, up);
	return matrix;
}

XMVECTOR Camera::GetDirectionAsVector() const {
	return XMLoadFloat4(&_direction);
}

XMVECTOR Camera::GetUpAsVector() const {
	return XMLoadFloat4(&_up);
}

void Camera::RotateByForwardAndTurn(float forwardAngle, float turnAngle) {
	const XMVECTOR direction = GetDirectionAsVector();
	const XMVECTOR up = GetUpAsVector();
	XMVECTOR rightDirection = XMVector3Cross(direction, up);
	XMVECTOR upDirection = GetUpAsVector();

	XMMATRIX forwardRotate = XMMatrixRotationAxis(rightDirection, forwardAngle);
	XMMATRIX turnRotate = XMMatrixRotationAxis(upDirection, turnAngle);

	const XMMATRIX rotation = forwardRotate * turnRotate;
	const XMVECTOR newAt = XMVector4Transform(direction, rotation);
	const XMVECTOR newUp = XMVector4Transform(up, rotation);

	_direction = XMFLOAT4(newAt.m128_f32);
	_up = XMFLOAT4(newUp.m128_f32);
}

void Camera::Move(const float forward, const float side, const float height) {
	const XMVECTOR direction = GetDirectionAsVector();
	XMVECTOR position = XMLoadFloat4(&GetPosition());
	const XMMATRIX sideMatrix = XMMatrixRotationY(XMConvertToRadians(90));

	const XMVECTOR forwardAmount = direction * forward;
	const XMVECTOR sideAmount = XMVector4Transform(direction, sideMatrix) * side;

	position += forwardAmount + sideAmount + XMVectorSet(0.0f, height, 0.0f, 0.0f);
	_position = XMFLOAT4(position.m128_f32);
}