#ifndef EAE6320_CCAMERA_DATA_H
#define EAE6320_CCAMERA_DATA_H

#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"

namespace eae6320
{
	namespace Camera
	{
		class cCamera
		{
		public:
			inline virtual ~cCamera();

			static cCamera* Initialize();

#pragma region Gets
			Math::cVector GetPosition()const;
			Math::cQuaternion GetOrientation()const;
			float GetFieldOfView()const;
			float GetNearPlaneDistance()const;
			float GetFarPlaneDistance()const;
			float GetAspectRatio()const;
#pragma endregion

#pragma region Sets
			void SetPosition(const Math::cVector position);
			void SetOrientation(const Math::cQuaternion orientation);
			void SetFieldOfView(const float fieldOfView);
			void SetNearPlaneDistance(const float nearPlaneDistance);
			void SetFarPlaneDistance(const float farPlaneDistance);
#pragma endregion


		private:
			inline cCamera();

			Math::cVector position;
			Math::cQuaternion orientation;
			float fieldOfView;
			float nearPlaneDistance;
			float farPlaneDistance;
			float aspectRatio;
		};
	}
}
#endif // !EAE6320_CCAMERA_DATA_H
