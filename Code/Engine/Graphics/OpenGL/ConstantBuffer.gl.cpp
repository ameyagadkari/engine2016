#include "../ConstantBuffer.h"
#include "../../Logging/Logging.h"
#include "../../Asserts/Asserts.h"
#include "../../Time/Time.h"
#include "../ConstantBufferData.h"

namespace eae6320
{
	namespace Graphics
	{
		// Create a uniform buffer object and make it active
		bool ConstantBuffer::CreateConstantBuffer(ConstantBufferType constantBufferType, size_t sizeOfConstantBuffer, void* initialBufferData)
		{
			this->constantBufferType = constantBufferType;
			this->sizeOfConstantBuffer = sizeOfConstantBuffer;
			bool wereThereErrors = false;

			// Create a uniform buffer object and make it active
			{
				const GLsizei bufferCount = 1;
				glGenBuffers(bufferCount, &s_constantBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glBindBuffer(GL_UNIFORM_BUFFER, s_constantBufferId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to bind the new uniform buffer %u: %s",
							s_constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get an unused uniform buffer ID: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			// Fill in the constant buffer
			if (constantBufferType == ConstantBufferType::FRAME)
			{
				reinterpret_cast<ConstantBufferData::sFrame*>(initialBufferData)->g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
			}
			// Allocate space and copy the constant data into the uniform buffer
			{
				const GLenum usage = GL_DYNAMIC_DRAW;	// The buffer will be modified frequently and used to draw
				glBufferData(GL_UNIFORM_BUFFER, sizeOfConstantBuffer, reinterpret_cast<const GLvoid*>(initialBufferData), usage);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to allocate the new uniform buffer %u: %s",
						s_constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}

		OnExit:

			return !wereThereErrors;
		}

		void ConstantBuffer::BindingConstantBuffer(BindMode bindMode)
		{
			const GLuint bindingPointAssignedInShader = static_cast<int>(constantBufferType);
			glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointAssignedInShader, s_constantBufferId);
			EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		}

		void ConstantBuffer::UpdateConstantBuffer(void* bufferData, size_t sizeOfConstantBuffer)
		{
			/*if (constantBufferType == ConstantBufferType::FRAME)
			{
				reinterpret_cast<ConstantBufferData::sFrame*>(bufferData)->g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
			}*/
			// Make the uniform buffer active
			{
				glBindBuffer(GL_UNIFORM_BUFFER, s_constantBufferId);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			}
			// Copy the updated memory to the GPU
			{
				GLintptr updateAtTheBeginning = 0;
				GLsizeiptr updateTheEntireBuffer = 0;
				if (sizeOfConstantBuffer <= this->sizeOfConstantBuffer)
				{
					updateTheEntireBuffer = static_cast<GLsizeiptr>(sizeOfConstantBuffer);
				}
				else
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Size of the buffer is greater than initial size of the buffer");
				}
				glBufferSubData(GL_UNIFORM_BUFFER, updateAtTheBeginning, updateTheEntireBuffer, bufferData);
				EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
			}
		}

		bool ConstantBuffer::CleanUpConstantBuffer()
		{
			bool wereThereErrors = false;
			if (s_constantBufferId != 0)
			{
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &s_constantBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					Logging::OutputError("OpenGL failed to delete the constant buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
				}
				s_constantBufferId = 0;
			}
			return !wereThereErrors;
		}
	}
}