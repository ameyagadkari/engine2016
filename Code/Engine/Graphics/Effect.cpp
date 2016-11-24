#include "Effect.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../Platform/Platform.h"
#include "../StringHandler/HashedString.h"

#include <regex>

namespace
{
	std::cmatch match;
	const std::regex pattern_match("(data/effects/)(.*)");
	const std::regex pattern_match1("(\\.)([[:alpha:]]+)");
	const std::string pattern_replace("");
}

eae6320::Graphics::Effect::Effect() :
	effectUUID(0),
#if defined( EAE6320_PLATFORM_D3D )
	m_vertexShader(NULL),
	m_fragmentShader(NULL),
	m_vertexLayout(NULL)
#elif defined( EAE6320_PLATFORM_GL )
	m_programId(0)
#endif
{}

eae6320::Graphics::Effect::~Effect()
{
	if (!CleanUpEffect())
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Effect cleanup failed");
	}
}

bool eae6320::Graphics::Effect::LoadEffect(const char * const i_relativePath, Effect & o_effect)
{
	bool wereThereErrors = false;
	std::string fileName;
	// Load the binary effect file
	eae6320::Platform::sDataFromFile binaryEffect;
	{
		std::string errorMessage;
		if (!eae6320::Platform::LoadBinaryFile(i_relativePath, binaryEffect, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the binary effect file \"%s\": %s", i_relativePath, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Casting data to uint8_t* for pointer arithematic
	uint8_t* data = reinterpret_cast<uint8_t*>(binaryEffect.data);

	// Extracting Binary Data
	{
		// Extracting Offset To Add
		uint8_t offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Vertex Shader Path
		data += sizeof(offsetToAdd);
		const char * const relativeVertexShaderPath = reinterpret_cast<const char * const>(data);

		// Extracting Offset To Add
		data += offsetToAdd;
		offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Fragment Shader Path
		data += sizeof(offsetToAdd);
		const char * const relativeFragmentShaderPath = reinterpret_cast<const char * const>(data);

		// Loading Shaders
		if (!o_effect.LoadShaders(relativeVertexShaderPath, relativeFragmentShaderPath))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			Logging::OutputError("Failed to load shaders in: %s", i_relativePath);
			goto OnExit;
		}
	}

	std::regex_match(i_relativePath, match, pattern_match);
	fileName = std::regex_replace(match.str(2), pattern_match1, pattern_replace);
	o_effect.effectUUID = StringHandler::HashedString(fileName.c_str()).GetHash();

OnExit:
	binaryEffect.Free();

	return !wereThereErrors;
}

uint32_t eae6320::Graphics::Effect::GetEffectUUID() const
{
	return effectUUID;
}