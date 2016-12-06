// Header Files
//=============

#include<iostream>

#include "../Mesh.h"
#include "../CommonData.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../MeshData.h"

#include <D3D11.h>

bool eae6320::Graphics::Mesh::Initialize(const MeshData& i_meshData)
{
	CommonData *commonData = CommonData::GetCommonData();
	m_numberOfIndices = i_meshData.numberOfIndices;

	//Vertex Buffer Init
	const unsigned int vertexBufferSize = i_meshData.numberOfVertices * sizeof(MeshData::Vertex);

	D3D11_BUFFER_DESC bufferDescriptionVertexBuffer = { 0 };
	{
		bufferDescriptionVertexBuffer.ByteWidth = vertexBufferSize;
		bufferDescriptionVertexBuffer.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
		bufferDescriptionVertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescriptionVertexBuffer.CPUAccessFlags = 0;	// No CPU access is necessary
		bufferDescriptionVertexBuffer.MiscFlags = 0;
		bufferDescriptionVertexBuffer.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialDataVertexBuffer = { 0 };
	{
		if (i_meshData.vertexData)
		{
			initialDataVertexBuffer.pSysMem = i_meshData.vertexData;
		}
		else
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer because there is no Vertex Data");
			return false;
		}

		// (The other data members are ignored for non-texture buffers)
	}

	const HRESULT resultVertexBuffer = commonData->s_direct3dDevice->CreateBuffer(&bufferDescriptionVertexBuffer, &initialDataVertexBuffer, &m_vertexBuffer);
	if (FAILED(resultVertexBuffer))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer with HRESULT %#010x", resultVertexBuffer);
		return false;
	}

	//Index Buffer Init
	unsigned int indexBufferSize = 0;
	if (i_meshData.typeOfIndexData == 16)
	{
		indexBufferSize = i_meshData.numberOfIndices * sizeof(uint16_t);
		m_is16bit = true;
	}
	else
	{
		indexBufferSize = i_meshData.numberOfIndices * sizeof(uint32_t);
		m_is16bit = false;
	}
	D3D11_BUFFER_DESC bufferDescriptionIndexBuffer = { 0 };
	{
		bufferDescriptionIndexBuffer.ByteWidth = indexBufferSize;
		bufferDescriptionIndexBuffer.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
		bufferDescriptionIndexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDescriptionIndexBuffer.CPUAccessFlags = 0;	// No CPU access is necessary
		bufferDescriptionIndexBuffer.MiscFlags = 0;
		bufferDescriptionIndexBuffer.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialDataIndexBuffer = { 0 };
	{
		if (i_meshData.indexData)
		{
			initialDataIndexBuffer.pSysMem = i_meshData.indexData;
		}
		else
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create the index buffer because there is no Index Data");
			return false;
		}
	}
	// (The other data members are ignored for non-texture buffers)


	const HRESULT resultIndexBuffer = commonData->s_direct3dDevice->CreateBuffer(&bufferDescriptionIndexBuffer, &initialDataIndexBuffer, &m_indexBuffer);
	if (FAILED(resultIndexBuffer))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the index buffer with HRESULT %#010x", resultIndexBuffer);
		return false;
	}

	return true;

}

bool eae6320::Graphics::Mesh::CleanUp()
{
	bool wereThereErrors = false;
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}
	return !wereThereErrors;
}

void eae6320::Graphics::Mesh::RenderMesh()const
{
	CommonData *commonData = CommonData::GetCommonData();
	// Bind a specific vertex buffer to the device as a data source
	{
		const unsigned int startingSlot = 0;
		const unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		const unsigned int bufferStride = sizeof(MeshData::Vertex);

		// It's possible to start streaming data in the middle of a vertex buffer
		const unsigned int bufferOffset = 0;
		commonData->s_direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset);
	}
	// Specify what kind of data the vertex buffer holds
	{
		// Set the layout (which defines how to interpret a single vertex)
		//commonData->s_direct3dImmediateContext->IASetInputLayout(commonData->s_vertexLayout);
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// we have defined the vertex buffer as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		//commonData->s_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	{
		EAE6320_ASSERT(m_indexBuffer != NULL);
		// Every index is a 16 bit unsigned integer
		DXGI_FORMAT format;
		if (m_is16bit)
		{
			format = DXGI_FORMAT_R16_UINT;
		}
		else
		{
			format = DXGI_FORMAT_R32_UINT;
		}

		// The indices start at the beginning of the buffer
		const unsigned int offset = 0;
		commonData->s_direct3dImmediateContext->IASetIndexBuffer(m_indexBuffer, format, offset);
	}
	// Render triangles from the currently-bound vertex buffer
	{

		// It's possible to start rendering primitives in the middle of the stream
		const unsigned int indexOfFirstIndexToUse = 0;
		const unsigned int offsetToAddToEachIndex = 0;
		commonData->s_direct3dImmediateContext->DrawIndexed(m_numberOfIndices, indexOfFirstIndexToUse, offsetToAddToEachIndex);
	}
}
