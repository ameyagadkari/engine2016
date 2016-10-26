/*
	This class builds shaders
*/

#ifndef EAE6320_CSHADERBUILDER_H
#define EAE6320_CSHADERBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"

#include "../../Engine/Graphics/Configuration.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		namespace ShaderTypes
		{
			enum struct eShaderType { Unknown, Vertex, Fragment };
			/*const char* ToString(const eShaderType i_eShaderType)
			{
				switch (i_eShaderType)
				{
				case eShaderType::Unknown:
					return "Unknown";
					break;
				case eShaderType::Vertex:
					return "Vertex";
					break;
				case eShaderType::Fragment:
					return "Fragment";
					break;
				default:
					break;
				}
			}*/
		}
	}
}

namespace eae6320
{
	namespace AssetBuild
	{
		class cShaderBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual bool Build( const std::vector<std::string>& i_arguments );

			// Implementation
			//===============

		private:

			// Build
			//------

			bool Build( const Graphics::ShaderTypes::eShaderType i_shaderType, const std::vector<std::string>& i_arguments );
		};
	}
}

#endif	// EAE6320_CSHADERBUILDER_H
