/*
This file manages mesh-related functionality
*/

#ifndef EAE6320_MESH_H
#define EAE6320_MESH_H

#if defined( EAE6320_PLATFORM_D3D )
#include<D3D11.h>
#define MINIMUM_NUMBER_OF_VERTICES 3
#define MINIMUM_NUMBER_OF_INDICES 3
#elif defined( EAE6320_PLATFORM_GL )
#include "OpenGL\Includes.h"
#include "Configuration.h"
#define MINIMUM_NUMBER_OF_VERTICES 3
#define MINIMUM_NUMBER_OF_INDICES 3
#endif	

#include<cstdint>
#include "CommonData.h"
// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{	
		class Mesh
		{
		public:
			bool Initialize();
			bool CleanUp();
			void RenderMesh();
//#if defined( EAE6320_PLATFORM_D3D )
//			bool isMeshInitialized = false;
//#endif
		private:
			uint16_t numberOfVertices = 4;
			uint16_t numberOfIndices = 6;

			CommonData::sVertex *vertexData = NULL;
			uint16_t *indexData = NULL;
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



