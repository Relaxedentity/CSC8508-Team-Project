#pragma once

namespace NCL::CSC8503 {
	class MapNode {
	public:
		MapNode(Vector3 pos);
		~MapNode();
	protected:
		Vector3 location;
	};
}
