#include "AudioSource.h"
#include "AudioEngine.h"

void AudioSource::update()
{

	AudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, &velocity));

}

void AudioSource::play()
{


	AudioEngine::instance()->playSoundByName(currentSoundName, &pChannel);

	AudioEngine::ErrorCheck(pChannel->setVolume(volume));

	FMOD_MODE loopMode = bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;


	AudioEngine::ErrorCheck(pChannel->setMode(loopMode));
	pChannel->set3DMinMaxDistance(rolloffMinDistance, rolloffMaxDistance);
	setPaused(false);


}

bool AudioSource::isPlaying()
{
	bool isPlaying;
	AudioEngine::ErrorCheck(pChannel->isPlaying(&isPlaying));
	return isPlaying;
}

void AudioSource::setPosition(const FMOD_VECTOR newPosition)
{
	position = newPosition;

}

void AudioSource::setVelocity(const FMOD_VECTOR newVelocity)
{
	velocity = newVelocity;
}

void AudioSource::setSound(std::string soundName)
{
	//TODO:: check to make sure sound is valid 
	currentSoundName = soundName;

}

void AudioSource::setPaused(const bool isPaused)
{
	bPaused = isPaused;
	AudioEngine::ErrorCheck(pChannel->setPaused(bPaused));



}

void AudioSource::setVolume(const float newVolume)
{
	volume = newVolume;
	AudioEngine::ErrorCheck(pChannel->setVolume(volume));
}

void AudioSource::setLooping(const bool isLooping)
{
	bLoop = isLooping;

	FMOD_MODE loopMode = bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;


	AudioEngine::ErrorCheck(pChannel->setMode(loopMode));

}

void AudioSource::setRolloffMinMaxDistance(const float minDistance, const float maxDistance)
{
	rolloffMinDistance = minDistance;
	rolloffMaxDistance = maxDistance;

}
