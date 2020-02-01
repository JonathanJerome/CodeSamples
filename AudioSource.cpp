#include "AudioSource.h"
#include "AudioManager.h"
#include <cassert>
#include "Random.h"

AudioSource::AudioSource() {}

AudioSource::~AudioSource() {}

void AudioSource::f_startup()
{

}

void AudioSource::f_initialize()
{
	m_pos.x = f_getowner()->f_GetPos().x;
	m_pos.y = f_getowner()->f_GetPos().y;
	m_pos.z = f_getowner()->f_GetPos().z;
	
	m_pre_pos.x = f_getowner()->f_GetPos().x;
	m_pre_pos.y = f_getowner()->f_GetPos().y;
	m_pre_pos.z = f_getowner()->f_GetPos().z;
	
	if(m_channel != nullptr)
		(*m_channel)->setChannelGroup(getsys(AudioManager)->m_GameGroup);
	//getsys(AudioManager)->m_System->getChannel(Random::Rand(0,50), &m_channel);
	//m_channel->set3DOcclusion(m_direct_occlusion, m_reverb_occulsion);
}

void AudioSource::f_destroy()
{
	getsys(AudioManager)->f_flagfordeletion(this);
}

int AudioSource::f_imgui()
{
	if (ImGui::CollapsingHeader("AudioSource"))
	{
		{
			static ImGuiTextFilter filter;

			ImGui::Separator();
			std::string tmp;
			int i = 0;
			ImGui::Text("Ambient Noise Options:");
			ImGui::DragFloat("AmbientCutOff", &m_ambientCutOff);
			ImGui::DragFloat("AmbientVolume", &m_ambientVolume);
			ImGui::Checkbox("PlayAmbientOnStart", &m_PlayOnStart);
			if (ImGui::DragFloat("Max (Keep at 1000.0f)", &m_max))
			{
				if (m_channel != nullptr)
					(*m_channel)->set3DMinMaxDistance(m_min, m_max);
			}
			if (ImGui::DragFloat("Min", &m_min))
			{
				if (m_channel != nullptr)
					(*m_channel)->set3DMinMaxDistance(m_min, m_max);
			}



			static int selected = -1;

			ImGui::Text(std::string{ "Current Sound: " + m_tmpsound }.c_str());

			ImGui::Separator();

			filter.Draw("Audio source");

			ImGui::BeginChild("AudioSource");

			for (auto& p : fs::directory_iterator("Audio//SFX"))
			{
				tmp = p.path().filename().string();

				if (filter.PassFilter(tmp.c_str()))
				{
					if (ImGui::Selectable(tmp.c_str(), selected == i))
					{
						selected = i;
						m_tmpsound = tmp;
					}
				}


				++i;
			}

		}

		ImGui::EndChild();
		
		ImGui::Separator();

		if (ImGui::Button("SFX"))
		{
			PlaySFX(m_tmpsound);
		}
		ImGui::SameLine();
		if (ImGui::Button("SFXLoop"))
		{
			PlaySFXLoop(m_tmpsound);
		}
		ImGui::SameLine();
		if (ImGui::Button("BGM"))
		{
			PlayBGM(m_tmpsound);
		}
		ImGui::SameLine();
		if (ImGui::Button("MENU"))
		{
			PlayMenu(m_tmpsound);
		}

		if (ImGui::Button("Stop"))
		{
			Stop();
		}

		ImGui::Separator();
		if (ImGui::Button("Remove"))
		{
			f_getowner()->f_removecomp<AudioSource>(this);
			f_destroy();
			return 1;
		}
		
	}
	return 0;
}

int AudioSource::f_save(XDocument doc, XNode node)
{
	x_savestring(doc, node, "NAME", typeid(AudioSource).name()); //Important
	x_savefloat(node, "Max", m_max);
	x_savefloat(node, "Min", m_min);
	x_savestring(doc, node, "Sound", m_tmpsound);
	x_savebool(node, "PlayOnStart", m_PlayOnStart);
	x_savefloat(node, "m_ambientCutOff", m_ambientCutOff);
	x_savefloat(node, "m_ambientVolume", m_ambientVolume);
	return 0;
}

int AudioSource::f_load(XDocument doc, XNode node)
{
	x_loadfloat(node, "Max", m_max);
	x_loadfloat(node, "Min", m_min);
	x_loadstring(doc, node, "Sound", m_tmpsound);
	x_loadbool(node, "PlayOnStart", m_PlayOnStart);
	x_loadfloat(node, "m_ambientCutOff", m_ambientCutOff);
	x_loadfloat(node, "m_ambientVolume", m_ambientVolume);
	return 0;
}

AudioSource* AudioSource::f_clone()
{
	//Add to system
	AudioSource* tmp = getsys(AudioManager)->f_AddAudioSource();
	//Copy all relevant data
	tmp->m_min = m_min;
	tmp->m_max = m_max;
	//Return pointer
	return tmp;
}

AudioSource * AudioSource::f_gentemplate()
{
	//Use only memory from memory manager
	AudioSource* tmp = getsys(AudioManager)->m_AudioSourcePool->f_GetComponentMem<AudioSource>();
	//Copy all relevant data
	tmp->m_min = m_min;
	tmp->m_max = m_max;
	//Return pointer
	return tmp;
}

void AudioSource::Update()
{
	//Update 3D attributes

	Camera * CamTmp = getsys(GLUN::Graphics)->GetCamera();
	float tmpDis = Magnitude(CamTmp->GetPos() - f_getowner()->f_GetPos());
	
	if (m_ambientCutOff > tmpDis)
	{
		if (m_channel != nullptr)
			(*m_channel)->setMute(false);
		//m_pre_pos = m_pos;
		if (f_getowner())
		{

			m_pos.x = f_getowner()->f_GetPos().x;
			m_pos.y = f_getowner()->f_GetPos().y;
			m_pos.z = f_getowner()->f_GetPos().z;
		}
		/*m_vel.x = m_pos.x - m_pre_pos.x / g_Timer->m_dt;
		m_vel.y = m_pos.y - m_pre_pos.y / g_Timer->m_dt;
		m_vel.z = m_pos.z - m_pre_pos.z / g_Timer->m_dt;*/

		m_vel.x = 0.0f;
		m_vel.y = 0.0f;
		m_vel.z = 0.0f;
		if (m_channel != nullptr)
			(*m_channel)->set3DAttributes(&m_pos, &m_vel);
	}
	else
	{
		if (m_channel != nullptr)
			(*m_channel)->setMute(true);
	}
	
	

}

void AudioSource::PlayBGM(std::string play)
{
	if (IsPlaying())
	{
		Stop();
	}

	if (getsys(AudioManager)->m_AudioResources.find(play) != getsys(AudioManager)->m_AudioResources.end())
	{
		shared_ptr<AudioClip> m_clip = (*(getsys(AudioManager)->m_AudioResources.find(play))).second;
		m_clip->m_sound->setMode(FMOD_LOOP_NORMAL);
		m_clip->m_sound->setMode(FMOD_2D);

		getsys(AudioManager)->m_System->playSound(m_clip->m_sound, getsys(AudioManager)->m_BGMGroup, false, m_channel);
		m_tmpsound = m_clip->m_name;
		if ((m_channel) != nullptr)
		{
			(*m_channel)->setMode(FMOD_LOOP_NORMAL);
			(*m_channel)->setMode(FMOD_2D);
		}
		std::cout << "playing: " << play << std::endl;
		//m_channel->set3DAttributes(&m_pos, &m_vel);
		//m_channel->set3DMinMaxDistance(m_min, m_max);
		//m_channel->set3DOcclusion(m_direct_occlusion, m_reverb_occulsion);
	}
}

void AudioSource::PlaySFX(std::string play)
{
	if (IsPlaying())
	{
		Stop();
	}

	if (getsys(AudioManager)->m_AudioResources.find(play) != getsys(AudioManager)->m_AudioResources.end())
	{
		shared_ptr<AudioClip> m_clip = (*(getsys(AudioManager)->m_AudioResources.find(play))).second;
		m_clip->m_sound->setMode(FMOD_LOOP_OFF);
		m_clip->m_sound->setMode(FMOD_3D);
		

		getsys(AudioManager)->m_System->playSound(m_clip->m_sound, getsys(AudioManager)->m_GameGroup, false, m_channel);
		m_tmpsound = m_clip->m_name;
		if ((m_channel) != nullptr)
		{
			if (getsys(AudioManager)->m_Muted == false)
			{
				(*m_channel)->setVolume(1.0f);
			}

			(*m_channel)->set3DAttributes(&m_pos, &m_vel);
			(*m_channel)->set3DMinMaxDistance(m_min, m_max);
			(*m_channel)->set3DOcclusion(m_direct_occlusion, m_reverb_occulsion);
			(*m_channel)->setMode(FMOD_LOOP_OFF);
			(*m_channel)->setMode(FMOD_3D);
		}
	}
}

void AudioSource::PlaySFXLoop(std::string sound)
{
	if (IsPlaying())
	{
		Stop();
	}

	if (getsys(AudioManager)->m_AudioResources.find(sound) != getsys(AudioManager)->m_AudioResources.end())
	{
		shared_ptr<AudioClip> m_clip = (*(getsys(AudioManager)->m_AudioResources.find(sound))).second;
		m_clip->m_sound->setMode(FMOD_LOOP_NORMAL);
		m_clip->m_sound->setMode(FMOD_3D);

		getsys(AudioManager)->m_System->playSound(m_clip->m_sound, getsys(AudioManager)->m_GameGroup, false, m_channel);
		m_tmpsound = m_clip->m_name;
		std::cout << "playing: " << sound << std::endl;
		if ((m_channel) != nullptr)
		{
			if (getsys(AudioManager)->m_Muted == false)
			{
				(*m_channel)->setVolume(m_ambientVolume);
			}
			(*m_channel)->setMode(FMOD_LOOP_NORMAL);
			(*m_channel)->setMode(FMOD_3D);
			(*m_channel)->set3DAttributes(&m_pos, &m_vel);
			(*m_channel)->set3DMinMaxDistance(m_min, m_max);
			(*m_channel)->set3DOcclusion(m_direct_occlusion, m_reverb_occulsion);
		}
	}
}

void AudioSource::PlayMenu(std::string play)
{
	if (IsPlaying())
	{
		Stop();
	}

	if (getsys(AudioManager)->m_AudioResources.find(play) != getsys(AudioManager)->m_AudioResources.end())
	{
		shared_ptr<AudioClip> m_clip = (*(getsys(AudioManager)->m_AudioResources.find(play))).second;
		
		m_clip->m_sound->setMode(FMOD_LOOP_OFF);
		m_clip->m_sound->setMode(FMOD_2D);

		getsys(AudioManager)->m_System->playSound(m_clip->m_sound, getsys(AudioManager)->m_MenuGroup, false, m_channel);
		m_tmpsound = m_clip->m_name;
		std::cout << "playing: " << play << std::endl;
		if ((m_channel) != nullptr)
		{
			if (getsys(AudioManager)->m_Muted == false)
			{
				(*m_channel)->setVolume(m_ambientVolume);
			}
			(*m_channel)->set3DAttributes(&m_pos, &m_vel);
			(*m_channel)->set3DMinMaxDistance(m_min, m_max);
			(*m_channel)->set3DOcclusion(m_direct_occlusion, m_reverb_occulsion);
			(*m_channel)->setMode(FMOD_LOOP_OFF);
			(*m_channel)->setMode(FMOD_2D);
		}
	}

}

void AudioSource::Stop()
{
	if((m_channel) != nullptr)
		(*m_channel)->stop();

	m_channel = nullptr;
}

bool AudioSource::IsPlaying(std::string sound)
{
	if (sound == "DEF")
	{
		bool playing = true;
		if ((m_channel) != nullptr)
			(*m_channel)->isPlaying(&playing);
		return playing;
	}
	else
	{
		bool playing = true;
		if ((m_channel) != nullptr)
			(*m_channel)->isPlaying(&playing);
		return (playing && m_tmpsound == sound);
	}

}

void AudioSource::SetVolume(float vol)
{
	if ((m_channel) != nullptr)
		(*m_channel)->setVolume(vol);
}

float AudioSource::GetVolume()
{
	float vol = 0.0f;
	if ((m_channel) != nullptr)
		(*m_channel)->getVolume(&vol);
	return vol;
}

void AudioSource::SetMinMax(float min, float max)
{
	m_min = min;
	m_max = max;
	if ((m_channel) != nullptr)
		(*m_channel)->set3DMinMaxDistance(min, max);
}

float AudioSource::GetMin()
{
	if ((m_channel) != nullptr)
		(*m_channel)->get3DMinMaxDistance(&m_min, &m_max);
	return m_min;
}

float AudioSource::GetMax()
{
	if ((m_channel) != nullptr)
		(*m_channel)->get3DMinMaxDistance(&m_min, &m_max);
	return m_max;
}


