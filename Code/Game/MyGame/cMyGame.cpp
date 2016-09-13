// Header Files
//=============

#include "cMyGame.h"
#include "../../Engine/Asserts/Asserts.h"
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
	mesh = new Graphics:: Mesh();
	bool wereThereErrors = false;
	if (mesh && !mesh->Initialize())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		//return false;
	}
	return !wereThereErrors;
}

void eae6320::cMyGame::DrawMesh()
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
	}
	return !wereThereErrors;
}
