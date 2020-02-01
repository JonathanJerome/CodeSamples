/*Start Header*****************************************************************/
/*!
\file	AudioManager.h
\author <[name], [digipen id], and [srs id]>
\par	<[email]>
\date	<Jan 25, 2019>
\brief	AudioManager to handle all audio clips in the scene (declaration)

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/
#pragma once


#include <filesystem>
#include "fmod.hpp"			//headers to fmod lib
#include "fmod_errors.h"	
#include "System.h"			//access to f_functions and SerializationWrapper class

#include <iostream>			//printout for debugging
#include <unordered_map>	//storage of keys to audio files
#include <memory>			//memory allocation/serialisation purposes

#include "CoreEngine.h"
#include "AudioSource.h"
#include "AudioListener.h"
#include "AudioClip.h"
#include "AbstractSoundSystem.h"


namespace fs = std::experimental::filesystem;

class AudioManager : public AbstractSoundSystem
{
private:
	

public:
	bool m_Muted = false;
	float m_sfxvol = 0.8f;
	float m_bgmvol = 0.6f;
	float m_menvol = 0.8f;

	FMOD::ChannelGroup* m_BGMGroup;
	FMOD::ChannelGroup* m_GameGroup;
	FMOD::ChannelGroup* m_MenuGroup;

	float m_MasterVol;

	std::unordered_map<std::string, std::shared_ptr<AudioClip>> m_AudioResources;
	std::vector<std::string> m_ResourceNames;


	void FMODErrorCheck(FMOD_RESULT result);
	FMOD::System*  m_System;
	AudioManager();								//constructor
	~AudioManager();							//destructor

	void f_startup() override;					//(override) called once at engine start-up
	void f_update() override;					//(override) called once every frame
	void f_reboot() override;					//(override) called once at end of level (transition)
	void f_shutdown() override;					//(override) called once at engine shutdown

												//Return 0 == success, Return 1 == fail		 		
	int f_initialize() override;				//(override) called once after all Sub-Systems have called "start-up"
	int f_reinitialize() override;				//(override) called once at the start of "Load Level"
	int f_load() override;						//(override) load SubSystem data from XML
	int f_save() override;						//(override) save SubSystem data to   XML
	int f_imgui() override;						//(override) render ImGUI window to edit data

												//Helper functions of given type
	float GetVolume(int type);					//volume getter
	void SetVolume(int type, float newVol);		//volume setter
	void Pause(int type);						//pause audioclip
	void Resume(int type);						//resume audioclip
	void Mute(int type);						//mute audioclip
	void UnMute(int type);						//unmute audioclip
	
												//Helper functions of all types
	void PauseAllAudio();						//pause all audioclips
	void ResumeAllAudio();						//resume all audioclips
	void MuteAllAudio();						//mute all audioclips
	void UnMuteAllAudio();						//unmute all audioclips

	std::unordered_map<std::string, std::shared_ptr<AudioClip>>& f_getclipmap() { return m_AudioResources; }
	std::vector<std::string>& f_getclipnames() { return m_ResourceNames; }

	//For testing ***************************************************/
	std::string soundname;
	std::shared_ptr<AudioClip> m_currentsound;
	void PlayCurrentSound();
	//***************************************************************/
	//PlaySound just "soundname.ogg"
	void PlayBGMSound(std::string name, float vol = -1.0f) override;
	void PlaySFXSound(std::string name, float vol = -1.0f) override;

	void StopBGM();
	void StopSFX();
	void StopMENU();

	//1 Memory Pool for Audio Source
	MemoryPool* m_AudioSourcePool;
	//2 A vector to store and update all active Audio Sources
	std::vector<AudioSource*> m_sources;
	AudioSource* f_AddAudioSource();
	void f_RemoveAudioSource(AudioSource* tmp);
	//3 Non member function to create and register Audio Source components
	//Note: Remember to update
	void f_flagfordeletion(AudioSource* del);
	std::vector<AudioSource*> tobedeleted;
	//4 A single instance of Audio Listener, there can only be one in the game
	//Note: If it does not belong to anyone, place it on GLUN::Graphics::mainCamera
	AudioListener m_ears;

	void StartSounds();
};

Component* f_CreateAudioSource();
