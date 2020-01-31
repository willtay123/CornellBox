#include "Model.h"


Model::Model() :
	_numOfVertices(0),
	_numOfIndices(0),
	_vertices(nullptr),
	_indices(nullptr),
	_vertexBufferID(nullptr),
	_indexBufferID(nullptr),
	_vertexStride(0) {

}

Model::Model(const vector<VertexData>& vertices, const vector<unsigned short>& indices) :
	_numOfVertices(vertices.size()),
	_numOfIndices(indices.size()),
	_vertexBufferID(nullptr),
	_indexBufferID(nullptr),
	_vertexStride(0) {

	_vertices = new VertexData[_numOfVertices];
	for (unsigned int i = 0; i < _numOfVertices; i++) {
		_vertices[i] = vertices[i];
	}
	
	_indices = new unsigned short[_numOfIndices];
	for (unsigned int i = 0; i < _numOfIndices; i++) {
		_indices[i] = indices[i];
	}
}

Model::Model(const Model& other) :
	_numOfVertices(other._numOfVertices),
	_numOfIndices(other._numOfIndices),
	_vertexBufferID(other._vertexBufferID),
	_indexBufferID(other._indexBufferID),
	_vertexStride(other._vertexStride) {

	_vertices = new VertexData[_numOfVertices];
	for (unsigned int i = 0; i < _numOfVertices; i++) {
		_vertices[i] = other._vertices[i];
	}

	_indices = new unsigned short[_numOfIndices];
	for (unsigned int i = 0; i < _numOfIndices; i++) {
		_indices[i] = other._indices[i];
	}
}

Model::~Model() {
	delete[] _vertices;
	delete[] _indices;
}

Model& Model::operator=(const Model& rhs) {
	if (this != &rhs) {
		// Clean current data
		delete[] _vertices;
		delete[] _indices;

		// Copy array data
		_numOfVertices = rhs._numOfVertices;
		_vertices = new VertexData[_numOfVertices];
		std::copy(rhs._vertices, rhs._vertices + rhs._numOfVertices, _vertices);
		_vertexBufferID = rhs._vertexBufferID;
		_vertexStride = rhs._vertexStride;

		_numOfIndices = rhs._numOfIndices;
		_indices = new unsigned short[_numOfIndices];
		std::copy(rhs._indices, rhs._indices + rhs._numOfIndices, _indices);
		_indexBufferID = rhs._indexBufferID;
	}
	return *this;
}

HRESULT Model::BindVertexBuffer(ID3D11Device* const device, ID3D11DeviceContext* const context, const UINT stride, const UINT offset) {
	_vertexStride = stride;

	// Create buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexData) * _numOfVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// Create subresource data
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = _vertices;
	const HRESULT hr = device->CreateBuffer(&bd, &InitData, &_vertexBufferID);
	if (FAILED(hr))
		return hr;

	context->IASetVertexBuffers(0, 1, &_vertexBufferID, &_vertexStride, &offset);

	return S_OK;
}

HRESULT Model::BindIndexBuffer(ID3D11Device* const device, ID3D11DeviceContext* const context, const UINT offset) {
	// Create buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned short) * _numOfIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// Create subresource data
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	_indexBufferID = nullptr;
	InitData.pSysMem = _indices;
	const HRESULT hr = device->CreateBuffer(&bd, &InitData, &_indexBufferID);
	if (FAILED(hr))
		return hr;

	context->IASetIndexBuffer(_indexBufferID, DXGI_FORMAT_R16_UINT, offset);

	return S_OK;
}