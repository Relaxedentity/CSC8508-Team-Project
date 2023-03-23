#pragma once
#include "GameObject.h"
#include "NetworkBase.h"
#include "NetworkState.h"

namespace NCL::CSC8503 {
	class GameObject;

	struct FullPacket : public GamePacket {
		int		objectID = -1;

		NetworkState fullState;

		FullPacket() {
			type = Full_State;
			size = sizeof(FullPacket) - sizeof(GamePacket);
		}
	};
	struct ProjectilePacket : public GamePacket {
		int		lastID;
		int		myID;
		float	yaw[9];
		Vector3 projPos;

		ProjectilePacket() {
			type = Projectile_Fired;
			size = sizeof(ProjectilePacket);
		}
	};
	struct MovementPacket : public GamePacket {
		int		lastID;
		int		myID;
		char	buttonstates[8];
		float	yaw[9];
		MovementPacket() {
			type = Movement;
			size = sizeof(MovementPacket);
		}
	};
	struct HealthPacket : public GamePacket {
		int		myID;
		int		lastID;
		float		health;

		HealthPacket() {
			type = Health;
			size = sizeof(ProjectilePacket);
		}
	};

	struct DeltaPacket : public GamePacket {
		int		fullID		= -1;
		int		objectID	= -1;
		char	pos[3];
		char	orientation[4];
		//int projectileID;

		DeltaPacket() {
			type = Delta_State;
			size = sizeof(DeltaPacket) - sizeof(GamePacket);
		}
	};

	struct ClientPacket : public GamePacket {
		int		lastID;
		char	buttonstates[8];
		int		myID;
		float	yaw[9];
		Vector3 projPos;

		ClientPacket() {
			type = Received_State;
			size = sizeof(ClientPacket);
		}
	};
	struct InitialPacket : public GamePacket {
		int count;

		InitialPacket() {
			type = Player_Connected;
			size = sizeof(InitialPacket);
		}
	};

	class NetworkObject		{
	public:
		NetworkObject(GameObject& o, int id);
		virtual ~NetworkObject();

		//Called by clients
		virtual bool ReadPacket(GamePacket& p);
		//Called by servers
		virtual bool WritePacket(GamePacket** p, bool deltaFrame, int stateID);

		void UpdateStateHistory(int minID);
		GameObject& getGameObject() {
			return object;
		}
		int GetNetworkID() {
			return networkID;
		}
		void GameobjectMove(int i, Quaternion yaw, bool grounded);
		void GameObjectRotate(Quaternion yaw);
		

	protected:

		NetworkState& GetLatestNetworkState();

		bool GetNetworkState(int frameID, NetworkState& state);

		virtual bool ReadDeltaPacket(DeltaPacket &p);
		virtual bool ReadFullPacket(FullPacket &p);

		virtual bool WriteDeltaPacket(GamePacket**p, int stateID);
		virtual bool WriteFullPacket(GamePacket**p);

		GameObject& object;
		
		NetworkState lastFullState;

		std::vector<NetworkState> stateHistory;

		int deltaErrors;
		int fullErrors;

		int networkID;

		bool updateWaiting = false;
	};
}