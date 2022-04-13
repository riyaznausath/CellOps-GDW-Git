#pragma once

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
//audio engine to play sounds by given filename

//implements singleton design pattern, a nice conviniet way to eb lazy and used all the time in games :D
//this design pattern allows global access to a single instance of a class and guarentees there will only be one
class AudioEngine
{


public:

	const float WORLD_SCALE = 1.0f;

	static AudioEngine* instance();

	static int ErrorCheck(FMOD_RESULT result);

	void init();
	void update();
	void shutdown();


	void loadSound(const std::string& soundName, const std::string& fileName, bool b3d, bool bLooping = false, bool bstream = false);
	void unloadSound(const std::string& soundName);
	void playSoundByName(const std::string& soundName, FMOD::Channel** channelReturned = nullptr);

	FMOD::System* getSystem();

private:

	//hide constructor
	AudioEngine();

	FMOD::Studio::System* pStudioSystem = nullptr;

	FMOD::System* pSystem = nullptr;
	std::unordered_map<std::string, FMOD::Sound*> sounds;



};

