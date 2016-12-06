// Header Files
//=============

#include "../cSprite.h"

// <windows.h> is #inluded before <D3D11.h> so that it is #included consistently.
// I didn't think about this and should change this next year so that D3D has it's own Includes.h file like OpenGL does :(
#include "../../Windows/Includes.h"
#include <D3D11.h>
#include "../MeshData.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../CommonData.h"

// Static Data Initialization
//===========================

ID3D11Buffer* eae6320::Graphics::cSprite::ms_vertexBuffer = NULL;

namespace
{
	// When the topology is a quad Direct3D only needs 4 vertices
	const unsigned int s_vertexCount = 4;
	eae6320::Graphics::CommonData *commonData = eae6320::Graphics::CommonData::GetCommonData();
}

// Interface
//==========

// Render
//-------

void eae6320::Graphics::cSprite::Draw() const
{
	EAE6320_ASSERT(ms_vertexBuffer != NULL);

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
			MeshData::Vertex* const vertexData = reinterpret_cast<MeshData::Vertex*>(memoryToWriteTo);
			// Position
			{
				// Lower Left
				{
					MeshData::Vertex& vertex = vertexData[0];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.bottom;
					vertex.z = 0.0f;
				}
				// Upper Left
				{
					MeshData::Vertex& vertex = vertexData[1];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.top;
					vertex.z = 0.0f;
				}
				// Lower Right
				{
					MeshData::Vertex& vertex = vertexData[2];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.bottom;
					vertex.z = 0.0f;
				}
				// Upper Right
				{
					MeshData::Vertex& vertex = vertexData[3];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.top;
					vertex.z = 0.0f;
				}
			}
			// Texture Coordinates
			{
				// Lower Left
				{
					MeshData::Vertex& vertex = vertexData[0];
					vertex.u = m_textureCoordinates.left;
					vertex.v = m_textureCoordinates.bottom;
				}
				// Upper Left
				{
					MeshData::Vertex& vertex = vertexData[1];
					vertex.u = m_textureCoordinates.left;
					vertex.v = m_textureCoordinates.top;
				}
				// Lower Right
				{
					MeshData::Vertex& vertex = vertexData[2];
					vertex.u = m_textureCoordinates.right;
					vertex.v = m_textureCoordinates.bottom;
				}
				// Upper Right
				{
					MeshData::Vertex& vertex = vertexData[3];
					vertex.u = m_textureCoordinates.right;
					vertex.v = m_textureCoordinates.top;
				}
			}
			// Colors
			for (unsigned int i = 0; i < s_vertexCount; ++i)
			{
				MeshData::Vertex& vertex = vertexData[i];
				vertex.r = vertex.g = vertex.b = vertex.a = 255;
			}
		}
		// Let Direct3D know that the memory contains the data
		// (the pointer will be invalid after this call)
		{
			const unsigned int noSubResources = 0;
			commonData->s_direct3dImmediateContext->Unmap(ms_vertexBuffer, noSubResources);
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
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// we have defined the vertex buffer as a quad, which means that a triangle "strip" will work
		// (meaning that the first primitive will be a triangle defined by three vertices
		// and the second primitive will be a triangle defined by the two most recent vertices and one new vertex)
		commonData->s_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}
	// Render triangles from the currently-bound vertex buffer
	{
		// It's possible to start rendering primitives in the middle of the stream
		const unsigned int indexOfFirstVertexToUse = 0;
		commonData->s_direct3dImmediateContext->Draw(s_vertexCount, indexOfFirstVertexToUse);
	}
}

// Initialization / Clean Up
//--------------------------

bool eae6320::Graphics::cSprite::Initialize()
{
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	{
		const size_t bufferSize = static_cast<size_t>(s_vertexCount) * sizeof(MeshData::Vertex);
		EAE6320_ASSERT(bufferSize < (uint64_t(1u) << (sizeof(bufferDescription.ByteWidth) * 8)));
		bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;	// The buffer will change frequently
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// The CPU will write to the buffer
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;	// Not used
	}
	MeshData::Vertex vertexData[s_vertexCount];
	{
		// Lower Left
		{
			MeshData::Vertex& vertex = vertexData[0];
			vertex.x = -1.0f;
			vertex.y = -1.0f;
			vertex.z = 0.0f;
			vertex.u = zeroAsHalf;
			vertex.v = oneAsHalf;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		// Upper Left
		{
			MeshData::Vertex& vertex = vertexData[1];
			vertex.x = -1.0f;
			vertex.y = 1.0f;
			vertex.z = 0.0f;
			vertex.u = zeroAsHalf;
			vertex.v = zeroAsHalf;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		// Lower Right
		{
			MeshData::Vertex& vertex = vertexData[2];
			vertex.x = 1.0f;
			vertex.y = -1.0f;
			vertex.z = 0.0f;
			vertex.u = oneAsHalf;
			vertex.v = oneAsHalf;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		// Upper Right
		{
			MeshData::Vertex& vertex = vertexData[3];
			vertex.x = 1.0f;
			vertex.y = 1.0f;
			vertex.z = 0.0f;
			vertex.u = oneAsHalf;
			vertex.v = zeroAsHalf;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
	}
	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		initialData.pSysMem = vertexData;
		// (The other data members are ignored for non-texture buffers)
	}
	const HRESULT result = commonData->s_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &ms_vertexBuffer);
	if (SUCCEEDED(result))
	{
		return true;
	}
	else
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer for sprites with HRESULT %#010x", result);
		return false;
	}
}

bool eae6320::Graphics::cSprite::CleanUp()
{
	if (ms_vertexBuffer)
	{
		ms_vertexBuffer->Release();
		ms_vertexBuffer = NULL;
	}

	return true;
}
