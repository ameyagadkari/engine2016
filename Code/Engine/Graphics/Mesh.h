/*
This file manages mesh-related functionality
*/

#ifndef EAE6320_MESH_H
#define EAE6320_MESH_H

#if defined( EAE6320_PLATFORM_D3D )
#include<D3D11.h>
#define MINIMUM_NUMBER_OF_VERTICES 3
#define MINIMUM_NUMBER_OF_INDICES 3
#define MAXIMUM_NUMBER_OF_VERTICES USHRT_MAX
#define MAXIMUM_NUMBER_OF_INDICES USHRT_MAX
#elif defined( EAE6320_PLATFORM_GL )
#include "OpenGL\Includes.h"
#include "Configuration.h"
#define MINIMUM_NUMBER_OF_VERTICES 3
#define MINIMUM_NUMBER_OF_INDICES 3
#define MAXIMUM_NUMBER_OF_VERTICES USHRT_MAX
#define MAXIMUM_NUMBER_OF_INDICES USHRT_MAX
#endif	

#include "MeshData.h"
#include "CommonData.h"
#include "ConstantBufferData.h"
// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class Mesh
		{
		public:
			static Mesh* LoadMesh(const char* const relativePath);
			bool CleanUp();
			void RenderMesh();		
		private:		
			uint16_t numberOfIndices = 0;
			ConstantBufferData::sDrawCall drawCallData;
			bool Initialize(MeshData&meshData);		
#if defined( EAE6320_PLATFORM_D3D )
			// The vertex buffer holds the data for each vertex
			ID3D11Buffer* s_vertexBuffer = NULL;
			ID3D11Buffer* s_indexBuffer = NULL;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint s_vertexArrayId = 0;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			// OpenGL debuggers don't seem to support freeing the vertex buffer
			// and letting the vertex array object hold a reference to it,
			// and so if debug info is enabled an explicit reference is held
			GLuint s_vertexBufferId = 0;
			GLuint s_indexBufferId = 0;
#endif
#endif			
		};
	}
}
#endif	// EAE6320_MESH_H



