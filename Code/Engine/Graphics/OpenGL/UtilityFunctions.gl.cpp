#include "UtilityFunctions.gl.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../MeshData.h"
#include "Includes.h"

bool eae6320::Graphics::OpenGLUtilityFunctions::InitilaizeVertexFormat()
{
	bool wereThereErrors = false;
	// The "stride" defines how large a single vertex is in the stream of data
	// (or, said another way, how far apart each position element is)
	const GLsizei stride = sizeof(MeshData::Vertex);
	// Position (0)
	// 3 floats == 12 bytes
	// Offset = 0
	{
		const GLuint vertexElementLocation = 0;
		const GLint elementCount = 3;
		const GLboolean isNormalized = GL_FALSE;	// The given floats should be used as-is

		glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, isNormalized, stride,
			reinterpret_cast<GLvoid*>(offsetof(MeshData::Vertex, x)));

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
		const GLboolean isNormalized = GL_TRUE;	// The given floats should be normalized

		glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, isNormalized, stride,
			reinterpret_cast<GLvoid*>(offsetof(MeshData::Vertex, r)));

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

	{
		const GLuint vertexElementLocation = 2;
		const GLint elementCount = 2;
		const GLboolean isNormalized = GL_FALSE;	// The given floats should be used as-is

		glVertexAttribPointer(vertexElementLocation, elementCount, GL_HALF_FLOAT, isNormalized, stride,
			reinterpret_cast<GLvoid*>(offsetof(MeshData::Vertex, u)));

		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glEnableVertexAttribArray(vertexElementLocation);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to enable the TEXCOORD vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to set the TEXCOORD vertex attribute at location %u: %s",
				vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

OnExit:
	return !wereThereErrors;
}
