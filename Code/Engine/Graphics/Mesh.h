/*
This file manages mesh-related functionality
*/

#ifndef EAE6320_MESH_H
#define EAE6320_MESH_H

#if defined( EAE6320_PLATFORM_D3D )
#include<D3D11.h>
#elif defined( EAE6320_PLATFORM_GL )
#include "OpenGL\Includes.h"
#include "Configuration.h"
#endif	

#include <cstdint>


// Forward Declarations
//=====================
namespace eae6320
{
	namespace Graphics
	{
		struct MeshData;
	}
}

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class Mesh
		{
		public:
			static bool LoadMesh(const char * const i_relativePath, Mesh &o_mesh);
			bool CleanUp();
			void RenderMesh()const;	
			Mesh();
			~Mesh();
		private:		
			uint32_t m_numberOfIndices;
			bool m_is16bit;
			bool Initialize(const MeshData &i_meshData);		
#if defined( EAE6320_PLATFORM_D3D )
			// The vertex buffer holds the data for each vertex
			ID3D11Buffer* m_vertexBuffer;
			ID3D11Buffer* m_indexBuffer;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_vertexArrayId;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			// OpenGL debuggers don't seem to support freeing the vertex buffer
			// and letting the vertex array object hold a reference to it,
			// and so if debug info is enabled an explicit reference is held
			GLuint m_vertexBufferId;
			GLuint m_indexBufferId;
#endif
#endif			
		};
	}
}
#endif	// EAE6320_MESH_H