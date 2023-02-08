#include "NetworkObject.h"
#include "./enet/enet.h"
#include "Window.h"
#include <reactphysics3d/reactphysics3d.h>
using namespace NCL;
using namespace CSC8503;

NetworkObject::NetworkObject(GameObject& o, int id) : object(o)	{
	deltaErrors = 0;
	fullErrors  = 0;
	networkID   = id;
	o.SetNetworkObject(this);
}

NetworkObject::~NetworkObject()	{
}

bool NetworkObject::ReadPacket(GamePacket& p) {
	if (p.type == Delta_State) {
		return ReadDeltaPacket((DeltaPacket&)p);
	}
	if (p.type == Full_State) {
		return ReadFullPacket((FullPacket&)p);
	}
	return false; //this isn't a packet we care about!
}

bool NetworkObject::WritePacket(GamePacket** p, bool deltaFrame, int stateID) {
	if (deltaFrame) {
		if (!WriteDeltaPacket(p, stateID)) {
			return WriteFullPacket(p);
		}
	}
	return WriteFullPacket(p);
}
//Client objects recieve these packets
bool NetworkObject::ReadDeltaPacket(DeltaPacket &p) {
	if (p.fullID != lastFullState.stateID) {
		return false; //cant delta this frame
	}
	UpdateStateHistory(p.fullID);

	Vector3 fullPos = lastFullState.position;
	Quaternion fullOrientation = lastFullState.orientation;

	fullPos.x += p.pos[0];
	fullPos.y += p.pos[1];
	fullPos.z += p.pos[2];

	fullOrientation.x += ((float)p.orientation[0]) / 127.0f;
	fullOrientation.y += ((float)p.orientation[1]) / 127.0f;
	fullOrientation.z += ((float)p.orientation[2]) / 127.0f;
	fullOrientation.w += ((float)p.orientation[3]) / 127.0f;

	object.GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(fullPos.x, fullPos.y, fullPos.z), reactphysics3d::Quaternion(fullOrientation.x, fullOrientation.y, fullOrientation.z, fullOrientation.w)));
	return true;
}

bool NetworkObject::ReadFullPacket(FullPacket &p) {
	if (p.fullState.stateID < lastFullState.stateID) {
		return false;//received an old packet, ignore!
	}
	lastFullState = p.fullState;

	object.GetPhysicsObject()->setTransform(reactphysics3d::Transform(reactphysics3d::Vector3(lastFullState.position.x, lastFullState.position.y, lastFullState.position.z), reactphysics3d::Quaternion(lastFullState.orientation.x, lastFullState.orientation.y, lastFullState.orientation.z, lastFullState.orientation.w)));

	stateHistory.emplace_back(lastFullState);

	return true;
}

bool NetworkObject::WriteDeltaPacket(GamePacket**p, int stateID) {
	DeltaPacket* dp = new DeltaPacket();
	NetworkState state;
	if (!GetNetworkState(stateID, state)) {
		return false; // cant delta!
	}
	dp->fullID = stateID;
	dp->objectID = networkID;

	Vector3 currentPos = object.GetPhysicsObject()->getTransform().getPosition();
	Quaternion currentOrientation = object.GetPhysicsObject()->getTransform().getOrientation();

	currentPos -= state.position;
	currentOrientation -= state.orientation;

	dp->pos[0] = (char)currentPos.x;
	dp->pos[1] = (char)currentPos.y;
	dp->pos[2] = (char)currentPos.z;

	dp->orientation[0] = (char)(currentOrientation.x * 127.0f);
	dp->orientation[1] = (char)(currentOrientation.y * 127.0f);
	dp->orientation[2] = (char)(currentOrientation.z * 127.0f);
	dp->orientation[3] = (char)(currentOrientation.x * 127.0f);

	*p = dp;

	return true;
}

bool NetworkObject::WriteFullPacket(GamePacket**p) {
	FullPacket* fp = new FullPacket();

	fp->objectID = networkID;
	fp->fullState.position = object.GetPhysicsObject()->getTransform().getPosition();
	fp->fullState.orientation = object.GetPhysicsObject()->getTransform().getOrientation();
	fp->fullState.stateID = lastFullState.stateID++;
	*p = fp;
	return true;
}

NetworkState& NetworkObject::GetLatestNetworkState() {
	return lastFullState;
}

bool NetworkObject::GetNetworkState(int stateID, NetworkState& state) {
	for (auto i = stateHistory.begin(); i < stateHistory.end(); ++i) {
		if ((*i).stateID == stateID) {
			state = (*i);
			return true;
		}
	}
	return false;
}

void NetworkObject::UpdateStateHistory(int minID) {
	for (auto i = stateHistory.begin(); i < stateHistory.end();) {
		if ((*i).stateID < minID) {
			i = stateHistory.erase(i);
		}
		else {
			++i;
		}
	}
}

void NetworkObject::GameobjectMove(int i) {
	GameObject& g = getGameObject();
	switch (i)
	{
	case 1:
		g.GetPhysicsObject()->applyWorldForceAtCenterOfMass(g.GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, -5)); // forward
		break;

	case 2:
		g.GetPhysicsObject()->applyWorldForceAtCenterOfMass(g.GetPhysicsObject()->getTransform().getOrientation() * reactphysics3d::Vector3(0, 0, 5)); //backward
		break;

	case 3:
		g.GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(0, 2, 0)); //left
		break;

	case 4:
		g.GetPhysicsObject()->applyWorldTorque(reactphysics3d::Vector3(0, -2, 0)); //right
		break;

	case 5:
		g.GetPhysicsObject()->applyWorldForceAtCenterOfMass(reactphysics3d::Vector3(0, 50, 0)); //up
		break;
	}
}