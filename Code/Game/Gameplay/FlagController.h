#ifndef EAE6320_FLAG_CONTROLLER_H
#define EAE6320_FLAG_CONTROLLER_H

#include "cbController.h"
#include <cstdint>

//namespace eae6320
//{
//	namespace Physics
//	{
//		struct Triangle;
//	}
//}


namespace eae6320
{
	namespace Gameplay
	{

		class FlagController final : cbController
		{
		public:
			static const uint32_t classUUID;
			static FlagController* Initialize()
			{
				return new FlagController();
			}
			void SetPlayerTransform(Transform const * const  i_playerTransform)
			{
				m_playerTransform = i_playerTransform;
			}
			/*void SetFlagCollisionData(const size_t i_numberOfFlagTriangles, Physics::Triangle const * const i_flagTriangles)
			{
				m_numberOfFlagTriangles = i_numberOfFlagTriangles;
				m_flagTriangles = i_flagTriangles;
			}*/
		private:
			explicit FlagController()
				:
				//m_numberOfFlagTriangles(0),
				//m_flagTriangles(nullptr),
				m_playerTransform(nullptr),
				m_flagNotCarrying(true)
			{}
			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform) override;
			//size_t m_numberOfFlagTriangles;
			//Physics::Triangle const * m_flagTriangles;
			Transform const * m_playerTransform;
			bool m_flagNotCarrying;
		};
	}
}

#endif // !EAE6320_FLAG_CONTROLLER_H
