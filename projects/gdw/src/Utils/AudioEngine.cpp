#include "AudioEngine.h"
#include "fmod_errors.h"

AudioEngine* AudioEngine::instance()
{

	//singleton
	static AudioEngine* pInstance = nullptr;
	if (pInstance == nullptr)
	{
		pInstance = new AudioEngine();
	}


	return pInstance;
}

AudioEngine::AudioEngine()
{

}

int AudioEngine::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD is not ok " << FMOD_ErrorString(result) << std::endl;
		return 1;
#ifdef _DEBUG
		__debugbreak();
#endif


	}

	return 0;
}

void AudioEngine::init()
{
	ErrorCheck(FMOD::System_Create(&pSystem));

	ErrorCheck(pSystem->init(32, FMOD_INIT_NORMAL, nullptr));

	pSystem->set3DNumListeners(1);

	pSystem->set3DSettings(1.0f, WORLD_SCALE, 1.0f);
	//Forward and up vectors can be created from your transformation matrices (world transform)

	FMOD_VECTOR up = { 0, 0, 1 };
	FMOD_VECTOR forward = { 0, 1 , 0 };
	FMOD_VECTOR position = { 0, 0, 0 };
	FMOD_VECTOR velocity = { 0, 0, 0 };

	FMOD::Studio::System::create(&pStudioSystem);


	pSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);
}

void AudioEngine::update()
{
	ErrorCheck(pSystem->update());
}

void AudioEngine::shutdown()
{
	ErrorCheck(pSystem->close());
	ErrorCheck(pSystem->release());
}

void AudioEngine::loadSound(const std::string& soundName, const std::string& fileName, bool b3d, bool bLooping, bool bStream)
{
	//check if loaded
	auto foundElement = sounds.find(soundName);
	if (foundElement != sounds.end())
	{
		//sound already loaded
		return;
	}

	FMOD_MODE mode = FMOD_DEFAULT;
	mode != (b3d) ? FMOD_3D : FMOD_2D;
	mode != (bLooping) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode != (bStream) ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;


	FMOD::Sound* loadedSound;
	ErrorCheck(pSystem->createSound(fileName.c_str(), mode, nullptr, &loadedSound));

	if (loadedSound != nullptr)
	{
		sounds[soundName] = loadedSound;
		loadedSound->set3DMinMaxDistance(1.0f, 1000.0);

	}

}

void AudioEngine::unloadSound(const std::string& soundName)
{
	auto foundElement = sounds.find(soundName);
	if (foundElement != sounds.end())
	{

		ErrorCheck(foundElement->second->release());
		sounds.erase(foundElement);
	}
}

void AudioEngine::playSoundByName(const std::string& soundName, FMOD::Channel** channelReturned)
{
	FMOD::Channel* pChanel = nullptr;

	auto soundIt = sounds.find(soundName);
	if (soundIt == sounds.end())
	{
#if _DEBUG
		__debugbreak;
		std::cerr << "ohes  noes sound: " << soundName << " was not found!";

#endif
	}



	ErrorCheck(pSystem->playSound((*soundIt).second, nullptr, false, channelReturned));

}

FMOD::System* AudioEngine::getSystem()
{
	return pSystem;
}