#ifndef EAE6320_CAMERA_H
#define EAE6320_CAMERA_H

#include <vector>
#include "../../Game/Gameplay/Transform.h"

namespace eae6320
{
	namespace Gameplay
	{
		class cbController;
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
			Debug::Shapes::DebugObject* m_forward;
			Debug::Shapes::DebugObject* m_sphere;

			~Camera();
			explicit Camera( Gameplay::cbController const * const i_controller = nullptr, const Math::cVector i_position = Math::cVector::zero, const Math::cVector i_orientationEular = Math::cVector::zero, const float i_fieldOfView = Math::ConvertDegreesToRadians(45.0f), const float i_nearPlaneDistance = 0.1f, const  float i_farPlaneDistance = 10000.0f);

			void UpdateCameraPosition();
			void UpdateCameraOrientation();

			static bool CleanUp();

			static void UpdateMaxCameras(std::vector<Debug::Shapes::DebugObject*>& debugObjects);
			static void ChangeCurrentCamera();
			static void PushBackToVector(Camera *i_camera);
			static Camera* GetCurrentCamera();

#pragma region Gets
			Gameplay::Transform GetTransform()const;
			float GetFieldOfView()const;
			float GetNearPlaneDistance()const;
			float GetFarPlaneDistance()const;
			float GetAspectRatio() const;
#pragma endregion

#pragma region Sets
			void SetFieldOfView(const float i_fieldOfView);
			void SetNearPlaneDistance(const float i_nearPlaneDistance);
			void SetFarPlaneDistance(const float i_farPlaneDistance);
#pragma endregion


		private:
			Gameplay::Transform m_transform;
		public:
			Gameplay::cbController* m_controller;
		private:
			float m_fieldOfView;
			float m_nearPlaneDistance;
			float m_farPlaneDistance;
			float m_aspectRatio;
			
			static std::vector<Camera*> s_cameras;
			static Camera* s_currentCamera;
			static size_t s_currentCameraNumber;
			static size_t s_maxCameraNumber;
		};
	}
}
#endif // !EAE6320_CAMERA_H
