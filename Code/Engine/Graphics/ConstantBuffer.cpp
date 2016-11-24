#include "ConstantBuffer.h"
#include "../Logging/Logging.h"
#include "../Asserts/Asserts.h"


eae6320::Graphics::ConstantBuffer::ConstantBuffer() :
	m_sizeOfConstantBuffer(0),
#if defined( EAE6320_PLATFORM_D3D )
	m_constantBuffer(NULL)
#elif defined( EAE6320_PLATFORM_GL ) 
	m_constantBufferId(0)
#endif
{}

eae6320::Graphics::ConstantBuffer::~ConstantBuffer()
{
	if (!CleanUpConstantBuffer())
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Constant buffer cleanup failed");
	}
}

bool eae6320::Graphics::ConstantBuffer::InitializeConstantBuffer(ConstantBufferType constantBufferType, size_t sizeOfConstantBuffer, void* initialBufferData)
{
	this->m_constantBufferType = constantBufferType;
	this->m_sizeOfConstantBuffer = sizeOfConstantBuffer;
	bool wereThereErrors = false;
	if (!CreateConstantBuffer(initialBufferData))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Failed to create constant buffer of type %d", constantBufferType);
	}
	return !wereThereErrors;
}