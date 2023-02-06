/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Vector2.h"
#include <reactphysics3d/reactphysics3d.h>
#include "Vector4.h"
#include "Vector3.h"
#include <algorithm>
using namespace NCL;
using namespace Maths;
using namespace reactphysics3d;

NCL::Maths::Vector2::Vector2(const Vector3& v3) : x(v3.x), y(v3.y){
}

NCL::Maths::Vector2::Vector2(const Vector4& v4) : x(v4.x), y(v4.y) {
}

NCL::Maths::Vector2::Vector2(const reactphysics3d::Vector2& v2) : x(v2.x), y(v2.y) {
}

constexpr NCL::Maths::Vector2 NCL::Maths::Vector2::Clamp(const NCL::Maths::Vector2& input, const NCL::Maths::Vector2& mins, const NCL::Maths::Vector2& maxs) {
	return NCL::Maths::Vector2(
		std::clamp(input.x, mins.x, maxs.x),
		std::clamp(input.y, mins.y, maxs.y)
	);
}