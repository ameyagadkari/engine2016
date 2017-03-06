#include "Physics.h"
#include "../Graphics/MeshData.h"
#include "../Platform/Platform.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"


namespace
{
	const float s_epsilon = 1.0e-9f;
	char const * const collsionDataPath = "data/meshes/plane.binmesh";
	eae6320::Platform::sDataFromFile binaryMesh;
	bool IntersectSegmentTriangle(eae6320::Math::cVector p, eae6320::Math::cVector q, eae6320::Math::cVector a, eae6320::Math::cVector b, eae6320::Math::cVector c, eae6320::Math::cVector n1, eae6320::Math::cVector n2, eae6320::Math::cVector n3, float &u, float &v, float &w, float &t);

	
	//std::vector<bool> check;
	//std::vector<size_t>indexes;
}

namespace eae6320
{
	namespace Physics
	{
		bool isPlayerOnGround = false;
		Graphics::MeshData* collisionData = nullptr;
	}
}
bool eae6320::Physics::Initialize()
{
	bool wereThereErrors = false;

	// Load the binary mesh file
	{
		std::string errorMessage;
		if (!LoadBinaryFile(collsionDataPath, binaryMesh, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			Logging::OutputError("Failed to load the binary mesh file \"%s\": %s", collsionDataPath, errorMessage.c_str());
			goto OnExit;
		}
	}

	// Casting data to uint8_t* for pointer arithematic
	uint8_t* data = reinterpret_cast<uint8_t*>(binaryMesh.data);

	collisionData = new Graphics::MeshData();
	// Extracting Binary Data
	{
		// Extracting Type Of IndexData		
		collisionData->typeOfIndexData = *reinterpret_cast<uint32_t*>(data);

		// Extracting Number Of Vertices
		data += sizeof(uint32_t);
		collisionData->numberOfVertices = *reinterpret_cast<uint32_t*>(data);

		// Extracting Number Of Indices
		data += sizeof(uint32_t);
		collisionData->numberOfIndices = *reinterpret_cast<uint32_t*>(data);

		// Extracting Vertex Array
		data += sizeof(uint32_t);
		collisionData->vertexData = reinterpret_cast<Graphics::MeshData::Vertex*>(data);

		// Extracting Index Array
		data += collisionData->numberOfVertices * sizeof(Graphics::MeshData::Vertex);
		collisionData->indexData = data;
	}

OnExit:
	return !wereThereErrors;
}

void eae6320::Physics::CheckCollision(const Math::cVector i_newPosition, const Camera::LocalAxes i_localAxes, Math::cVector& o_localOffset)
{
	float u, v, w, t;
	// Is grounded check
	{
		const Math::cVector q = i_newPosition - (i_localAxes.up*100.0f);
		for (size_t i = 0; i < collisionData->numberOfIndices; i += 3)
		{
			const Math::cVector p1(collisionData->vertexData[i].x, collisionData->vertexData[i].y, collisionData->vertexData[i].z);
			const Math::cVector p2(collisionData->vertexData[i + 1].x, collisionData->vertexData[i + 1].y, collisionData->vertexData[i + 1].z);
			const Math::cVector p3(collisionData->vertexData[i + 2].x, collisionData->vertexData[i + 2].y, collisionData->vertexData[i + 2].z);

			const Math::cVector n1(collisionData->vertexData[i].nx, collisionData->vertexData[i].ny, collisionData->vertexData[i].nz);
			const Math::cVector n2(collisionData->vertexData[i + 1].nx, collisionData->vertexData[i + 1].ny, collisionData->vertexData[i + 1].nz);
			const Math::cVector n3(collisionData->vertexData[i + 2].nx, collisionData->vertexData[i + 2].ny, collisionData->vertexData[i + 2].nz);

			isPlayerOnGround = IntersectSegmentTriangle(i_newPosition, q, p1, p2, p3, n1, n2, n3, u, v, w, t);
			//check.push_back(isPlayerOnGround);
			if (isPlayerOnGround)
			{
				break;
			}
		}
		/*size_t length = check.size();
		for (size_t i = 0; i < length; i++)
		{
			if (check[i])indexes.push_back(i);
		}*/
		//!isPlayerOnGround) ? o_localOffset -= o_localOffset.up : o_localOffset.y = 0.0f;
		if (!isPlayerOnGround)
		{
			o_localOffset -= o_localOffset.up;
		}
		else
		{
			o_localOffset.y = 0.0f;
		}
		//check.clear();
		//indexes.clear();
	}
}

bool eae6320::Physics::CleanUp()
{
	if (collisionData)
	{
		binaryMesh.Free();
		collisionData->vertexData = nullptr;
		collisionData->indexData = nullptr;
		delete collisionData;
		collisionData = nullptr;
	}
	return true;
}

namespace
{
	bool IntersectSegmentTriangle(eae6320::Math::cVector p, eae6320::Math::cVector q, eae6320::Math::cVector a, eae6320::Math::cVector b, eae6320::Math::cVector c, eae6320::Math::cVector n1, eae6320::Math::cVector n2, eae6320::Math::cVector n3, float &u, float &v, float &w, float &t)
	{
		eae6320::Math::cVector ab = b - a;
		eae6320::Math::cVector ac = c - a;
		eae6320::Math::cVector qp = p - q;

		// Compute triangle normal. Can be precalculated or cached if
		// intersecting multiple segments against the same triangle
		/*eae6320::Math::cVector n = Cross(ab, ac);
		//if (n.GetLength() > 1.0e-9f)
		if (n != eae6320::Math::cVector::zero)
		{
			n = n.CreateNormalized();
		}*/

		eae6320::Math::cVector n = n1;
		/*if (Dot(n, vertexNormal) < s_epsilon)
		{
			n = -n;
		}*/

		// Compute denominator d. If d <= 0, segment is parallel to or points
		// away from triangle, so exit early
		float d = Dot(qp, n);
		if (d <= s_epsilon) return false;

		// Compute intersection t value of pq with plane of triangle. A ray
		// intersects if 0 <= t. Segment intersects if 0 <= t <= 1. Delay
		// dividing by d until intersection has been found to pierce triangle
		eae6320::Math::cVector ap = p - a;
		t = Dot(ap, n);
		if (t < s_epsilon) return false;
		if (t > d) return false; // For segment; exclude this code line for a ray test

		// Compute barycentric coordinate components and test if within bounds
		eae6320::Math::cVector e = Cross(qp, ap);
		v = Dot(ac, e);
		if (v < s_epsilon || v > d) return false;
		w = -Dot(ab, e);
		if (w < s_epsilon || v + w > d) return false;

		// Segment/ray intersects triangle. Perform delayed division and
		// compute the last barycentric coordinate component
		float ood = 1.0f / d;
		t *= ood;
		v *= ood;
		w *= ood;
		u = 1.0f - v - w;
		return true;
	}
}
