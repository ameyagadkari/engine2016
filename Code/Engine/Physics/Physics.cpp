#include "Physics.h"
#include "../Graphics/MeshData.h"
#include "../Platform/Platform.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

namespace
{
	char const * const collsionDataPath = "data/meshes/collisiondata.binmesh";
	eae6320::Platform::sDataFromFile binaryMesh;
	bool IntersectSegmentTriangle(eae6320::Math::cVector p, eae6320::Math::cVector q, eae6320::Math::cVector a, eae6320::Math::cVector b, eae6320::Math::cVector c, float &u, float &v, float &w, float &t);
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
		const Math::cVector q = i_newPosition - Math::cVector::up*100.0f;
		uint32_t j1,j2,j3;
		for (size_t i = 0; i < collisionData->numberOfIndices; i += 3)
		{
#if defined( EAE6320_PLATFORM_D3D )
			j1 = reinterpret_cast<uint32_t*>(collisionData->indexData)[collisionData->numberOfIndices - 1 - i];
			j2 = reinterpret_cast<uint32_t*>(collisionData->indexData)[collisionData->numberOfIndices - 1 - (i + 1)];
			j3 = reinterpret_cast<uint32_t*>(collisionData->indexData)[collisionData->numberOfIndices - 1 - (i + 2)];
#elif defined( EAE6320_PLATFORM_GL )
			j1 = reinterpret_cast<uint32_t*>(collisionData->indexData)[i];
			j2 = reinterpret_cast<uint32_t*>(collisionData->indexData)[i + 1];
			j3 = reinterpret_cast<uint32_t*>(collisionData->indexData)[i + 2];
#endif
			const Math::cVector p1(collisionData->vertexData[j1].x, collisionData->vertexData[j1].y, collisionData->vertexData[j1].z);
			const Math::cVector p2(collisionData->vertexData[j2].x, collisionData->vertexData[j2].y, collisionData->vertexData[j2].z);
			const Math::cVector p3(collisionData->vertexData[j3].x, collisionData->vertexData[j3].y, collisionData->vertexData[j3].z);

			isPlayerOnGround = IntersectSegmentTriangle(i_newPosition, q, p1, p2, p3, u, v, w, t);
			if (isPlayerOnGround) break;
		}
		!isPlayerOnGround ? o_localOffset -= Math::cVector::up : o_localOffset.y = 0.0f;
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
	bool IntersectSegmentTriangle(eae6320::Math::cVector p, eae6320::Math::cVector q, eae6320::Math::cVector a, eae6320::Math::cVector b, eae6320::Math::cVector c, float &u, float &v, float &w, float &t)
	{
		eae6320::Math::cVector ab = b - a;
		eae6320::Math::cVector ac = c - a;
		eae6320::Math::cVector qp = p - q;

		// Compute triangle normal. Can be precalculated or cached if
		// intersecting multiple segments against the same triangle
		eae6320::Math::cVector n = Cross(ab, ac);

		// Compute denominator d. If d <= 0, segment is parallel to or points
		// away from triangle, so exit early
		float d = Dot(qp, n);
		if (d <= 0.0f) return false;

		// Compute intersection t value of pq with plane of triangle. A ray
		// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
		// dividing by d until intersection has been found to pierce triangle
		eae6320::Math::cVector ap = p - a;
		t = Dot(ap, n);
		if (t < 0.0f) return false;
		if (t > d) return false; // For segment; exclude this code line for a ray test

		// Compute barycentric coordinate components and test if within bounds
		eae6320::Math::cVector e = Cross(qp, ap);
		v = Dot(ac, e);
		if (v < 0.0f || v > d) return false;
		w = -Dot(ab, e);
		if (w < 0.0f || v + w > d) return false;

		// Segment/ray intersects triangle. Perform delayed division and compute the last barycentric coordinate component
		float ood = 1.0f / d;
		t *= ood;
		v *= ood;
		w *= ood;
		u = 1.0f - v - w;
		return true;
	}
}
