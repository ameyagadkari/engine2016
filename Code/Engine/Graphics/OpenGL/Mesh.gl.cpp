// Header Files
//=============

#include "../Mesh.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"


bool eae6320::Graphics::Mesh::Initialize(void *meshData)
{
	MeshData<uint16_t> *meshData_16 = NULL;
	MeshData<uint32_t> *meshData_32 = NULL;

	if (is16bit)
	{
		meshData_16 = reinterpret_cast<MeshData<uint16_t>*>(meshData);
		numberOfIndices_16 = meshData_16->numberOfIndices;
	}
	else
	{
		meshData_32 = reinterpret_cast<MeshData<uint32_t>*>(meshData);
		numberOfIndices_32 = meshData_32->numberOfIndices;
	}
	bool wereThereErrors = false;
	GLuint vertexBufferId = 0;
	GLuint indexBufferId = 0;

	// Create a vertex array object and make it active
	{
		const GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &s_vertexArrayId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(s_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Create a vertex buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &vertexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Assign the data to the buffer
	{
		//Vextex Buffer init
		unsigned int vertexBufferSize = 0;
		if (is16bit)
		{
			vertexBufferSize = meshData_16->numberOfVertices * sizeof(MeshData<uint16_t>::Vertex);
		}
		else
		{
			vertexBufferSize = meshData_32->numberOfVertices * sizeof(MeshData<uint32_t>::Vertex);
		}
		if (is16bit)
		{
			if (meshData_16->vertexData)
			{
				glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, reinterpret_cast<GLvoid*>(meshData_16->vertexData),
					// In our class we won't ever read from the buffer
					GL_STATIC_DRAW);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer because there is no Vertex Data");
				goto OnExit;
			}
		}
		else
		{
			if (meshData_32->vertexData)
			{
				glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, reinterpret_cast<GLvoid*>(meshData_32->vertexData),
					// In our class we won't ever read from the buffer
					GL_STATIC_DRAW);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer because there is no Vertex Data");
				goto OnExit;
			}
		}
	}
	// Initialize the vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		GLsizei stride = 0;
		if (is16bit)
		{
			stride = sizeof(MeshData<uint16_t>::Vertex);
		}
		else
		{
			stride = sizeof(MeshData<uint32_t>::Vertex);
		}


		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			const GLuint vertexElementLocation = 0;
			const GLint elementCount = 3;
			const GLboolean isNormalized = GL_FALSE;	// The given floats should be used as-is
			if (is16bit)
			{
				glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, isNormalized, stride,
					reinterpret_cast<GLvoid*>(offsetof(MeshData<uint16_t>::Vertex, x)));
			}
			else
			{
				glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, isNormalized, stride,
					reinterpret_cast<GLvoid*>(offsetof(MeshData<uint32_t>::Vertex, x)));
			}	
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}

		{
			const GLuint vertexElementLocation = 1;
			const GLint elementCount = 4;
			const GLboolean isNormalized = GL_TRUE;	// The given floats should be used as-is
			if (is16bit)
			{
				glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, isNormalized, stride,
					reinterpret_cast<GLvoid*>(offsetof(MeshData<uint16_t>::Vertex, r)));
			}
			else
			{
				glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, isNormalized, stride,
					reinterpret_cast<GLvoid*>(offsetof(MeshData<uint32_t>::Vertex, r)));
			}
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the COLOR vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the COLOR vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
	}

	// Create a index buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &indexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the index buffer: %s",
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
		//Index Buffer init
		unsigned int indexBufferSize = 0;
		if (is16bit)
		{
			indexBufferSize = meshData_16->numberOfIndices * sizeof(uint16_t);
		}
		else
		{
			indexBufferSize = meshData_32->numberOfIndices * sizeof(uint32_t);
		}
		if (is16bit)
		{
			if (meshData_16->indexData)
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, reinterpret_cast<GLvoid*>(meshData_16->indexData),
					// In our class we won't ever read from the buffer
					GL_STATIC_DRAW);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the index buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("OpenGL failed to allocate the index buffer because there is no Index Data");
				goto OnExit;
			}
		}
		else
		{
			if (meshData_32->indexData)
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, reinterpret_cast<GLvoid*>(meshData_32->indexData),
					// In our class we won't ever read from the buffer
					GL_STATIC_DRAW);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the index buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("OpenGL failed to allocate the index buffer because there is no Index Data");
				goto OnExit;
			}
		}
		
	}

OnExit:

	if (s_vertexArrayId != 0)
	{
		// Unbind the vertex array
		// (this must be done before deleting the vertex buffer)
		glBindVertexArray(0);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// The vertex and index buffer objects can be freed
			// (the vertex array object will still hold references to them,
			// and so they won't actually goes away until it gets freed).
			// Unfortunately debuggers don't work well when these are freed
			// (gDEBugger just doesn't show anything and RenderDoc crashes),
			// and so don't free them if debug info is enabled.
			if (vertexBufferId != 0)
			{
#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &vertexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to create vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
				vertexBufferId = 0;
#else
				s_vertexBufferId = vertexBufferId;
#endif
			}

			if (indexBufferId != 0)
			{
#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &indexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to create index buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
				indexBufferId = 0;
#else
				s_indexBufferId = indexBufferId;
#endif
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to unbind the vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	return !wereThereErrors;

}

bool eae6320::Graphics::Mesh::CleanUp()
{
	bool wereThereErrors = false;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
	if (s_vertexBufferId != 0)
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &s_vertexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_vertexBufferId = 0;
	}

	if (s_indexBufferId != 0)
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &s_indexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the index buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_indexBufferId = 0;
	}
#endif
	if (s_vertexArrayId != 0)
	{
		const GLsizei arrayCount = 1;
		glDeleteVertexArrays(arrayCount, &s_vertexArrayId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_vertexArrayId = 0;
	}
	return !wereThereErrors;
}

void eae6320::Graphics::Mesh::RenderMesh()
{
	// Bind a specific vertex buffer to the device as a data source
	{
		glBindVertexArray(s_vertexArrayId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	// Render triangles from the currently-bound vertex buffer
	{
		// The mode defines how to interpret multiple vertices as a single "primitive";
		// we define a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		const GLenum mode = GL_TRIANGLES;
		// Every index is a 16 bit unsigned integer
		GLenum indexType;
		if (is16bit)
		{
			indexType = GL_UNSIGNED_SHORT;
		}
		else
		{
			indexType = GL_UNSIGNED_INT;
		}
		// It's possible to start rendering primitives in the middle of the stream
		const GLvoid* const offset = 0;
		if (is16bit)
		{
			glDrawElements(mode, numberOfIndices_16, indexType, offset);
		}
		else
		{
			glDrawElements(mode, numberOfIndices_32, indexType, offset);
		}
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}