#ifndef EAE6320_TRANSFORM_H
#define EAE6320_TRANSFORM_H
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"
#include "LocalAxes.h"
#include "../../Engine/Math/Functions.h"

namespace eae6320
{
	namespace Gameplay
	{
		struct Transform
		{
			LocalAxes m_localAxes;
			Math::cVector m_position;
			Math::cVector m_orientationEular;
			Math::cQuaternion GetOrientation() const
			{
				return m_orientationQuaternion;
			}
			void UpdateLocalAxes()
			{
				Math::cVector forward;

				forward.z = -cosf(Math::ConvertDegreesToRadians(m_orientationEular.y))*
					cosf(Math::ConvertDegreesToRadians(m_orientationEular.x));
				forward.y = -sinf(Math::ConvertDegreesToRadians(m_orientationEular.x));
				forward.x = sinf(Math::ConvertDegreesToRadians(m_orientationEular.y))*
					cosf(Math::ConvertDegreesToRadians(m_orientationEular.x));

				m_localAxes.m_forward = forward.CreateNormalized();
				m_localAxes.m_right = Cross(m_localAxes.m_forward, Math::cVector::up).CreateNormalized();
				m_localAxes.m_up = Cross(m_localAxes.m_right, m_localAxes.m_forward).CreateNormalized();

				m_orientationQuaternion =
					Math::cQuaternion(Math::ConvertDegreesToRadians(m_orientationEular.x), m_localAxes.m_right)*
					Math::cQuaternion(Math::ConvertDegreesToRadians(m_orientationEular.y), m_localAxes.m_up)*
					Math::cQuaternion(Math::ConvertDegreesToRadians(m_orientationEular.z), m_localAxes.m_forward);
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
		};
	}
}

#endif // !EAE6320_TRANSFORM_H