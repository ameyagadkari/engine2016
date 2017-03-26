#ifndef EAE6320_CAMERA_H
#define EAE6320_CAMERA_H

#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/Functions.h"
#include "LocalAxes.h"
#include <vector>

namespace eae6320
{
	namespace Camera
	{
		class ICameraController;
	}
}

namespace eae6320
{
	namespace Debug
	{
		namespace Shapes
		{
			class DebugObject;
		}
	}
}

namespace eae6320
{
	namespace Camera
	{
		class Camera
		{
		public:
			Debug::Shapes::DebugObject* m_sphere;
			~Camera();
			void UpdateCameraPosition();
			void UpdateCameraOrientation();
			static Camera* Initialize(const ICameraController* controller = nullptr, const Math::cVector position = Math::cVector::zero, const Math::cVector eularAngles = Math::cVector::zero, const float fieldOfView = Math::ConvertDegreesToRadians(45.0f), const float nearPlaneDistance = 0.1f, const  float farPlaneDistance = 10000.0f);
			static bool CleanUp();

			static void UpdateMaxCameras();
			static void ChangeCurrentCamera();
			static void PushBackToVector(Camera *camera);

#pragma region Gets
			Math::cVector GetPosition()const;
			Math::cVector GetOrientationEular()const;
			Math::cQuaternion GetOrientation()const;
			float GetFieldOfView()const;
			float GetNearPlaneDistance()const;
			float GetFarPlaneDistance()const;
			float GetAspectRatio() const;
			static Camera* GetCurrentCamera();
			//LocalAxes GetLocalAxes()const;
#pragma endregion

#pragma region Sets
			void SetPosition(const Math::cVector position);
			void SetOrientationEular(const Math::cVector eularAngles);
			void SetFieldOfView(const float fieldOfView);
			void SetNearPlaneDistance(const float nearPlaneDistance);
			void SetFarPlaneDistance(const float farPlaneDistance);
#pragma endregion


		private:
			Camera(const ICameraController* controller, const Math::cVector position, const Math::cVector eularAngles, const Math::cQuaternion orientation, const float fieldOfView, const float nearPlaneDistance, const float farPlaneDistance);

			LocalAxes localAxes;
			Math::cQuaternion orientation;
			Math::cVector position;
			Math::cVector eularAngles;
			const ICameraController* controller;
			float fieldOfView;
			float nearPlaneDistance;
			float farPlaneDistance;
			float aspectRatio;
			
			static std::vector<Camera*> sCameras;
			static Camera* sCurrentCamera;
			static size_t sCurrentCameraNumber;
			static size_t sMaxCameraNumber;

			void UpdateCameraAxes();
		};
	}
}
#endif // !EAE6320_CAMERA_H
