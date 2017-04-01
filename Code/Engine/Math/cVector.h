/*
	This class represents a position or direction
*/

#ifndef EAE6320_MATH_CVECTOR_H
#define EAE6320_MATH_CVECTOR_H

// Class Declaration
//==================

namespace eae6320
{
	namespace Math
	{
		class cVector
		{
			// Interface
			//==========

		public:

			// Addition
			cVector operator +( const cVector& i_rhs ) const;
			cVector& operator +=( const cVector& i_rhs );

			// Subtraction / Negation
			cVector operator -( const cVector& i_rhs ) const;
			cVector& operator -=( const cVector& i_rhs );
			cVector operator -() const;

			// Multiplication
			cVector operator *( const float i_rhs ) const;
			cVector& operator *=( const float i_rhs );
			friend cVector operator *( const float i_lhs, const cVector& i_rhs );

			// Division
			cVector operator /( const float i_rhs ) const;
			cVector& operator /=( const float i_rhs );

			// Length / Normalization
			float GetLength() const;
			void Normalize();
			cVector CreateNormalized() const;
			cVector ClampMagnitude(const float i_maxLength) const;
			float DistanceBetween(const cVector& i_other) const;

			// Products
			friend float Dot( const cVector& i_lhs, const cVector& i_rhs );
			friend cVector Cross( const cVector& i_lhs, const cVector& i_rhs );

			// Comparison
			bool operator ==( const cVector& i_rhs ) const;
			bool operator !=( const cVector& i_rhs ) const;

			// Initialization / Shut Down
			//---------------------------

			cVector( const float i_x = 0.0f, const float i_y = 0.0f, const float i_z = 0.0f );

			// Copy and assignment
			cVector(const cVector& i_other);
			cVector& operator=(const cVector& i_other);

			// Data
			//=====

		// A vector class is one of the very few examples where I make data public
		public:
			static const cVector right;
			static const cVector up;
			static const cVector forward;

			static const cVector left;
			static const cVector down;
			static const cVector back;

			static const cVector zero;

			float x, y, z;

			// Friend Classes
			//===============

			friend class cMatrix_transformation;
			friend class cQuaternion;
		};

		// Friend Function Declarations
		//=============================

		cVector operator *( const float i_lhs, const cVector& i_rhs );
		float Dot( const cVector& i_lhs, const cVector& i_rhs );
		cVector Cross( const cVector& i_lhs, const cVector& i_rhs );
	}
}

#endif	// EAE6320_MATH_CVECTOR_H
