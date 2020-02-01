#pragma once
#include "fmod.hpp"
#include "AudioClip.h"
#include "AbstractAudioSource.h"
#include "Component.h"
#include "GameObject.h"
#include "CoreEngine.h" 
#include "Math.h"

class AudioManager;

class AudioSource : public AbstractAudioSource
{
	//Position and velocity to set		 --set3DAttributes
	FMOD_VECTOR m_pos;
	FMOD_VECTOR m_pre_pos;
	FMOD_VECTOR m_vel;
	//Max and Min radius				 --set3DMinMaxDistance
	float m_min = 1.0f;
	float m_max = 1000.0f;
	//AABB for sound occulsion			 --set3DOcclusion
	float m_direct_occlusion = 0.5f;
	float m_reverb_occulsion = 0.5f;
	//Facing Direction for cone emission --set3DConeOrientation

	//
	float	m_ambientVolume = 0.5f;
	float	m_ambientCutOff = 1000.0f;   


	std::string m_tmpsound = "Magic_EnergyRingPass4.ogg";		//String to store ImGui Tmp Sound
	FMOD::Channel** m_channel = nullptr; 
	bool m_PlayOnStart = false;

public:
	AudioSource();												//constructor
	~AudioSource();												//destructor

	void f_startup()						override;			//(overide) called once on component creation
	void f_initialize()						override;			//(overide) called once after load
	void f_destroy()						override;			//(overide) called once before component deletion

	int f_imgui()							override;			//(override) called for ImGUI window to edit component data
	int f_save(XDocument doc, XNode node)	override;			//(override) save data to  XML
	int f_load(XDocument doc, XNode node)	override;			//(override) load datafrom XML

	AudioSource* f_clone()					override;			//Clone must be pure virtual
	AudioSource* f_gentemplate()			override;			//(override) Similar to f_clone() but System relinquishes control of component. (i.e. not stored in System's container, NOT updated) 
	// UPDATES
	void Update();

	
	void  PlayBGM(std::string play)							override;	//Play BGM
	void  PlaySFX(std::string play)							override;	//Play SFX
	void  PlaySFXLoop(std::string sound)					override;	//Play SFX LOOP
	void  PlayMenu(std::string play)						override;	//Play Menu
	void  Stop()											override;	//Stop Playing
	bool  IsPlaying(std::string sound = "DEF")				override;	//Check if Playing
	void  SetVolume(float vol)								override;	//Set Volume from 0.0(min) to 1.0f(max)
	float GetVolume()										override;	//Get Volume
	void  SetMinMax(float min = 1.0f, float max = 1000.0f)	override;	//Set Min/Max keep max at 1000
	float GetMin()											override;	//Get Min
	float GetMax()											override;	//Get Max 

	//Set Facing Direction
	//Get Facing Direction


private:
	friend AudioManager;
};

