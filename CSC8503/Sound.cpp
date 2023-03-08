#include <irrKlang.h>

#include <conio.h>
#include "Sound.h"
#include "Assets.h"
#include "GameTechRenderer.h"
#include "Window.h"

using namespace irrklang;
using namespace NCL;
using namespace CSC8503;
using namespace Assets;



void SoundObject::ShootVoice(ISoundEngine* shootvoice, Vector3 ShootPosition)
{
	const std::string a = "bell.WAV";
	const std::string s = SOUNDSDIR + a;
	const char* HitVoicefilepos = s.c_str();
	ISound* HitV = shootvoice->play3D(HitVoicefilepos, vec3df(ShootPosition.x, ShootPosition.y, ShootPosition.z), false, false, true);
	HitV->setMinDistance(2.0f);
	HitV->drop();
}

void SoundObject::HitVoice(ISoundEngine* hitvoice, Vector3 HitPosition)
{
	const std::string a = "explosion.WAV";
	const std::string s = SOUNDSDIR + a;
	const char* HitVoicefilepos = s.c_str();
	ISound* HitV = hitvoice->play3D(HitVoicefilepos, vec3df(HitPosition.x, HitPosition.y, HitPosition.z), false, false, true);
	HitV->setMinDistance(10.0f);
	HitV->drop();
}

void SoundObject::MainMusic(ISoundEngine* mainmusic)
{
	const std::string a = "MF-PANTS.MOD";
	const std::string s = SOUNDSDIR + a;
	const char* soundfilepos = s.c_str();
	mainmusic->play2D(soundfilepos, true);
}

void SoundObject::JumpVoice(ISoundEngine* jumpvoice, Vector3 PlayerjumpPosition)
{
	const std::string a = "jump.WAV";
	const std::string s = SOUNDSDIR + a;
	const char* HitVoicefilepos = s.c_str();
	ISound* HitV = jumpvoice->play3D(HitVoicefilepos, vec3df(PlayerjumpPosition.x, PlayerjumpPosition.y, PlayerjumpPosition.z), false, false, true);
	HitV->setMinDistance(5.0f);
	HitV->drop();
}

void SoundObject::MoveVoice(ISoundEngine* runvoice, Vector3 runPosition)
{
	const std::string a = "RUN.WAV";
	const std::string s = SOUNDSDIR + a;
	const char* HitVoicefilepos = s.c_str();
	ISound* HitV = runvoice->play3D(HitVoicefilepos, vec3df(runPosition.x, runPosition.y, runPosition.z), false, false, true);
	HitV->setMinDistance(10.0f);
	HitV->drop();
}

void SoundObject::fireSoundMapping(ISoundEngine* fire, Vector3 MainCameraPostiton, Vector3 SecCameraPosition)
{

	ShootVoice(fire, MainCameraPostiton);
	ShootVoice(fire, SecCameraPosition);
}

void SoundObject::moveSoundMapping(ISoundEngine* move, Vector3 MainCameraPostiton, Vector3 SecCameraPosition)
{
	MoveVoice(move, MainCameraPostiton);
	MoveVoice(move, SecCameraPosition);
}

void SoundObject::jumpSoundMapping(ISoundEngine* jump, Vector3 MainCameraPostiton, Vector3 SecCameraPosition)
{
	JumpVoice(jump, MainCameraPostiton);
	JumpVoice(jump, SecCameraPosition);
}
