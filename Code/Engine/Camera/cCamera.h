#ifndef EAE6320_CCAMERA_DATA_H
#define EAE6320_CCAMERA_DATA_H

#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/Functions.h"
#include <vector>

namespace eae6320
{
	namespace Camera
	{
		class cCamera
		{
		public:					
			inline virtual ~cCamera();

			void UpdateCurrentCameraPosition();
			void UpdateCurrentCameraOrientation();

			static cCamera* Initialize(bool isStatic, Math::cVector position = Math::cVector(0.0f, 0.0f, 10.0f), float fieldOfView = Math::ConvertDegreesToRadians(60.0f), float nearPlaneDistance = 0.1f, float farPlaneDistance = 100.0f);
			static bool CleanUp();

			static void UpdateMaxCameras();
			static void ChangeCurrentCamera();
			static void PushBackToVector(cCamera *camera);

#pragma region Gets
			Math::cVector GetPosition()const;
			Math::cQuaternion GetOrientation()const;
			float GetFieldOfView()const;
			float GetNearPlaneDistance()const;
			float GetFarPlaneDistance()const;
			static float GetAspectRatio();
			static cCamera* GetCurrentCamera();
#pragma endregion

#pragma region Sets
			void SetPosition(const Math::cVector position);
			void SetOrientation(const Math::cQuaternion orientation);
			void SetFieldOfView(const float fieldOfView);
			void SetNearPlaneDistance(const float nearPlaneDistance);
			void SetFarPlaneDistance(const float farPlaneDistance);
#pragma endregion


		private:
			inline cCamera(bool isStatic, Math::cVector position, float fieldOfView, float nearPlaneDistance, float farPlaneDistance);

			Math::cVector position;
			Math::cQuaternion orientation;
			float fieldOfView;
			float nearPlaneDistance;
			float farPlaneDistance;
			bool isStatic;

			float eularOrientationOffsetsDegrees[3];
						
			static std::vector<eae6320::Camera::cCamera*> sCameras;
			static cCamera* sCurrentCamera;
			static size_t sCurrentCameraNumber;
			static size_t sMaxCameraNumber;
			static float sAspectRatio;
		};
	}
}
#endif // !EAE6320_CCAMERA_DATA_H
