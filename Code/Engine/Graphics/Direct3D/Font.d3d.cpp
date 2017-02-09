#include "../Font.h"
#include "../../Windows/Includes.h"
#include <D3D11.h>
#include "../MeshData.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../CommonData.h"

// Static Data Initialization
//===========================

ID3D11Buffer* eae6320::Graphics::Font::ms_vertexBuffer = nullptr;
ID3D11Buffer* eae6320::Graphics::Font::ms_indexBuffer = nullptr;

namespace
{
	eae6320::Graphics::CommonData *commonData = eae6320::Graphics::CommonData::GetCommonData();
}

void eae6320::Graphics::Font::RenderText(const MeshData& i_meshData)
{
	EAE6320_ASSERT(ms_vertexBuffer != NULL);
	EAE6320_ASSERT(ms_indexBuffer != NULL);

	// Update the vertex buffer
	{
		// Get a pointer from Direct3D that can be written to
		void* memoryToWriteTo;
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			{
				// Discard previous contents when writing
				const unsigned int noSubResources = 0;
				const D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
				const unsigned int noFlags = 0;
				const HRESULT result = commonData->s_direct3dImmediateContext->Map(ms_vertexBuffer, noSubResources, mapType, noFlags, &mappedSubResource);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					Logging::OutputError("Direct3D failed to map the sprite vertex buffer");
					return;
				}
			}
			memoryToWriteTo = mappedSubResource.pData;
		}
		// Fill in the memory that Direct3D has provided
		{
			memcpy(memoryToWriteTo, i_meshData.vertexData, i_meshData.numberOfVertices * sizeof(MeshData::Vertex));
		}
		// Let Direct3D know that the memory contains the data
		// (the pointer will be invalid after this call)
		{
			const unsigned int noSubResources = 0;
			commonData->s_direct3dImmediateContext->Unmap(ms_vertexBuffer, noSubResources);
		}
	}

	// Update the Index buffer
	{
		// Get a pointer from Direct3D that can be written to
		void* memoryToWriteTo;
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			{
				// Discard previous contents when writing
				const unsigned int noSubResources = 0;
				const D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
				const unsigned int noFlags = 0;
				const HRESULT result = commonData->s_direct3dImmediateContext->Map(ms_indexBuffer, noSubResources, mapType, noFlags, &mappedSubResource);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					Logging::OutputError("Direct3D failed to map the sprite vertex buffer");
					return;
				}
			}
			memoryToWriteTo = mappedSubResource.pData;
		}
		// Fill in the memory that Direct3D has provided
		{
			memcpy(memoryToWriteTo, i_meshData.indexData, i_meshData.numberOfIndices * sizeof(uint16_t));
		}
		// Let Direct3D know that the memory contains the data
		// (the pointer will be invalid after this call)
		{
			const unsigned int noSubResources = 0;
			commonData->s_direct3dImmediateContext->Unmap(ms_indexBuffer, noSubResources);
		}
	}

	// Bind the vertex buffer to the device as a data source
	{
		const unsigned int startingSlot = 0;
		const unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		const unsigned int bufferStride = sizeof(MeshData::Vertex);

		// It's possible to start streaming data in the middle of a vertex buffer
		const unsigned int bufferOffset = 0;
		commonData->s_direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &ms_vertexBuffer, &bufferStride, &bufferOffset);
	}
	// Specify what kind of data the vertex buffer holds
	{
		// Set the layout (which defines how to interpret a single vertex)
		//commonData->s_direct3dImmediateContext->IASetInputLayout(commonData->ms_vertexLayout);
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// we have defined the vertex buffer as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		//commonData->s_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	{
		// Every index is a 16 bit unsigned integer
		const DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;

		// The indices start at the beginning of the buffer
		const unsigned int offset = 0;
		commonData->s_direct3dImmediateContext->IASetIndexBuffer(ms_indexBuffer, format, offset);
	}
	// Render triangles from the currently-bound vertex buffer
	{

		// It's possible to start rendering primitives in the middle of the stream
		const unsigned int indexOfFirstIndexToUse = 0;
		const unsigned int offsetToAddToEachIndex = 0;
		commonData->s_direct3dImmediateContext->DrawIndexed(i_meshData.numberOfIndices, indexOfFirstIndexToUse, offsetToAddToEachIndex);
	}
}

bool eae6320::Graphics::Font::Initialize()
{
	D3D11_BUFFER_DESC bufferDescriptionVertexBuffer = { 0 };
	{
		const size_t vertexBufferSize = 1000 * static_cast<size_t>(4) * sizeof(MeshData::Vertex);
		EAE6320_ASSERT(vertexBufferSize < (uint64_t(1u) << (sizeof(bufferDescriptionVertexBuffer.ByteWidth) * 8)));
		bufferDescriptionVertexBuffer.ByteWidth = static_cast<unsigned int>(vertexBufferSize);
		bufferDescriptionVertexBuffer.Usage = D3D11_USAGE_DYNAMIC;	// The buffer will change frequently
		bufferDescriptionVertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescriptionVertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// The CPU will write to the buffer
		bufferDescriptionVertexBuffer.MiscFlags = 0;
		bufferDescriptionVertexBuffer.StructureByteStride = 0;	// Not used
	}
	MeshData::Vertex vertexData[1000 * 4];
	{
		// Lower Left
		{
			MeshData::Vertex& vertex = vertexData[0];
			vertex.x = -1.0f;
			vertex.y = -1.0f;
			vertex.z = 0.0f;
			vertex.u = 0;
			vertex.v = 0;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		// Upper Left
		{
			MeshData::Vertex& vertex = vertexData[1];
			vertex.x = -1.0f;
			vertex.y = 1.0f;
			vertex.z = 0.0f;
			vertex.u = 0;
			vertex.v = 0;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		// Lower Right
		{
			MeshData::Vertex& vertex = vertexData[2];
			vertex.x = 1.0f;
			vertex.y = -1.0f;
			vertex.z = 0.0f;
			vertex.u = 0;
			vertex.v = 0;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		// Upper Right
		{
			MeshData::Vertex& vertex = vertexData[3];
			vertex.x = 1.0f;
			vertex.y = 1.0f;
			vertex.z = 0.0f;
			vertex.u = 0;
			vertex.v = 0;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
	}
	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		initialData.pSysMem = vertexData;
		// (The other data members are ignored for non-texture buffers)
	}
	const HRESULT resultVertexBuffer = commonData->s_direct3dDevice->CreateBuffer(&bufferDescriptionVertexBuffer, &initialData, &ms_vertexBuffer);
	if (FAILED(resultVertexBuffer))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer for sprites with HRESULT %#010x", resultVertexBuffer);
		return false;
	}

	uint16_t indexData[1000 * 6] = { 0,1,2,3,2,1 };
	//Index Buffer init
	const unsigned int indexBufferSize = 1000 * 6 * sizeof(uint16_t);
	D3D11_BUFFER_DESC bufferDescriptionIndexBuffer = { 0 };
	{
		EAE6320_ASSERT(indexBufferSize < (uint64_t(1u) << (sizeof(bufferDescriptionIndexBuffer.ByteWidth) * 8)));
		bufferDescriptionIndexBuffer.ByteWidth = indexBufferSize;
		bufferDescriptionIndexBuffer.Usage = D3D11_USAGE_DYNAMIC;  // The buffer will change frequently
		bufferDescriptionIndexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDescriptionIndexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// The CPU will write to the buffer
		bufferDescriptionIndexBuffer.MiscFlags = 0;
		bufferDescriptionIndexBuffer.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialDataIndexBuffer = { 0 };
	{
		initialDataIndexBuffer.pSysMem = indexData;
	}
	// (The other data members are ignored for non-texture buffers)


	const HRESULT resultIndexBuffer = commonData->s_direct3dDevice->CreateBuffer(&bufferDescriptionIndexBuffer, &initialDataIndexBuffer, &ms_indexBuffer);
	if (FAILED(resultIndexBuffer))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the index buffer with HRESULT %#010x", resultIndexBuffer);
		return false;
	}
	return true;
}

bool eae6320::Graphics::Font::CleanUpFont()
{
	if (ms_vertexBuffer)
	{
		ms_vertexBuffer->Release();
		ms_vertexBuffer = nullptr;
	}
	if (ms_indexBuffer)
	{
		ms_indexBuffer->Release();
		ms_indexBuffer = nullptr;
	}
	return true;
}