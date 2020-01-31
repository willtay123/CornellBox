#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Light {
private:
	XMFLOAT4 _position;
	XMFLOAT4 _lightColour;

public:
	Light();
	explicit Light(const XMFLOAT4& position);
	Light(const float xPos, const float yPos, const float zPos);
	Light(const Light& other);
	~Light();
	Light& operator=(const Light& rhs);

	XMFLOAT4 GetPosition() const;
	XMVECTOR GetPositionAsVector() const;
	void SetPosition(const float x, const float y, const float z) {
		_position = XMFLOAT4(x, y, z, 0.0f);
	}

	XMFLOAT4 GetColour() const;
	XMVECTOR GetColourAsVector() const;
	void SetColour(const float red, const float green, const float blue, const float alpha) {
		_lightColour = XMFLOAT4(red, green, blue, alpha);
	}
};

