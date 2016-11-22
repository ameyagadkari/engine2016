#include "ConstantBuffer.h"
#include "../Logging/Logging.h"
#include "../Asserts/Asserts.h"

bool eae6320::Graphics::ConstantBuffer::InitializeConstantBuffer(ConstantBufferType constantBufferType, size_t sizeOfConstantBuffer, void* initialBufferData)
{
	this->constantBufferType = constantBufferType;
	this->sizeOfConstantBuffer = sizeOfConstantBuffer;
	bool wereThereErrors = false;
	if (!CreateConstantBuffer(initialBufferData))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Failed to create constant buffer of type %d", constantBufferType);
	}
	return !wereThereErrors;
}