/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Quaternion.h"
#include <reactphysics3d/reactphysics3d.h>
#include "Matrix4.h"
#include "Matrix3.h"
#include "Vector3.h"
#include "Maths.h"
#include <algorithm>
#include <cmath>

using namespace NCL;
using namespace NCL::Maths;
using namespace reactphysics3d;

NCL::Maths::Quaternion::Quaternion(void)
{
	x = y = z = 0.0f;
	w = 1.0f;
}

NCL::Maths::Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

NCL::Maths::Quaternion::Quaternion(double x, double y, double z, double w)
{
	this->x = (float)x;
	this->y = (float)y;
	this->z = (float)z;
	this->w = (float)w;
}

NCL::Maths::Quaternion::Quaternion(const Vector3& vector, float w) {
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
	this->w = w;
}

NCL::Maths::Quaternion::Quaternion(const Matrix4 &m) {
	w = sqrt(std::max(0.0f, (1.0f + m.array[0][0] + m.array[1][1] + m.array[2][2])))  * 0.5f;

	if (abs(w) < 0.0001f) {
		x = sqrt(std::max(0.0f, (1.0f + m.array[0][0] - m.array[1][1] - m.array[2][2]))) / 2.0f;
		y = sqrt(std::max(0.0f, (1.0f - m.array[0][0] + m.array[1][1] - m.array[2][2]))) / 2.0f;
		z = sqrt(std::max(0.0f, (1.0f - m.array[0][0] - m.array[1][1] + m.array[2][2]))) / 2.0f;

		x = (float)copysign(x, m.array[2][1] - m.array[1][2]);
		y = (float)copysign(y, m.array[0][2] - m.array[2][0]);
		z = (float)copysign(z, m.array[1][0] - m.array[0][1]);
	}
	else {
		float qrFour = 4.0f * w;
		float qrFourRecip = 1.0f / qrFour;

		x = (m.array[1][2] - m.array[2][1]) * qrFourRecip;
		y = (m.array[2][0] - m.array[0][2]) * qrFourRecip;
		z = (m.array[0][1] - m.array[1][0]) * qrFourRecip;
	}
}

NCL::Maths::Quaternion::Quaternion(const Matrix3& m) {
	w = sqrt(std::max(0.0f, (1.0f + m.array[0][0] + m.array[1][1] + m.array[2][2]))) * 0.5f;

	float qrFour = 4.0f * w;
	float qrFourRecip = 1.0f / qrFour;

	x = (m.array[5] - m.array[7]) * qrFourRecip;
	y = (m.array[6] - m.array[2]) * qrFourRecip;
	z = (m.array[1] - m.array[3]) * qrFourRecip;
}

NCL::Maths::Quaternion::Quaternion(const reactphysics3d::Quaternion& q) {
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

reactphysics3d::Quaternion NCLtoReactQuaternion(NCL::Maths::Quaternion i) {
	return reactphysics3d::Quaternion(i.x, i.y, i.z, i.w);
}

float NCL::Maths::Quaternion::Dot(const Quaternion &a,const Quaternion &b){
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

void NCL::Maths::Quaternion::Normalise(){
	float magnitude = sqrt(x*x + y*y + z*z + w*w);

	if(magnitude > 0.0f){
		float t = 1.0f / magnitude;

		x *= t;
		y *= t;
		z *= t;
		w *= t;
	}
}

NCL::Maths::Quaternion NCL::Maths::Quaternion::Normalised() const {
	Quaternion temp(*this);
	temp.Normalise();
	return temp;
}

void NCL::Maths::Quaternion::CalculateW()	{
	w = 1.0f - (x*x)-(y*y)-(z*z);
	if(w < 0.0f) {
		w = 0.0f;
	}
	else{
		w = - sqrt(w);
	}
}

NCL::Maths::Quaternion NCL::Maths::Quaternion::Conjugate() const
{
	return NCL::Maths::Quaternion(-x,-y,-z,w);
}

NCL::Maths::Quaternion NCL::Maths::Quaternion::Lerp(const NCL::Maths::Quaternion &from, const NCL::Maths::Quaternion &to, float by) {
	Quaternion temp = to;

	float dot = Quaternion::Dot(from,to);

	if(dot < 0.0f) {
		temp = -to;
	}

	return (from * (1.0f - by)) + (temp * by);
}
//SIGGRAPH Shoemake
NCL::Maths::Quaternion NCL::Maths::Quaternion::Slerp(const NCL::Maths::Quaternion &from, const NCL::Maths::Quaternion &to, float by) {
	float t = by;// / 2.0f;

	float dot = std::clamp(Quaternion::Dot(from,to), -1.0f, 1.0f);

	if (dot == 1.0f) {
		return from;
	}

	float theta = std::abs(acos(dot));

	float aScale = sin((1 - t) * theta);
	float bScale = sin(t * theta);

	Quaternion q = (from * aScale) + (to * bScale);

	q *= 1.0f / sin(theta);

	if (q.x != q.x) {
		bool a = true;
	}

	q.Normalise();
	return q;
}

//http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
//Verified! Different values to above, due to difference between x/z being 'forward'
NCL::Maths::Vector3 NCL::Maths::Quaternion::ToEuler() const {
	NCL::Maths::Vector3 euler;

	float t = x*y + z*w;

	if (t > 0.4999) {
		euler.z = Maths::RadiansToDegrees(Maths::PI / 2.0f);
		euler.y = Maths::RadiansToDegrees(2.0f * atan2(x, w));
		euler.x = 0.0f;

		return euler;
	}

	if (t < -0.4999) {
		euler.z = -Maths::RadiansToDegrees(Maths::PI / 2.0f);
		euler.y = -Maths::RadiansToDegrees(2.0f * atan2(x, w));
		euler.x = 0.0f;
		return euler;
	}

	float sqx = x*x;
	float sqy = y*y;
	float sqz = z*z;

	euler.z = Maths::RadiansToDegrees(asin(2 * t));
	euler.y = Maths::RadiansToDegrees(atan2(2 * y*w - 2 * x*z, 1.0f - 2 * sqy - 2 * sqz));
	euler.x = Maths::RadiansToDegrees(atan2(2 * x*w - 2 * y*z, 1.0f - 2 * sqx - 2.0f*sqz));

	return euler;
}

//http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/
//VERIFIED AS CORRECT - Pitch and roll are changed around as the above uses x as 'forward', whereas we use -z
NCL::Maths::Quaternion NCL::Maths::Quaternion::EulerAnglesToQuaternion(float roll, float yaw, float pitch) {
	float cos1 = (float)cos(Maths::DegreesToRadians(yaw   * 0.5f));
	float cos2 = (float)cos(Maths::DegreesToRadians(pitch * 0.5f));
	float cos3 = (float)cos(Maths::DegreesToRadians(roll  * 0.5f));

	float sin1 = (float)sin(Maths::DegreesToRadians(yaw   * 0.5f));
	float sin2 = (float)sin(Maths::DegreesToRadians(pitch * 0.5f));
	float sin3 = (float)sin(Maths::DegreesToRadians(roll  * 0.5f));

	NCL::Maths::Quaternion q;

	q.x = (sin1 * sin2 * cos3) + (cos1 * cos2 * sin3);
	q.y = (sin1 * cos2 * cos3) + (cos1 * sin2 * sin3);
	q.z = (cos1 * sin2 * cos3) - (sin1 * cos2 * sin3);
	q.w = (cos1 * cos2 * cos3) - (sin1 * sin2 * sin3);

	return q;
};

NCL::Maths::Quaternion NCL::Maths::Quaternion::AxisAngleToQuaterion(const NCL::Maths::Vector3& vector, float degrees) {
	float theta		= (float)Maths::DegreesToRadians(degrees);
	float result	= (float)sin(theta / 2.0f);

	return Quaternion((float)(vector.x * result), (float)(vector.y * result), (float)(vector.z * result), (float)cos(theta / 2.0f));
}

NCL::Maths::Quaternion NCL::Maths::Quaternion::LookAt(const Vector3& moveDir, const Vector3& up)
{
	// Normalize forward and up vectors
	Vector3 f = moveDir.Normalised();
	Vector3 u = up.Normalised();

	// Calculate right vector
	Vector3 r = Vector3::Cross(f, u);

	// Calculate quaternion from rotation matrix
	Matrix3 rotationMatrix;
	rotationMatrix.array[0][0] = r.x;
	rotationMatrix.array[0][1] = r.y;
	rotationMatrix.array[0][2] = r.z;
	rotationMatrix.array[1][0] = u.x;
	rotationMatrix.array[1][1] = u.y;
	rotationMatrix.array[1][2] = u.z;
	rotationMatrix.array[2][0] = -f.x;
	rotationMatrix.array[2][1] = -f.y;
	rotationMatrix.array[2][2] = -f.z;
	return Quaternion(rotationMatrix);
}


NCL::Maths::Vector3		NCL::Maths::Quaternion::operator *(const NCL::Maths::Vector3 &a)	const {
	NCL::Maths::Quaternion newVec = *this * NCL::Maths::Quaternion(a.x, a.y, a.z, 0.0f) * Conjugate();
	return NCL::Maths::Vector3(newVec.x, newVec.y, newVec.z);
}

