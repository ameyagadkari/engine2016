#ifndef EAE6320_TRANSFORM_H
#define EAE6320_TRANSFORM_H
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"
#include "LocalAxes.h"
#include "../../Engine/Math/Functions.h"

namespace
{
	const float MaxRange = 360.0f;
	const float MaxRange2 = 360.0f;
}

namespace eae6320
{
	namespace Gameplay
	{
		struct Transform
		{
			LocalAxes m_localAxes;
			Math::cVector m_position;
			void SetOrientationEular(const Math::cVector i_orientationEular)
			{
				if (i_orientationEular.x > MaxRange)m_orientationEular.x = i_orientationEular.x - MaxRange2;
				else if (i_orientationEular.x < MaxRange)m_orientationEular.x = i_orientationEular.x + MaxRange2;
				else m_orientationEular.x = i_orientationEular.x;

				if (i_orientationEular.y > MaxRange)m_orientationEular.y = i_orientationEular.y - MaxRange2;
				else if (i_orientationEular.y < MaxRange)m_orientationEular.y = i_orientationEular.y + MaxRange2;
				else m_orientationEular.y = i_orientationEular.y;


				if (i_orientationEular.z > MaxRange)m_orientationEular.z = i_orientationEular.z - MaxRange2;
				else if (i_orientationEular.z < MaxRange)m_orientationEular.z = i_orientationEular.z + MaxRange2;
				else m_orientationEular.z = i_orientationEular.z;
			}

			void SetOrientationEular(const float i_x, const float i_y, const float i_z)
			{
				if (i_x > MaxRange)m_orientationEular.x = i_x - MaxRange2;
				else if (i_x < MaxRange)m_orientationEular.x = i_x + MaxRange2;
				else m_orientationEular.x = i_x;

				if (i_y > MaxRange)m_orientationEular.y = i_y - MaxRange2;
				else if (i_y < MaxRange)m_orientationEular.y = i_y + MaxRange2;
				else m_orientationEular.y = i_y;


				if (i_z > MaxRange)m_orientationEular.z = i_z - MaxRange2;
				else if (i_z < MaxRange)m_orientationEular.z = i_z + MaxRange2;
				else m_orientationEular.z = i_z;
			}
			Math::cVector GetOrientationEular()const
			{
				return m_orientationEular;
			}
			Math::cQuaternion GetOrientationQuarternion()const
			{
				return m_orientationQuaternion;
			}
			void UpdateLocalAxes(bool UpdateForward = true, Math::cVector i_forward = Math::cVector::zero)
			{
				Math::cVector forward;

				if (UpdateForward)
				{
					forward.z = -cosf(Math::ConvertDegreesToRadians(m_orientationEular.y))*
						cosf(Math::ConvertDegreesToRadians(m_orientationEular.x));
					forward.y = -sinf(Math::ConvertDegreesToRadians(m_orientationEular.x));
					forward.x = sinf(Math::ConvertDegreesToRadians(m_orientationEular.y))*
						cosf(Math::ConvertDegreesToRadians(m_orientationEular.x));
				}
				else
				{
					forward = i_forward;
				}

				m_localAxes.m_forward = forward.CreateNormalized();
				m_localAxes.m_right = Cross(m_localAxes.m_forward, Math::cVector::up).CreateNormalized();
				m_localAxes.m_up = Cross(m_localAxes.m_right, m_localAxes.m_forward).CreateNormalized();

				m_orientationQuaternion =
					Math::cQuaternion(Math::ConvertDegreesToRadians(m_orientationEular.x), Math::cVector::right)*
					Math::cQuaternion(Math::ConvertDegreesToRadians(m_orientationEular.y), Math::cVector::up)*
					Math::cQuaternion(Math::ConvertDegreesToRadians(m_orientationEular.z), Math::cVector::forward);
			}

			explicit Transform(const Math::cVector i_position = Math::cVector::zero, const Math::cVector i_orientationEular = Math::cVector::zero)
				:
				m_position(i_position),
				m_orientationEular(i_orientationEular)
			{
				UpdateLocalAxes();
			}
		private:
			Math::cQuaternion m_orientationQuaternion;
			Math::cVector m_orientationEular;
		};
	}
}

#endif // !EAE6320_TRANSFORM_H