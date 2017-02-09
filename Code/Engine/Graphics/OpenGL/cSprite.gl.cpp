// Header Files
//=============

#include "../cSprite.h"

#include "../MeshData.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "Includes.h"
#include "UtilityFunctions.gl.h"

// Static Data Initialization
//===========================

GLuint eae6320::Graphics::cSprite::ms_vertexArrayId = 0;
GLuint eae6320::Graphics::cSprite::ms_vertexBufferId = 0;

namespace
{
	// When the topology is a quad OpenGL only needs 4 vertices
	const unsigned int s_vertexCount = 4;
}

// Interface
//==========

// Render
//-------

void eae6320::Graphics::cSprite::Draw() const
{
	// Bind the vertex buffer to the device as a data source
	{
		glBindVertexArray(ms_vertexArrayId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	// Update the vertex buffer
	{
		MeshData::Vertex vertexData[s_vertexCount];
		{
			// Position
			{
				// Lower Left
				{
					MeshData::Vertex& vertex = vertexData[0];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.bottom;
					vertex.z = -1.0f;
				}
				// Lower Right
				{
					MeshData::Vertex& vertex = vertexData[1];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.bottom;
					vertex.z = -1.0f;
				}
				// Upper Left
				{
					MeshData::Vertex& vertex = vertexData[2];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.top;
					vertex.z = -1.0f;
				}
				// Upper Right
				{
					MeshData::Vertex& vertex = vertexData[3];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.top;
					vertex.z = -1.0f;
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
				// Lower Right
				{
					MeshData::Vertex& vertex = vertexData[1];
					vertex.u = m_textureCoordinates.right;
					vertex.v = m_textureCoordinates.bottom;
				}
				// Upper Left
				{
					MeshData::Vertex& vertex = vertexData[2];
					vertex.u = m_textureCoordinates.left;
					vertex.v = m_textureCoordinates.top;
				}
				// Upper Right
				{
					MeshData::Vertex& vertex = vertexData[3];
					vertex.u = m_textureCoordinates.right;
					vertex.v = m_textureCoordinates.top;
				}
			}
			// Colors
			for ( unsigned int i = 0; i < s_vertexCount; ++i )
			{
				MeshData::Vertex& vertex = vertexData[i];
				vertex.r = vertex.g = vertex.b = vertex.a = 255;
			}
		}
		// Make the vertex buffer active
		{
			glBindBuffer( GL_ARRAY_BUFFER, ms_vertexBufferId );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
		// Invalidate the old data
		{
			// This is to tell OpenGL that synchronization isn't necessary
			// (it can finish drawing with the contents of the previous buffer,
			// but there's no need to wait for the next update before drawing anything else)
			glInvalidateBufferData( ms_vertexBufferId );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
		// Re-allocate and copy the new data to the GPU
		{
			glBufferData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( sizeof( vertexData ) ), vertexData, GL_STREAM_DRAW );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
	}
	// Render triangles from the currently-bound vertex buffer
	{
		// The mode defines how to interpret multiple vertices as a single "primitive";
		// we have defined the vertex buffer as a quad, which means that a triangle "strip" will work
		// (meaning that the first primitive will be a triangle defined by three vertices
		// and the second primitive will be a triangle defined by the two most recent vertices and one new vertex)
		const GLenum mode = GL_TRIANGLE_STRIP;
		// It's possible to start rendering primitives in the middle of the stream
		const GLint indexOfFirstVertexToRender = 0;
		glDrawArrays( mode, indexOfFirstVertexToRender, static_cast<GLsizei>( s_vertexCount ) );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
}

// Initialization / Clean Up
//--------------------------

bool eae6320::Graphics::cSprite::Initialize()
{
	bool wereThereErrors = false;

	// Store the vertex data in a vertex buffer
	{
		// Create a vertex array object and make it active
		{
			const GLsizei arrayCount = 1;
			glGenVertexArrays( arrayCount, &ms_vertexArrayId );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glBindVertexArray( ms_vertexArrayId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to bind the sprites' new vertex array: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex array ID for sprites: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}

		// Create a vertex buffer object and make it active
		{
			const GLsizei bufferCount = 1;
			glGenBuffers( bufferCount, &ms_vertexBufferId );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glBindBuffer( GL_ARRAY_BUFFER, ms_vertexBufferId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to bind the sprites' new vertex buffer: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex buffer ID for the sprites: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		// Assign initial data to the buffer
		{
			MeshData::Vertex vertexData[s_vertexCount];
			{
				// Lower Left
				{
					MeshData::Vertex& vertex = vertexData[0];
					vertex.x = -1.0f;
					vertex.y = -1.0f;
					vertex.z = -1.0f;
					vertex.u = zeroAsHalf;
					vertex.v = zeroAsHalf;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				// Lower Right
				{
					MeshData::Vertex& vertex = vertexData[1];
					vertex.x = 1.0f;
					vertex.y = -1.0f;
					vertex.z = -1.0f;
					vertex.u = oneAsHalf;
					vertex.v = zeroAsHalf;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				// Upper Left
				{
					MeshData::Vertex& vertex = vertexData[2];
					vertex.x = -1.0f;
					vertex.y = 1.0f;
					vertex.z = -1.0f;
					vertex.u = zeroAsHalf;
					vertex.v = oneAsHalf;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				// Upper Right
				{
					MeshData::Vertex& vertex = vertexData[3];
					vertex.x = 1.0f;
					vertex.y = 1.0f;
					vertex.z = -1.0f;
					vertex.u = oneAsHalf;
					vertex.v = oneAsHalf;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
			}
			glBufferData( GL_ARRAY_BUFFER, sizeof( vertexData ), reinterpret_cast<const GLvoid*>( vertexData ),
				// The buffer will change frequently, and each update will only be used once for a draw call
				GL_STREAM_DRAW );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				wereThereErrors = true;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to allocate the sprites' vertex buffer: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		// Initialize the vertex format
		if (!OpenGLUtilityFunctions::InitilaizeVertexFormat())
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("OpenGL failed to initialize vertex format");
			goto OnExit;
		}
	}

OnExit:

	return !wereThereErrors;
}

bool eae6320::Graphics::cSprite::CleanUp()
{
	bool wereThereErrors = false;

	// Make sure that the vertex array isn't bound
	{
		// Unbind the vertex array
		glBindVertexArray( 0 );
		const GLenum errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			wereThereErrors = true;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to unbind all vertex arrays before deleting cleaning up the sprites: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
	}
	if ( ms_vertexBufferId != 0 )
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers( bufferCount, &ms_vertexBufferId );
		const GLenum errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			wereThereErrors = true;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the sprites' vertex buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		ms_vertexBufferId = 0;
	}
	if ( ms_vertexArrayId != 0 )
	{
		const GLsizei arrayCount = 1;
		glDeleteVertexArrays( arrayCount, &ms_vertexArrayId );
		const GLenum errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the sprites' vertex array: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		ms_vertexArrayId = 0;
	}

	return !wereThereErrors;
}
