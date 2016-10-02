// Header Files
//=============


#include <vector>
#include "../../Engine/Gameplay/GameObject.h"
#include "cMyGame.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/UserInput/UserInput.h"

#define NUMBER_OF_COORDINATES 2
// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cMyGame::~cMyGame()
{

}
// Helper Function Declarations
//=============================

namespace
{
	void GenerateRelativePaths();
	std::vector<eae6320::Gameplay::GameObject*> gameObjects;
	std::vector<std::string> relativePaths;
	size_t numberOfMeshes;
	float offset[2] = { 0.0f,0.0f };
	float offset1[2] = { 0.0f,0.0f };
}
// Inherited Implementation
//=========================

// Initialization / Clean Up
//--------------------------

bool eae6320::cMyGame::Initialize()
{
	GenerateRelativePaths();
	for (size_t i = 0; i < numberOfMeshes; i++)
	{
		gameObjects.push_back(Gameplay::GameObject::Initilaize(relativePaths[i].c_str()));
	}
	bool wereThereErrors = false;
	//if (!mesh1)
	//{
	//	wereThereErrors = true;
	//	EAE6320_ASSERT(false);
	//	Logging::OutputError("Mesh was not created %s", path_mesh1);
	//}
	//if (!mesh2)
	//{
	//	wereThereErrors = true;
	//	EAE6320_ASSERT(false);
	//	Logging::OutputError("Mesh was not created %s", path_mesh2);
	//}
	return !wereThereErrors;
}

void eae6320::cMyGame::UpdatePlayerPosition()
{
	if (UserInput::IsKeyPressed(VK_UP) || UserInput::IsKeyPressed(0x57))//W
		offset[1] += 1.0f;
	if (UserInput::IsKeyPressed(VK_DOWN) || UserInput::IsKeyPressed(0x53))//S
		offset[1] -= 1.0f;
	if (UserInput::IsKeyPressed(VK_LEFT) || UserInput::IsKeyPressed(0x41))//A
		offset[0] -= 1.0f;
	if (UserInput::IsKeyPressed(VK_RIGHT) || UserInput::IsKeyPressed(0x44))//D
		offset[0] += 1.0f;

	const float speed_unitsPerSecond = 0.5f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	offset[0] *= offsetModifier;
	offset[1] *= offsetModifier;

	gameObjects[0]->SetNewOffset(offset1[0], offset1[1]);
	gameObjects[1]->SetNewOffset(offset[0], offset[1]);	
}

void eae6320::cMyGame::SubmitMesh()
{
	Graphics::SetGameObject(gameObjects[0], 0.5f, 0.5f);
	Graphics::SetGameObject(gameObjects[1], 0.0f, 0.0f);
}

bool eae6320::cMyGame::CleanUp()
{
	for (size_t i = 0; i < numberOfMeshes; i++)
	{
		delete gameObjects[i];
	}
	gameObjects._Pop_back_n(numberOfMeshes);
	bool wereThereErrors = false;
	/*if (mesh1 && !mesh1->CleanUp())
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
	}*/
	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	void GenerateRelativePaths()
	{
		std::string prefix = "data/Meshes/";
		WIN32_FIND_DATA search_data;
		memset(&search_data, 0, sizeof(WIN32_FIND_DATA));
		HANDLE handle = FindFirstFile((prefix + "*").c_str(), &search_data);
		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!strcmp(search_data.cFileName, "."))continue;
				else if (!strcmp(search_data.cFileName, ".."))continue;
				else
					relativePaths.push_back((prefix + search_data.cFileName).c_str());
			} while (FindNextFile(handle, &search_data));
		}
		FindClose(handle);
		numberOfMeshes = relativePaths.size();
	}
}