#include<iostream>
#include<cmath>

#include "Mesh.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../Platform/Platform.h"
#include "MeshData.h"

//#include <chrono>

eae6320::Graphics::Mesh::Mesh() :
	m_numberOfIndices(0),
	m_is16bit(true),
#if defined( EAE6320_PLATFORM_D3D )
	m_vertexBuffer(NULL),
	m_indexBuffer(NULL)
#elif defined( EAE6320_PLATFORM_GL )
	m_vertexArrayId(0)
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
	,m_vertexBufferId(0),
	m_indexBufferId(0)
#endif
#endif
{}

eae6320::Graphics::Mesh::~Mesh()
{
	if (!CleanUp())
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Mesh cleanup failed");
	}
}

bool eae6320::Graphics::Mesh::LoadMesh(const char * const i_relativePath, Mesh &o_mesh)
{
	//auto begin = std::chrono::high_resolution_clock::now();
	//std::chrono::time_point<std::chrono::steady_clock> end;
	//long long ms;

	bool wereThereErrors = false;
	MeshData *meshData = NULL;

	// Load the binary mesh file
	eae6320::Platform::sDataFromFile binaryMesh;
	{
		std::string errorMessage;
		if (!eae6320::Platform::LoadBinaryFile(i_relativePath, binaryMesh, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the binary mesh file \"%s\": %s", i_relativePath, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Casting data to uint8_t* for pointer arithematic
	uint8_t* data = reinterpret_cast<uint8_t*>(binaryMesh.data);

	meshData = reinterpret_cast<MeshData*>(malloc(sizeof(MeshData)));

	// Extracting Binary Data
	{
		// Extracting Type Of IndexData		
		meshData->typeOfIndexData = *reinterpret_cast<uint32_t*>(data);

		// Extracting Number Of Vertices
		data += sizeof(uint32_t);
		meshData->numberOfVertices = *reinterpret_cast<uint32_t*>(data);

		// Extracting Number Of Indices
		data += sizeof(uint32_t);
		meshData->numberOfIndices = *reinterpret_cast<uint32_t*>(data);

		// Extracting Vertex Array
		data += sizeof(uint32_t);
		meshData->vertexData = reinterpret_cast<MeshData::Vertex*>(data);

		// Extracting Index Array
		data += meshData->numberOfVertices * sizeof(MeshData::Vertex);
		meshData->indexData = data;
	}

	//end = std::chrono::high_resolution_clock::now();
	//ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	//Logging::OutputMessage("%lld", ms);

	if (!o_mesh.Initialize(*meshData))
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Failed to initialize mesh: %s", i_relativePath);
		goto OnExit;
	}


OnExit:
	if (meshData)
	{
		free(meshData);
	}

	binaryMesh.Free();

	return !wereThereErrors;
}