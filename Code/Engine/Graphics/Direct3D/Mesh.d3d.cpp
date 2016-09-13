// Header Files
//=============

#include "../Mesh.h"
#include "../CommonData.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"

bool eae6320::Graphics::Mesh::Initialize()
{
	CommonData *commonData = CommonData::GetCommonData();
	// Create the vertex layout
	{
		// These elements must match the VertexFormat::sVertex layout struct exactly.
		// They instruct Direct3D how to match the binary data in the vertex buffer
		// to the input elements in a vertex shader
		// (by using so-called "semantic" names so that, for example,
		// "POSITION" here matches with "POSITION" in shader code).
		// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
		const unsigned int vertexElementCount = 1;
		D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = { 0 };
		{
			// Slot 0

			// POSITION
			// 2 floats == 8 bytes
			// Offset = 0
			{
				D3D11_INPUT_ELEMENT_DESC& positionElement = layoutDescription[0];

				positionElement.SemanticName = "POSITION";
				positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
				positionElement.Format = DXGI_FORMAT_R32G32_FLOAT;
				positionElement.InputSlot = 0;
				positionElement.AlignedByteOffset = offsetof(CommonData::CommonData::sVertex, x);
				positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
			}
		}
		const HRESULT result = commonData->s_direct3dDevice->CreateInputLayout(layoutDescription, vertexElementCount,
			commonData->compiledVertexShader->GetBufferPointer(), commonData->compiledVertexShader->GetBufferSize(), &commonData->s_vertexLayout);
		if (FAILED(result))
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create a vertex input layout with HRESULT %#010x", result);
			return false;
		}
		if (commonData->compiledVertexShader)
		{
			commonData->compiledVertexShader->Release();
			commonData->compiledVertexShader = NULL;
		}

	}

	// Eventually the vertex data should come from a file but for now it is hard-coded here.
	// You will have to update this in a future assignment
	// (one of the most common mistakes in the class is to leave hard-coded values here).

	const unsigned int triangleCount = 2;
	const unsigned int vertexCountPerTriangle = 3;
	const unsigned int vertexCount = triangleCount * vertexCountPerTriangle;
	const unsigned int bufferSize = vertexCount * sizeof(CommonData::CommonData::sVertex);
	CommonData::CommonData::sVertex vertexData[vertexCount];
	{
		vertexData[0].x = 0.0f;
		vertexData[0].y = 0.0f;

		vertexData[1].x = 1.0f;
		vertexData[1].y = 1.0f;

		vertexData[2].x = 1.0f;
		vertexData[2].y = 0.0f;

		vertexData[3].x = 0.0f;
		vertexData[3].y = 0.0f;

		vertexData[4].x = 0.0f;
		vertexData[4].y = 1.0f;

		vertexData[5].x = 1.0f;
		vertexData[5].y = 1.0f;
	}

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	{
		bufferDescription.ByteWidth = bufferSize;
		bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		initialData.pSysMem = vertexData;
		// (The other data members are ignored for non-texture buffers)
	}

	const HRESULT result = commonData->s_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &s_vertexBuffer);
	if (FAILED(result))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer with HRESULT %#010x", result);
		return false;
	}

	return true;

}

bool eae6320::Graphics::Mesh::CleanUp()
{
	bool wereThereErrors = false;
	if (s_vertexBuffer)
	{
		s_vertexBuffer->Release();
		s_vertexBuffer = NULL;
	}
	return !wereThereErrors;
}

void eae6320::Graphics::Mesh::RenderMesh()
{
	CommonData *commonData = CommonData::CommonData::GetCommonData();
	// Bind a specific vertex buffer to the device as a data source
	{
		const unsigned int startingSlot = 0;
		const unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		const unsigned int bufferStride = sizeof(CommonData::CommonData::sVertex);
		// It's possible to start streaming data in the middle of a vertex buffer
		const unsigned int bufferOffset = 0;
		commonData->s_direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &s_vertexBuffer, &bufferStride, &bufferOffset);
	}
	// Specify what kind of data the vertex buffer holds
	{
		// Set the layout (which defines how to interpret a single vertex)
		commonData->s_direct3dImmediateContext->IASetInputLayout(commonData->s_vertexLayout);
		// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
		// we have defined the vertex buffer as a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		commonData->s_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	// Render triangles from the currently-bound vertex buffer
	{
		// As of this comment we are only drawing a single triangle
		// (you will have to update this code in future assignments!)
		const unsigned int triangleCount = 2;
		const unsigned int vertexCountPerTriangle = 3;
		const unsigned int vertexCountToRender = triangleCount * vertexCountPerTriangle;
		// It's possible to start rendering primitives in the middle of the stream
		const unsigned int indexOfFirstVertexToRender = 0;
		commonData->s_direct3dImmediateContext->Draw(vertexCountToRender, indexOfFirstVertexToRender);
	}
}
