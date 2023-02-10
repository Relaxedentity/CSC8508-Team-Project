/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Vector3.h"
#include <reactphysics3d/reactphysics3d.h>
#include "Vector4.h"
#include "Vector2.h"
#include <algorithm>
using namespace NCL;
using namespace Maths;
using namespace reactphysics3d;


NCL::Maths::Vector3::Vector3(const Vector2& v2, float newZ) : x(v2.x), y(v2.y), z(newZ) {
}

NCL::Maths::Vector3::Vector3(const Vector4& v4) : x(v4.x), y(v4.y), z(v4.z) {
}

NCL::Maths::Vector3::Vector3(const reactphysics3d::Vector3& v3) : x(v3.x), y(v3.y), z(v3.z) {
}

reactphysics3d::Vector3 NCLtoReactVector3(NCL::Maths::Vector3 i) {
	return reactphysics3d::Vector3(i.x, i.y, i.z);
}

constexpr NCL::Maths::Vector3 NCL::Maths::Vector3::Clamp(const NCL::Maths::Vector3& input, const NCL::Maths::Vector3& mins, const NCL::Maths::Vector3& maxs) {
	return NCL::Maths::Vector3(
		std::clamp(input.x, mins.x, maxs.x),
		std::clamp(input.y, mins.y, maxs.y),
		std::clamp(input.z, mins.z, maxs.z)
	);
}