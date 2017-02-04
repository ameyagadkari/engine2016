/*
This file manages font-related functionality
*/

#ifndef EAE6320_FONT_H
#define EAE6320_FONT_H
#include <cstdint>

// Forward Declarations
//=====================
namespace eae6320
{
	namespace Graphics
	{
		struct MeshData;
		struct FontData;
		class Material;
		namespace Sprite
		{
			struct TextureCoordinates;
		}
	}
}

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#elif defined( EAE6320_PLATFORM_GL )
typedef unsigned int GLuint;
#endif

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class Font
		{
		public:
			static bool LoadFont(const char * const i_relativePath);
			static bool CleanUp();
			static void RenderText(const MeshData& i_meshData);
			static Material* ms_material;
			static uint8_t* widthArray;
			static Sprite::TextureCoordinates* texcoordArray;
		private:
			Font() = default;
			static bool Initialize();
			static bool CleanUpFont();				
			// A single dynamic vertex buffer is created for _all_ sprites,
			// and its contents are changed every draw call
#if defined( EAE6320_PLATFORM_D3D )
			// A vertex buffer holds the data for each vertex
			static ID3D11Buffer* ms_vertexBuffer;
			static ID3D11Buffer* ms_indexBuffer;
#elif defined( EAE6320_PLATFORM_GL )
			// A vertex array encapsulates the vertex data and the vertex input layout
			static GLuint ms_vertexArrayId;
			static GLuint ms_vertexBufferId;
			static GLuint ms_indexBufferId;
#endif
		};
	}
}
#endif	// EAE6320_FONT_H