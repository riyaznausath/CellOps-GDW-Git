#pragma once

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

class AudioSource
{
public:


	void update();
	void play();

	bool isPlaying();

	void setPosition(const FMOD_VECTOR newPosition);
	void setVelocity(const FMOD_VECTOR newVelocity);

	void setSound(std::string soundName);
	void setPaused(const bool isPaused);
	void setVolume(const float newVolume);
	void setLooping(const bool isLooping);

	void setRolloffMinMaxDistance(const float minDistance, const float maxDistance);

private:

	std::string currentSoundName;

	FMOD::Channel* pChannel;

	FMOD_VECTOR position;
	FMOD_VECTOR velocity;

	float rolloffMinDistance = 0.5f;
	float rolloffMaxDistance = 0.5f;
	float volume = 1.0f;
	bool bLoop = false;
	bool bPaused = false;


};

