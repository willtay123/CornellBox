#include "Shader.h"


Shader::Shader() :
	vertexShader(nullptr),
	pixelShader(nullptr) {

}

Shader::Shader(ID3D11VertexShader* const pVertexShader, ID3D11PixelShader* const pPixelShader) :
	vertexShader(pVertexShader),
	pixelShader(pPixelShader) {

}

Shader::Shader(const Shader& other) :
	vertexShader(other.vertexShader),
	pixelShader(other.pixelShader) {

}

Shader& Shader::operator=(const Shader& rhs) {
	if (this != &rhs) {
		vertexShader = rhs.vertexShader;
		pixelShader = rhs.pixelShader;
	}
	return *this;
}