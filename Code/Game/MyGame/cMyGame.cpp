// Header Files
//=============

#include "cMyGame.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cMyGame::~cMyGame()
{

}

// Inherited Implementation
//=========================

// Initialization / Clean Up
//--------------------------

bool eae6320::cMyGame::Initialize()
{
	const char*const path = "data/Meshes/Square.txtmesh";
	mesh = Graphics::Mesh::LoadMesh(path);
	bool wereThereErrors = false;
	if (!mesh)
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Mesh was not created");
	}
	return !wereThereErrors;
}

void eae6320::cMyGame::SubmitMesh()
{
	eae6320::Graphics::SetMesh(mesh);
}

bool eae6320::cMyGame::CleanUp()
{
	bool wereThereErrors = false;
	if (mesh && !mesh->CleanUp())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Mesh cleanup failed");
	}
	if (mesh)
	{
		delete mesh;
	}
	return !wereThereErrors;
}
