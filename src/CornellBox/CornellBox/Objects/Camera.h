#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera {
private:
	XMFLOAT4 _position;
	XMFLOAT4 _direction;
	XMFLOAT4 _up;

public:

	Camera();
	Camera(const XMFLOAT4& eye, const XMFLOAT4& at, const XMFLOAT4& up);
	Camera(const Camera& other);
	~Camera();

	void Set(const XMFLOAT4& position, const XMFLOAT4& lookingAt, const XMFLOAT4& up);

	XMFLOAT4 GetPosition() const { return _position; }
	XMFLOAT4 GetDirection() const { return _direction; }
	XMVECTOR GetDirectionAsVector() const;
	XMVECTOR GetUpAsVector() const;
	XMMATRIX GetMatrix() const;

	void RotateByForwardAndTurn(float forwardAngle, float turnAngle);
	void Move(const float forward, const float side, const float height);
};

