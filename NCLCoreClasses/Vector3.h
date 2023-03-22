/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <cmath>
#include <iostream>
#include "Matrix3.h"
#include "Quaternion.h"
#include <algorithm>

namespace reactphysics3d {
	class Vector3;
}

namespace NCL::Maths {
	class Vector2;
	class Vector4;

	class Vector3 {
	public:
		union {
			struct {
				float x;
				float y;
				float z;
			};
			float array[3];
		};
	public:
		constexpr Vector3(void) : x(0.0f), y(0.0f), z(0.0f) {}

		constexpr Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

		Vector3(const Vector2& v2, float z = 0.0f);
		Vector3(const Vector4& v4);
		Vector3(const reactphysics3d::Vector3& v3);



		reactphysics3d::Vector3 NCLtoReactVector3(NCL::Maths::Vector3 i);

		~Vector3(void) = default;

		Vector3 Normalised() const {
			Vector3 temp(*this);
			temp.Normalise();
			return temp;
		}

		void			Normalise() {
			float length = Length();

			if (length != 0.0f) {
				length = 1.0f / length;
				x = x * length;
				y = y * length;
				z = z * length;
			}
		}

		float	Length() const {
			return sqrt((x*x) + (y*y) + (z*z));
		}

		static Vector3 GetRotated(float angle, const Vector3& axis)  {

			Quaternion rotation = Quaternion::AxisAngleToQuaterion(axis.Normalised(), angle);
			
			Quaternion result = rotation * Quaternion(0, axis.x, axis.y, axis.z) * rotation.Conjugate();
			return Vector3(result.x, result.y, result.z);

			
		}

		static Vector3 lerp(const Vector3& start, const Vector3& end, float t) {
			t = std::fmax(0, std::fmin(1, t));
			return Vector3(
				(1 - t) * start.x + t * end.x,
				(1 - t) * start.y + t * end.y,
				(1 - t) * start.z + t * end.z
			);
		}

		static Vector3 Projection(const Vector3& first, const Vector3& onto)
		{
			// calculate the scalar projection of this vector onto onto
			float scalarProjection = Dot(first, onto) / onto.LengthSquared();

			// calculate the projection vector
			Vector3 projection = onto.Normalised() * scalarProjection;

			return projection;
		}


		static Vector3 RotateY(const Vector3& vec, float angleInRad)  {
			//float rad = Math::ToRadians(angle);
			float sinA = sinf(angleInRad);
			float cosA = cosf(angleInRad);

			Matrix3 rotationMatrix;
			rotationMatrix.array[0][0] = cosA;
			rotationMatrix.array[0][1] = 0.0f;
			rotationMatrix.array[0][2] = sinA;
			rotationMatrix.array[1][0] = 0.0f;
			rotationMatrix.array[1][1] = 1.0f;
			rotationMatrix.array[1][2] = 0.0f;
			rotationMatrix.array[2][0] = -sinA;
			rotationMatrix.array[2][1] = 0.0f;
			rotationMatrix.array[2][2] = cosA;

			return rotationMatrix * vec;
		}

		constexpr float	LengthSquared() const {
			return ((x*x) + (y*y) + (z*z));
		}

		constexpr float		GetMaxElement() const {
			float v = x;
			if (y > v) {
				v = y;
			}
			if (z > v) {
				v = z;
			}
			return v;
		}

		float		GetAbsMaxElement() const {
			float v = abs(x);
			if (abs(y) > v) {
				v = abs(y);
			}
			if (abs(z) > v) {
				v = abs(z);
			}
			return v;
		}

		static constexpr Vector3 Clamp(const Vector3& input, const Vector3& mins, const Vector3& maxs);

		static constexpr float	Dot(const Vector3 &a, const Vector3 &b) {
			return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
		}

		static Vector3	Cross(const Vector3 &a, const Vector3 &b) {
			return Vector3((a.y*b.z) - (a.z*b.y), (a.z*b.x) - (a.x*b.z), (a.x*b.y) - (a.y*b.x));
		}

		inline Vector3  operator+(const Vector3  &a) const {
			return Vector3(x + a.x, y + a.y, z + a.z);
		}

		inline Vector3  operator-(const Vector3  &a) const {
			return Vector3(x - a.x, y - a.y, z - a.z);
		}

		inline Vector3  operator-() const {
			return Vector3(-x, -y, -z);
		}

		inline Vector3  operator*(float a)	const {
			return Vector3(x * a, y * a, z * a);
		}

		inline Vector3  operator*(const Vector3  &a) const {
			return Vector3(x * a.x, y * a.y, z * a.z);
		}

		inline Vector3  operator/(const Vector3  &a) const {
			return Vector3(x / a.x, y / a.y, z / a.z);
		};

		inline Vector3  operator/(float v) const {
			return Vector3(x / v, y / v, z / v);
		};

		inline constexpr void operator+=(const Vector3  &a) {
			x += a.x;
			y += a.y;
			z += a.z;
		}

		inline void operator-=(const Vector3  &a) {
			x -= a.x;
			y -= a.y;
			z -= a.z;
		}


		inline void operator*=(const Vector3  &a) {
			x *= a.x;
			y *= a.y;
			z *= a.z;
		}

		inline void operator/=(const Vector3  &a) {
			x /= a.x;
			y /= a.y;
			z /= a.z;
		}

		inline void operator*=(float f) {
			x *= f;
			y *= f;
			z *= f;
		}

		inline void operator/=(float f) {
			x /= f;
			y /= f;
			z /= f;
		}

		inline float operator[](int i) const {
			return array[i];
		}

		inline float& operator[](int i) {
			return array[i];
		}

		inline bool	operator==(const Vector3 &A)const { return (A.x == x && A.y == y && A.z == z) ? true : false; };
		inline bool	operator!=(const Vector3 &A)const { return (A.x == x && A.y == y && A.z == z) ? false : true; };

		inline friend std::ostream& operator<<(std::ostream& o, const Vector3& v) {
			o << "Vector3(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
			return o;
		}
	};
}