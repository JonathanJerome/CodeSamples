#include "AudioManager.h"
#include <cassert>
#include <string>
#include "Input.h"
#include "CoreEngine.h"

// Constructor
AudioManager::AudioManager()
	:
	m_MasterVol(1.0f),
	m_System(nullptr),
	m_BGMGroup(nullptr),
	m_GameGroup(nullptr),
	m_MenuGroup(nullptr),
	m_ResourceNames(),
	m_AudioResources(),
	m_Muted(false)
{
	// Create system
	FMODErrorCheck(FMOD::System_Create(&m_System));

	// Check number of drivers 
	int numDriver = 0;
	FMODErrorCheck(m_System->getNumDrivers(&numDriver));

	if (numDriver == 0)
	{
		FMODErrorCheck(m_System->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
	}

	// Init FMOD
	FMODErrorCheck(m_System->init(120, FMOD_INIT_NORMAL, nullptr));

	FMODErrorCheck(m_System->createChannelGroup(NULL, &m_BGMGroup));
	FMODErrorCheck(m_System->createChannelGroup(NULL, &m_GameGroup));
	FMODErrorCheck(m_System->createChannelGroup(NULL, &m_MenuGroup));
}

// Destructor
AudioManager::~AudioManager()
{
}

// Private function for error checking
void AudioManager::FMODErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
#ifdef _DEBUG
		std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
#endif
		assert(false);
	}
}

void AudioManager::f_startup()
{
	m_AudioSourcePool = engine->m_meman.f_AddcomponentPool<AudioSource>(DEFAULTPOOLSZ, "AudioSource");

	//Load the four folders with .ogg sounds
	//Others - the primary folder for all sounds that have no home - Auto FMOD2D
	for (auto& p : fs::directory_iterator("Audio//OTHERS"))
	{
		std::string path = p.path().string();
		std::string name = p.path().filename().string();
		//std::cout << "Sound loaded OTHERS: " << name << std::endl;
		f_getclipnames().push_back(name);
		f_getclipmap().emplace(name, std::make_shared<AudioClip>(path, name, 0));
	}
	//BGM - the primary folder for all sounds that will be used as BGMs- Auto FMOD2D
	for (auto& p : fs::directory_iterator("Audio//BGM"))
	{
		std::string path = p.path().string();
		std::string name = p.path().filename().string();
		//Check if we are inserting duplicate sounds
		if (m_AudioResources.find(name) == m_AudioResources.end())
		{
			//std::cout << "Sound loaded BGM: " << name << std::endl;
			f_getclipnames().push_back(name);
			f_getclipmap().emplace(name, std::make_shared<AudioClip>(path, name, 0));
		}
		else
		{
			std::cout << "SOUND::ERROR " << name << " already loaded from OTHER folder" << std::endl;
		}
	}
	//MENU - the primary folder for all sounds that will be used in Menus - Auto FMOD2D
	for (auto& p : fs::directory_iterator("Audio//MENU"))
	{
		std::string path = p.path().string();
		std::string name = p.path().filename().string();
		//Check if we are inserting duplicate sounds
		if (m_AudioResources.find(name) == m_AudioResources.end())
		{
			//std::cout << "Sound loaded MENU: " << name << std::endl;
			f_getclipnames().push_back(name);
			f_getclipmap().emplace(name, std::make_shared<AudioClip>(path, name, 0));
		}
		else
		{
			std::cout << "SOUND::ERROR " << name << " already loaded from OTHER/BGM folder" << std::endl;
		}
	}
	
	for (auto& p : fs::directory_iterator("Audio//SFX"))
	{
		std::string path = p.path().string();
		std::string name = p.path().filename().string();
		//Check if we are inserting duplicate sounds
		if (m_AudioResources.find(name) == m_AudioResources.end())
		{
			//std::cout << "Sound loaded SFX: " << name << std::endl;
			f_getclipnames().push_back(name);
			f_getclipmap().emplace(name, std::make_shared<AudioClip>(path, name, 1));
		}
		else
		{
			std::cout << "SOUND::ERROR " << name << " already loaded from OTHER/BGM/MENU folder" << std::endl;
		}
	}

	//DIALOGUE - the primary folder for all sounds that will be used in DIALOGUE - Auto FMOD2D
	for (auto& p : fs::directory_iterator("Audio//DIALOGUE"))
	{
		std::string path = p.path().string();
		std::string name = p.path().filename().string();
		//Check if we are inserting duplicate sounds
		if (m_AudioResources.find(name) == m_AudioResources.end())
		{
			//std::cout << "Sound loaded MENU: " << name << std::endl;
			f_getclipnames().push_back(name);
			f_getclipmap().emplace(name, std::make_shared<AudioClip>(path, name, 0));
		}
		else
		{
			std::cout << "SOUND::ERROR " << name << " already loaded from OTHER/BGM folder" << std::endl;
		}
	}

	m_BGMGroup->setVolume(m_bgmvol);
	m_GameGroup->setVolume(m_sfxvol);
}

// Update
void AudioManager::f_update()
{
	std::chrono::steady_clock::time_point lasttime, currenttime;
	lasttime = std::chrono::steady_clock::now();

	for (auto & elem : tobedeleted)
	{
		f_RemoveAudioSource(elem);
	}
	tobedeleted.clear();
	
	//Update Audio Sources
	for (auto & elem : m_sources)
	{
		elem->Update();
	}
	//Update Audio Listener
	m_ears.Update();
	
	if (m_System)
		m_System->update();


	currenttime = std::chrono::steady_clock::now();
	g_Timer->m_SoundSystem = static_cast<std::chrono::duration<float>>(currenttime - lasttime).count();

}

void AudioManager::f_reboot()
{
	StopBGM();
	StopSFX();
	StopMENU();

	for (auto & elem : m_sources)
	{
		m_AudioSourcePool->f_DeleteMem(elem);
	}
	tobedeleted.clear();
	m_sources.clear();

}


// Shutdown / Clean up operations before game is closed
void AudioManager::f_shutdown()
{
	if (m_BGMGroup)
		FMODErrorCheck(m_BGMGroup->release());
	if (m_GameGroup)
		FMODErrorCheck(m_GameGroup->release());
	if (m_MenuGroup)
		FMODErrorCheck(m_MenuGroup->release());

	for (auto& clip : m_AudioResources)
	{
		if(clip.second && clip.second->m_sound != nullptr)
			clip.second->m_sound->release();
	}
	m_AudioResources.clear();

	if (m_System)
	{
		FMODErrorCheck(m_System->close());
		FMODErrorCheck(m_System->release());
	}
}

// System Initialize after all systems are constructed
int AudioManager::f_initialize()
{
	

	return 0;
}

int AudioManager::f_reinitialize()
{
	/*if (engine->m_lvs.m_currlevel == "HackJob" && engine->m_EditorActive == false)
		getsys(AudioManager)->PlayBGMSound("Arrival_intro.ogg");*/
	return 0;
}

int AudioManager::f_load()
{
	//Load Volume for channels and other sound data
	return 0;
}

int AudioManager::f_save()
{
	//Save Volume for channels and other sound data
	return 0;
}

int AudioManager::f_imgui()
{
	ImGui::Text("Current Selected Sound: %s", soundname.c_str());

	ImGui::BeginChild("SoundSelector", ImVec2(0, 0));
	ImGui::Columns(2, "SCols");


	auto it = m_ResourceNames.begin();
	static int soundchosen = 0;

	for (unsigned i = 0; i < m_ResourceNames.size(); ++i, ++it)
	{
		if (ImGui::Selectable(it->c_str(), soundchosen == i))
		{
			soundchosen = i;
		}

	}

	ImGui::SetColumnOffset(1, 450);
	ImGui::NextColumn();

	//Old Listbox showcase
	//auto GameObjectSelector = [](void* data, int idx, const char**name)
	//{
	//	if (idx >= 0 && idx < (*reinterpret_cast<std::vector<std::string>*>(data)).size())
	//	{
	//		*name = (*reinterpret_cast<std::vector<std::string>*>(data))[idx].c_str();
	//		return true;
	//	}
	//	return false;
	//};
	//
	//if (ImGui::ListBox("Sounds", &(soundchosen), GameObjectSelector, &m_ResourceNames, static_cast<int>(m_ResourceNames.size())))
	//{
	//	soundname = m_ResourceNames[soundchosen];
	//	m_currentsound = (*(m_AudioResources.find(soundname))).second;
	//}

	if(ImGui::Button("PlayBGM"))
	{
		soundname = m_ResourceNames[soundchosen];
		PlayBGMSound(soundname);

	}

	if (ImGui::Button("PlaySFX"))
	{
		soundname = m_ResourceNames[soundchosen];
		PlaySFXSound(soundname);

	}


	if (ImGui::Button("StopBGM"))
	{
		StopBGM();
	}

	if (ImGui::Button("StopSFX"))
	{
		StopSFX();
	}

	if (ImGui::DragFloat("SFX_VOL", &m_sfxvol, 0.01f, 0.0f, 1.0f))
	{
		m_GameGroup->setVolume(m_sfxvol);
	}
	if (ImGui::DragFloat("BGM_VOL", &m_bgmvol, 0.01f, 0.0f, 1.0f))
	{
		m_BGMGroup->setVolume(m_bgmvol);
	}
	if (ImGui::DragFloat("MENU_VOL", &m_menvol, 0.01f, 0.0f, 1.0f))
	{
		m_MenuGroup->setVolume(m_menvol);
	}
	
	

	ImGui::EndChild();
	return 0;
}

// Get Volume from individual groups
float AudioManager::GetVolume(int type)
{
	float volume = 0.0f;

	switch (type)
	{
	case BGM:
		m_BGMGroup->getVolume(&volume);
		break;

	case GAMESFX:
	case GAMELOOPSFX:
	case GAMENON3D:
		m_GameGroup->getVolume(&volume);
		break;

	case MENUSFX:
		m_MenuGroup->getVolume(&volume);
		break;

	default:
		std::cout << "FMOD error! (Unable to get volume) - GetVolume(AudioType&) function." << std::endl;
		break;
	}

	return volume;
}

// Set Volume for individual groups
void AudioManager::SetVolume(int type, float volume)
{
	switch (type)
	{
	case BGM:
		m_BGMGroup->setVolume(volume * m_MasterVol);
		break;

	case GAMESFX:
	case GAMELOOPSFX:
	case GAMENON3D:
		m_GameGroup->setVolume(volume * m_MasterVol);
		break;

	case MENUSFX:
		m_MenuGroup->setVolume(volume * m_MasterVol);
		break;

	case ALL:
		if (m_MasterVol == 0)
		{
			m_BGMGroup->setVolume(m_MasterVol);
			m_GameGroup->setVolume(m_MasterVol);
			m_MenuGroup->setVolume(m_MasterVol);
		}
		else
		{
			m_BGMGroup->setVolume(GetVolume(BGM) / m_MasterVol * volume);
			m_GameGroup->setVolume(GetVolume(GAMESFX) / m_MasterVol * volume);
			m_MenuGroup->setVolume(GetVolume(MENUSFX) / m_MasterVol * volume);
		}
		m_MasterVol = volume;
		break;
	}
}

// Resume Audio Group
void AudioManager::Resume(int type)
{
	switch (type)
	{
	case BGM:
		m_BGMGroup->setPaused(false);
		break;

	case GAMESFX:
	case GAMELOOPSFX:
	case GAMENON3D:
		m_GameGroup->setPaused(false);
		break;

	case MENUSFX:
		m_MenuGroup->setPaused(false);
		break;

	case ALL:
		ResumeAllAudio();
		break;
	}
}

// Pause Audio Group
void AudioManager::Pause(int type)
{
	switch (type)
	{
	case BGM:
		m_BGMGroup->setPaused(true);
		break;

	case GAMESFX:
	case GAMELOOPSFX:
	case GAMENON3D:
		m_GameGroup->setPaused(true);
		break;

	case MENUSFX:
		m_MenuGroup->setPaused(true);
		break;

	case ALL:
		PauseAllAudio();
		break;
	}
}

// Mute Audio Group
void AudioManager::Mute(int type)
{
	switch (type)
	{
	case BGM:
		m_BGMGroup->setMute(true);
		break;

	case GAMESFX:
	case GAMELOOPSFX:
	case GAMENON3D:
		m_GameGroup->setMute(true);
		break;

	case MENUSFX:
		m_MenuGroup->setMute(true);
		break;

	case ALL:
		MuteAllAudio();
		break;
	}
}

// Unmute Audio Group
void AudioManager::UnMute(int type)
{
	switch (type)
	{
	case BGM:
		m_BGMGroup->setMute(false);
		break;

	case GAMESFX:
	case GAMELOOPSFX:
	case GAMENON3D:
		m_GameGroup->setMute(false);
		break;

	case MENUSFX:
		m_MenuGroup->setMute(false);
		break;

	case ALL:
		MuteAllAudio();
		break;
	}
}

// Resume All
void AudioManager::ResumeAllAudio()
{
	if (m_BGMGroup)
		m_BGMGroup->setPaused(false);

	if (m_GameGroup)
		m_GameGroup->setPaused(false);

	if (m_MenuGroup)
		m_MenuGroup->setPaused(false);
}

// Pause All
void AudioManager::PauseAllAudio()
{
	if (m_BGMGroup)
		m_BGMGroup->setPaused(true);

	if (m_GameGroup)
		m_GameGroup->setPaused(true);

	if (m_MenuGroup)
		m_MenuGroup->setPaused(true);
}

// Mute All
void AudioManager::MuteAllAudio()
{
	if (m_BGMGroup)
		m_BGMGroup->setMute(true);

	if (m_GameGroup)
		m_GameGroup->setMute(true);

	if (m_MenuGroup)
		m_MenuGroup->setMute(true);

	m_Muted = true;
}

// Unmute All
void AudioManager::UnMuteAllAudio()
{
	if (m_BGMGroup)
		m_BGMGroup->setMute(false);

	if (m_GameGroup)
		m_GameGroup->setMute(false);

	if (m_MenuGroup)
		m_MenuGroup->setMute(false);

	m_Muted = false;
}




void AudioManager::PlayCurrentSound()
{
	FMOD::Channel * m_channel = nullptr;
	m_System->playSound(m_currentsound->m_sound, m_BGMGroup, false, &m_channel);

}

void AudioManager::PlayBGMSound(std::string name, float vol)
{
	float volume = vol > 0.0f ? vol : m_bgmvol; //If vol is positive then use user volume
	if (m_AudioResources.find(name) != m_AudioResources.end())
	{
		m_currentsound = (*(m_AudioResources.find(name))).second;
		FMOD::Channel * m_channel = nullptr;
		m_channel->setMode(FMOD_LOOP_NORMAL);
		m_currentsound->m_sound->setMode(FMOD_LOOP_NORMAL);
		m_System->playSound(m_currentsound->m_sound, m_BGMGroup, false, &m_channel);
		m_channel->setVolume(volume);
		//std::cout << "playing: " << name << std::endl;
	}

}
	

void  AudioManager::PlaySFXSound(std::string name, float vol)
{
	float volume = vol > 0.0f ? vol : m_sfxvol; //If vol is positive then use user volume
	if (m_AudioResources.find(name) != m_AudioResources.end())
	{
		m_currentsound = (*(m_AudioResources.find(name))).second;
		FMOD::Channel *  m_channel = nullptr;
		m_System->playSound(m_currentsound->m_sound, m_GameGroup, false, &m_channel);
		if (m_Muted == false)
		{
			m_channel->setVolume(volume);
		}
		
		//std::cout << "playing: " << name << std::endl;
	}
}

void  AudioManager::StopBGM()
{
	m_BGMGroup->stop();
}

void  AudioManager::StopSFX()
{
	m_GameGroup->stop();
}

void AudioManager::StopMENU()
{
	m_MenuGroup->stop();
}

AudioSource * AudioManager::f_AddAudioSource()
{
	AudioSource* tmp = m_AudioSourcePool->f_GetComponentMem<AudioSource>();
	m_sources.push_back(tmp);
	return tmp;
}

void AudioManager::f_RemoveAudioSource(AudioSource * tmp)
{
	assert(tmp);
	RemoveFromVector(m_sources, tmp);
	m_AudioSourcePool->f_DeleteMem(tmp);
}

void AudioManager::f_flagfordeletion(AudioSource * del)
{
	for (const auto & elem : tobedeleted)
	{
		if (elem == del)
			return;
	}

	tobedeleted.push_back(del);
}

void AudioManager::StartSounds()
{
	for (auto & elem : m_sources)
	{
		if (elem->m_PlayOnStart)
		{
			elem->PlaySFXLoop(elem->m_tmpsound);
		}
	}
}

Component * f_CreateAudioSource()
{
	return getsys(AudioManager)->f_AddAudioSource();
}
