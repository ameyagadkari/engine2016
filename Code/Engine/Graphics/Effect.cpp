#include "Effect.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../Platform/Platform.h"

bool eae6320::Graphics::Effect::LoadEffect(const char * const relativePath, Effect &o_effect)
{
	bool wereThereErrors = false;

	// Load the binary effect file
	eae6320::Platform::sDataFromFile binaryEffect;
	{
		std::string errorMessage;
		if (!eae6320::Platform::LoadBinaryFile(relativePath, binaryEffect, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the binary effect file \"%s\": %s", relativePath, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Casting data to uint8_t* for pointer arithematic
	uint8_t* data = reinterpret_cast<uint8_t*>(binaryEffect.data);

	uint8_t offsetToAdd = 0;
	const char * relativeVertexShaderPath = NULL;
	const char * relativeFragmentShaderPath = NULL;

	// Extracting Binary Data
	{
		// Extracting Offset To Add
		offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Vertex Shader Path
		data += sizeof(offsetToAdd);
		relativeVertexShaderPath = reinterpret_cast<const char *>(data);

		// Extracting Offset To Add
		data += offsetToAdd;
		offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Fragment Shader Path
		data += sizeof(offsetToAdd);
		relativeFragmentShaderPath = reinterpret_cast<const char *>(data);
	}

	if (!o_effect.InitializeEffect(relativeVertexShaderPath,relativeFragmentShaderPath))
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Failed to initialize effect: %s", relativePath);
		goto OnExit;
	}

OnExit:
	binaryEffect.Free();

	return !wereThereErrors;
}