#pragma once
#include <irrKlang.h>
#include "GameTechRenderer.h"
#include"Sound.h"
using namespace irrklang;
namespace NCL {
	namespace CSC8503 {
		class SoundObject {
		public:
			void capsule(ISoundEngine* shootvoice);
			void cat(ISoundEngine* shootvoice);
			void coin(ISoundEngine* shootvoice);
			void ShootVoice(ISoundEngine* shootvoice, Vector3 ShootPosition);
			void ShotGunShootVoice(ISoundEngine* shootvoice, Vector3 ShootPosition);
			void HitVoice(ISoundEngine* hitvoice,Vector3 HitPosition);
			void MainMusic(ISoundEngine* mainmusic);
			void JumpVoice(ISoundEngine* hitvoice, Vector3 PlayerPosition);
			void MoveVoice(ISoundEngine* hitvoice, Vector3 PlayerPosition);

			void fireSoundMapping(ISoundEngine* fire, Vector3 MainCameraPostiton, Vector3 SecCameraPosition);
			void ShotGunfireSoundMapping(ISoundEngine* fire, Vector3 MainCameraPostiton, Vector3 SecCameraPosition);
			void moveSoundMapping(ISoundEngine* move, Vector3 MainCameraPostiton, Vector3 SecCameraPosition);
			void jumpSoundMapping(ISoundEngine* jump, Vector3 MainCameraPostiton, Vector3 SecCameraPosition);
		};
	}
}