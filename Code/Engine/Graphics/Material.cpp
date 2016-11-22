#include "Material.h"
#include "ConstantBuffer.h"
#include "ConstantBufferData.h"
#include "Effect.h"
#include "../Logging/Logging.h"
#include "../Asserts/Asserts.h"
#include "../Platform/Platform.h"
#include "../StringHandler/HashedString.h"

#include <regex>

uint32_t eae6320::Graphics::Material::previousEffectUUID = 0;

namespace
{
	std::cmatch match;
	const std::regex pattern_match("(data/materials/)(.*)");
	const std::regex pattern_match1("(\\.)([[:alpha:]]+)");
	const std::string pattern_replace("");

	eae6320::Graphics::ConstantBufferData::sMaterial materialBufferData;
}

eae6320::Graphics::Material::Material()
{
	materialBuffer = new ConstantBuffer();
	effect = new Effect();
}

eae6320::Graphics::Material::~Material()
{
	if (materialBuffer)
	{
		delete materialBuffer;
	}
	if (effect)
	{
		delete effect;
	}
}

bool eae6320::Graphics::Material::LoadMaterial(const char * const relativePath, Material & o_material)
{
	bool wereThereErrors = false;
	std::string fileName;
	// Load the binary effect file
	eae6320::Platform::sDataFromFile binaryMaterial;
	{
		std::string errorMessage;
		if (!eae6320::Platform::LoadBinaryFile(relativePath, binaryMaterial, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the binary effect file \"%s\": %s", relativePath, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Casting data to uint8_t* for pointer arithematic
	uint8_t* data = reinterpret_cast<uint8_t*>(binaryMaterial.data);

	// Extracting Binary Data
	{
		// Extracting constant buffer data
		materialBufferData = *reinterpret_cast<ConstantBufferData::sMaterial*>(data);

		// Extracting Offset To Add
		data += sizeof(ConstantBufferData::sMaterial);
		uint8_t offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Effect Path
		data += sizeof(offsetToAdd);
		const char * const relativeEffectPath = reinterpret_cast<const char * const>(data);

		//Initilaizing Constant Buffer
		o_material.materialBuffer->InitializeConstantBuffer(ConstantBufferType::MATERIAL, sizeof(materialBufferData), &materialBufferData);

		// Loading Effect
		if (!Graphics::Effect::LoadEffect(relativeEffectPath, *o_material.effect))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			Logging::OutputError("Failed to load effect in: %s", relativePath);
			goto OnExit;
		}
	}

	std::regex_match(relativePath, match, pattern_match);
	fileName = std::regex_replace(match.str(2), pattern_match1, pattern_replace);
	o_material.materialUUID = StringHandler::HashedString(fileName.c_str()).GetHash();

OnExit:
	binaryMaterial.Free();

	return !wereThereErrors;
}

uint32_t eae6320::Graphics::Material::GetMaterialUUID() const
{
	return materialUUID;
}

eae6320::Graphics::Effect * eae6320::Graphics::Material::GetEffect() const
{
	return effect;
}

bool eae6320::Graphics::Material::CleanUpMaterial()
{
	bool wereThereErrors = false;
	if (!materialBuffer->CleanUpConstantBuffer())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Material constant buffer cleanup failed");
	}

	if (!effect->CleanUpEffect())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Effect cleanup failed");
	}
	return !wereThereErrors;
}

void eae6320::Graphics::Material::BindMaterial()
{
	materialBuffer->BindingConstantBuffer(BindMode::PS_ONLY);
	uint32_t currentEffectUUID = effect->GetEffectUUID();
	if (currentEffectUUID != previousEffectUUID)
	{
		previousEffectUUID = currentEffectUUID;
		effect->BindEffect();
	}
}