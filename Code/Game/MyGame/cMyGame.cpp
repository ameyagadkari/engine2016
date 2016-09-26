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
	const char*const path_mesh1 = "data/Meshes/Square.txtmesh";
	const char*const path_mesh2 = "data/Meshes/Pentagon.txtmesh";
	mesh1 = Graphics::Mesh::LoadMesh(path_mesh1);
	mesh2 = Graphics::Mesh::LoadMesh(path_mesh2);
	bool wereThereErrors = false;
	if (!mesh1)
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Mesh was not created %s", path_mesh1);
	}
	if (!mesh2)
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Mesh was not created %s", path_mesh2);
	}
	return !wereThereErrors;
}

void eae6320::cMyGame::SubmitMesh()
{
	eae6320::Graphics::SetMesh(mesh1);
	eae6320::Graphics::SetMesh(mesh2);
}

bool eae6320::cMyGame::CleanUp()
{
	bool wereThereErrors = false;
	if (mesh1 && !mesh1->CleanUp())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Mesh1 cleanup failed");
	}
	if (mesh2 && !mesh2->CleanUp())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Mesh2 cleanup failed");
	}
	if (mesh1)
	{
		delete mesh1;
	}
	if (mesh2)
	{
		delete mesh2;
	}
	return !wereThereErrors;
}
