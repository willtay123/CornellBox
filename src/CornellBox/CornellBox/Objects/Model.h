#pragma once
#include <vector>
#include <xpolymorphic_allocator.h>
#include <d3d11_1.h>
#include "VertexData.h"

using namespace DirectX;

using std::vector;

class Model {
private:
	unsigned int _numOfVertices;
	unsigned int _numOfIndices;
	VertexData* _vertices;
	unsigned short* _indices;

	ID3D11Buffer* _vertexBufferID;
	ID3D11Buffer* _indexBufferID;
	UINT _vertexStride;

public:

	Model();
	Model(const vector<VertexData>& vertices, const vector<unsigned short>& indices);
	Model(const Model& other);
	~Model();

	Model& operator=(const Model& rhs);

	unsigned int NumOfVertices() const { return _numOfVertices; }
	const VertexData* const GetVertices() const { return _vertices; }

	unsigned int NumOfIndices() const { return _numOfIndices; }
	const unsigned short* const GetIndices() const { return _indices; }

	HRESULT BindVertexBuffer(ID3D11Device* const device, ID3D11DeviceContext* const context, const UINT stride, const UINT offset);
	HRESULT BindIndexBuffer(ID3D11Device* const device, ID3D11DeviceContext* const context, const UINT offset);

	ID3D11Buffer* const * const GetVertexBufferIDAddress() const { return &_vertexBufferID; }
	ID3D11Buffer* GetIndexBufferIDAddress() const { return _indexBufferID; }

	UINT GetVertexStride() const { return _vertexStride; }

};

