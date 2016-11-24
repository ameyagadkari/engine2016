#include "Material.h"
#include "ConstantBuffer.h"
#include "ConstantBufferData.h"
#include "Effect.h"
#include "cTexture.h"
#include "../Logging/Logging.h"
#include "../Asserts/Asserts.h"
#include "../Platform/Platform.h"
#include "../StringHandler/HashedString.h"

#include <regex>

uint32_t eae6320::Graphics::Material::s_previousEffectUUID = 0;

namespace
{
	std::cmatch match;
	const std::regex pattern_match("(data/materials/)(.*)");
	const std::regex pattern_match1("(\\.)([[:alpha:]]+)");
	const std::string pattern_replace("");

	eae6320::Graphics::ConstantBufferData::sMaterial materialBufferData;
}

eae6320::Graphics::Material::Material() :
	m_materialUUID(0),
	m_materialBuffer(new ConstantBuffer()),
	m_effect(new Effect())
	//m_texture(new cTexture())
{}

eae6320::Graphics::Material::~Material()
{
	if (!CleanUpMaterial())
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("Material cleanup failed");
	}
}

bool eae6320::Graphics::Material::LoadMaterial(const char * const i_relativePath, Material & o_material)
{
	bool wereThereErrors = false;
	std::string fileName;
	// Load the binary effect file
	eae6320::Platform::sDataFromFile binaryMaterial;
	{
		std::string errorMessage;
		if (!eae6320::Platform::LoadBinaryFile(i_relativePath, binaryMaterial, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the binary effect file \"%s\": %s", i_relativePath, errorMessage.c_str());
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

		// Extracting Offset To Add
		data += offsetToAdd;
		offsetToAdd = *reinterpret_cast<uint8_t*>(data);

		// Extracting Texture Path
		data += sizeof(offsetToAdd);
		const char * const relativeTexturePath = reinterpret_cast<const char * const>(data);

		//Initilaizing Constant Buffer
		o_material.m_materialBuffer->InitializeConstantBuffer(ConstantBufferType::MATERIAL, sizeof(materialBufferData), &materialBufferData);

		// Loading Effect
		if (!Graphics::Effect::LoadEffect(relativeEffectPath, *o_material.m_effect))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			Logging::OutputError("Failed to load effect in: %s", i_relativePath);
			goto OnExit;
		}

		// Loading Texture
		/*if (!Graphics::cTexture::Load(relativeTexturePath, *o_material.m_effect))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			Logging::OutputError("Failed to load effect in: %s", i_relativePath);
			goto OnExit;
		}*/
	}

	std::regex_match(i_relativePath, match, pattern_match);
	fileName = std::regex_replace(match.str(2), pattern_match1, pattern_replace);
	o_material.m_materialUUID = StringHandler::HashedString(fileName.c_str()).GetHash();

OnExit:
	binaryMaterial.Free();

	return !wereThereErrors;
}

uint32_t eae6320::Graphics::Material::GetMaterialUUID() const
{
	return m_materialUUID;
}

eae6320::Graphics::Effect * eae6320::Graphics::Material::GetEffect() const
{
	return m_effect;
}

bool eae6320::Graphics::Material::CleanUpMaterial()
{
	bool wereThereErrors = false;
	if (m_materialBuffer)
	{
		delete m_materialBuffer;
	}
	if (m_effect)
	{
		delete m_effect;
	}
	return !wereThereErrors;
}

void eae6320::Graphics::Material::BindMaterial()const
{
	m_materialBuffer->BindingConstantBuffer(BindMode::PS_ONLY);
	uint32_t currentEffectUUID = m_effect->GetEffectUUID();
	if (currentEffectUUID != s_previousEffectUUID)
	{
		s_previousEffectUUID = currentEffectUUID;
		m_effect->BindEffect();
	}
}