#ifndef EAE6320_CCAMERA_H
#define EAE6320_CCAMERA_H

#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/Functions.h"
#include "LocalAxes.h"
#include <vector>

namespace eae6320
{
	namespace Camera
	{
		class cCamera
		{
		public:
			LocalAxes localAxes;
			inline virtual ~cCamera();

			void UpdateCurrentCameraPosition();
			void UpdateCurrentCameraOrientation();

			static cCamera* Initialize(const bool isFlyCam, const bool isStatic, const Math::cVector eularAngles, const Math::cVector position = Math::cVector::zero, const float fieldOfView = Math::ConvertDegreesToRadians(45.0f), const float nearPlaneDistance = 0.1f, const  float farPlaneDistance = 10000.0f);
			static bool CleanUp();

			static void UpdateMaxCameras();
			static void ChangeCurrentCamera();
			static void PushBackToVector(cCamera *camera);

#pragma region Gets
			Math::cVector GetPosition()const;
			Math::cVector GetOrientationEular()const;
			Math::cQuaternion GetOrientation()const;
			float GetFieldOfView()const;
			float GetNearPlaneDistance()const;
			float GetFarPlaneDistance()const;
			float GetAspectRatio() const;
			static cCamera* GetCurrentCamera();
#pragma endregion

#pragma region Sets
			void SetPosition(const Math::cVector position);
			void SetOrientationEular(const Math::cVector eularAngles);
			void SetFieldOfView(const float fieldOfView);
			void SetNearPlaneDistance(const float nearPlaneDistance);
			void SetFarPlaneDistance(const float farPlaneDistance);
#pragma endregion


		private:
			inline cCamera(const bool isFlyCam, const bool isStatic, const Math::cVector eularAngles, const Math::cVector position, const Math::cQuaternion orientation, const float fieldOfView, const float nearPlaneDistance, const float farPlaneDistance);

			Math::cVector position;
			Math::cQuaternion orientation;
			Math::cVector eularAngles;
			float fieldOfView;
			float nearPlaneDistance;
			float farPlaneDistance;
			float aspectRatio;
			bool isStatic;
			bool isFlyCam;

			static std::vector<cCamera*> sCameras;
			static cCamera* sCurrentCamera;
			static size_t sCurrentCameraNumber;
			static size_t sMaxCameraNumber;

			void UpdateCameraAxes();
		};
	}
}
#endif // !EAE6320_CCAMERA_H
