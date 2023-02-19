#pragma once
#include <irrKlang.h>
#include "GameTechRenderer.h"
using namespace irrklang;
namespace NCL {
	namespace CSC8503 {
		class SoundObject {
		public:
			void ShootVoice(ISoundEngine* shootvoice, Vector3 ShootPosition);
			void HitVoice(ISoundEngine* hitvoice,Vector3 HitPosition);
			void MainMusic(ISoundEngine* mainmusic);
			void JumpVoice(ISoundEngine* hitvoice, Vector3 PlayerPosition);
			void MoveVoice(ISoundEngine* hitvoice, Vector3 PlayerPosition);
		};
	}
}