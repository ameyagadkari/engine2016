// Header Files
//=============

#include "../Font.h"

#include "../MeshData.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "Includes.h"
#include "UtilityFunctions.gl.h"
#include "../SpriteData.h"
#include "../../UserSettings/UserSettings.h"

// Static Data Initialization
//===========================

GLuint eae6320::Graphics::Font::ms_vertexArrayId = 0;
GLuint eae6320::Graphics::Font::ms_vertexBufferId = 0;
GLuint eae6320::Graphics::Font::ms_indexBufferId = 0;

void eae6320::Graphics::Font::RenderText(const char * const textToDisplay, int16_t x, int16_t y)
{
	const size_t length = strlen(textToDisplay);
	const unsigned int vertexCount = 4 * length;
	const unsigned int indexCount = 6 * length;

	const float widthMultiplier = 2.0f / UserSettings::GetResolutionWidth();
	const float heightMultiplier = 2.0f / UserSettings::GetResolutionHeight();

	Sprite::ScreenPosition* screenPosForEachQuad = reinterpret_cast<Sprite::ScreenPosition*>(malloc(length * sizeof(Sprite::ScreenPosition)));
	for (size_t i = 0; i < length; i++)
	{
		screenPosForEachQuad[i].left = x*widthMultiplier;
		screenPosForEachQuad[i].top = y*heightMultiplier;
		screenPosForEachQuad[i].right = (x + static_cast<int16_t>(widthArray[textToDisplay[i] - 32])+8)*widthMultiplier;
		screenPosForEachQuad[i].bottom = (y - 32)*heightMultiplier;
		if (i > 0)
		{
			screenPosForEachQuad[i].left += screenPosForEachQuad[i - 1].right+ 2*widthMultiplier;
			screenPosForEachQuad[i].right += screenPosForEachQuad[i].left;
		}
	}

	// Update the vertex buffer and index buffer
	{
		MeshData::Vertex* vertexData = reinterpret_cast<MeshData::Vertex*>(malloc(vertexCount * sizeof(MeshData::Vertex)));
		{
			for (size_t i = 0; i < length; i++)
			{
				vertexData[i * 4 + 0].AddVertexData(screenPosForEachQuad[i].left, screenPosForEachQuad[i].bottom, -1.0f, texcoordArray[textToDisplay[i] - 32].left, texcoordArray[textToDisplay[i] - 32].bottom);

				vertexData[i * 4 + 1].AddVertexData(screenPosForEachQuad[i].right, screenPosForEachQuad[i].bottom, -1.0f, texcoordArray[textToDisplay[i] - 32].right, texcoordArray[textToDisplay
					[i] - 32].bottom);

				vertexData[i * 4 + 2].AddVertexData(screenPosForEachQuad[i].left, screenPosForEachQuad[i].top, -1.0f, texcoordArray[textToDisplay[i] - 32].left, texcoordArray[textToDisplay[i] - 32].top);

				vertexData[i * 4 + 3].AddVertexData(screenPosForEachQuad[i].right, screenPosForEachQuad[i].top, -1.0f, texcoordArray[textToDisplay[i] - 32].right, texcoordArray[textToDisplay[i] - 32].top);
			}
		}

		uint16_t* indexData = reinterpret_cast<uint16_t*>(malloc(indexCount * sizeof(uint16_t)));
		for (size_t i = 0; i < length; i++)
		{
			indexData[i * 6 + 0] = static_cast<uint16_t>(i * 4 + 0);
			indexData[i * 6 + 1] = static_cast<uint16_t>(i * 4 + 1);
			indexData[i * 6 + 2] = static_cast<uint16_t>(i * 4 + 2);
			indexData[i * 6 + 3] = static_cast<uint16_t>(i * 4 + 3);
			indexData[i * 6 + 4] = static_cast<uint16_t>(i * 4 + 2);
			indexData[i * 6 + 5] = static_cast<uint16_t>(i * 4 + 1);
		}

		// Make the vertex buffer active
		{
			glBindBuffer(GL_ARRAY_BUFFER, ms_vertexBufferId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Invalidate the old data
		{
			// This is to tell OpenGL that synchronization isn't necessary
			// (it can finish drawing with the contents of the previous buffer,
			// but there's no need to wait for the next update before drawing anything else)
			glInvalidateBufferData(ms_vertexBufferId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Re-allocate and copy the new data to the GPU
		{
			const unsigned int vertexBufferSize = vertexCount * sizeof(MeshData::Vertex);
			glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, reinterpret_cast<GLvoid*>(vertexData),
				GL_STREAM_DRAW);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}


		// Make the index buffer active
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ms_indexBufferId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Invalidate the old data
		{
			// This is to tell OpenGL that synchronization isn't necessary
			// (it can finish drawing with the contents of the previous buffer,
			// but there's no need to wait for the next update before drawing anything else)
			glInvalidateBufferData(ms_indexBufferId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Re-allocate and copy the new data to the GPU
		{
			const unsigned int indexBufferSize = indexCount * sizeof(uint16_t);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, reinterpret_cast<GLvoid*>(indexData), GL_STREAM_DRAW);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}

		// Bind the vertex buffer to the device as a data source
		{
			glBindVertexArray(ms_vertexArrayId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		// Render triangles from the currently-bound vertex buffer
		{
			const GLenum mode = GL_TRIANGLES;
			// Every index is a 16 bit unsigned integer
			GLenum indexType = indexType = GL_UNSIGNED_SHORT;

			// It's possible to start rendering primitives in the middle of the stream
			const GLvoid* const offset = 0;
			glDrawElements(mode, indexCount, indexType, offset);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}
		if (vertexData)
		{
			free(vertexData);
			vertexData = nullptr;
		}
		if (indexData)
		{
			free(indexData);
			vertexData = nullptr;
		}
	}

	if (screenPosForEachQuad)
	{
		free(screenPosForEachQuad);
		screenPosForEachQuad = nullptr;
	}
}

bool eae6320::Graphics::Font::Initialize()
{
	bool wereThereErrors = false;

	// Store the vertex data in a vertex buffer
	{
		// Create a vertex array object and make it active
		{
			const GLsizei arrayCount = 1;
			glGenVertexArrays(arrayCount, &ms_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindVertexArray(ms_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to bind the fonts' new vertex array: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID for fonts: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}

		// Create a vertex buffer object and make it active
		{
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &ms_vertexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(GL_ARRAY_BUFFER, ms_vertexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to bind the fonts' new vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID for the sprites: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		// Assign initial data to the buffer
		{
			MeshData::Vertex vertexData[4];
			{
				// Lower Left
				{
					MeshData::Vertex& vertex = vertexData[0];
					vertex.x = -1.0f;
					vertex.y = -1.0f;
					vertex.z = -1.0f;
					vertex.u = 0;
					vertex.v = 0;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				// Lower Right
				{
					MeshData::Vertex& vertex = vertexData[1];
					vertex.x = 1.0f;
					vertex.y = -1.0f;
					vertex.z = -1.0f;
					vertex.u = 0;
					vertex.v = 0;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				// Upper Left
				{
					MeshData::Vertex& vertex = vertexData[2];
					vertex.x = -1.0f;
					vertex.y = 1.0f;
					vertex.z = -1.0f;
					vertex.u = 0;
					vertex.v = 0;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				// Upper Right
				{
					MeshData::Vertex& vertex = vertexData[3];
					vertex.x = 1.0f;
					vertex.y = 1.0f;
					vertex.z = -1.0f;
					vertex.u = 0;
					vertex.v = 0;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
			}
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), reinterpret_cast<const GLvoid*>(vertexData),
				// The buffer will change frequently, and each update will only be used once for a draw call
				GL_STREAM_DRAW);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to allocate the fonts' vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
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
	// Create a index buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &ms_indexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ms_indexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind fonts' index buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused index buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	{
		MeshData::Vertex indexData[6] = { 0,1,2,3,2,1 };
		//Index Buffer init
		const unsigned int indexBufferSize = 6 * sizeof(uint16_t);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, reinterpret_cast<GLvoid*>(indexData), GL_STREAM_DRAW);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate fonts' index buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
OnExit:

	return !wereThereErrors;
}

bool eae6320::Graphics::Font::CleanUpFont()
{
	bool wereThereErrors = false;

	// Make sure that the vertex array isn't bound
	{
		// Unbind the vertex array
		glBindVertexArray(0);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to unbind all vertex arrays before deleting cleaning up the fonts: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
	}
	if (ms_vertexBufferId != 0)
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &ms_vertexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the fonts' vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		ms_vertexBufferId = 0;
	}
	if (ms_indexBufferId != 0)
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &ms_indexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the fonts' index buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		ms_indexBufferId = 0;
	}
	if (ms_vertexArrayId != 0)
	{
		const GLsizei arrayCount = 1;
		glDeleteVertexArrays(arrayCount, &ms_vertexArrayId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the fonts' vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		ms_vertexArrayId = 0;
	}

	return !wereThereErrors;
}