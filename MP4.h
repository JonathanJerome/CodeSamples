#pragma once

//We use opencv to load MP4 files inorder to render them to textures in our game engine
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <string>



class Video
{
private:
	float m_LoadComplete = 0.0f;
	cv::VideoCapture m_vidcap;
	cv::Mat			 m_CurrentFrame;


public:

	//Conructors and destructors
	Video();
	~Video();

	//Methods
	//Load Video NOTE: string must me passed E.G. "Videos\\1mintest.mp4" <Where Video\\ is root directory, 1mintest is video name and .mp4 is extension>
	void Load(std::string vid_name);

	//Update
	void Update();
	//Pause
	void Pause();
	//Play
	void Play();
	//Loop
	void SetLoop(bool loop);
	bool GetLoop();
	//Reset
	void Reset();
	void Unload();

	//Convert To Texture
	void LoadVidTexture(int col, int row, void * data);

	//Variables - Video Properties
	//-Name
	std::string m_name;
	//-VideoFPS
	unsigned int FrameRate  = 0;
	float TimeTillNextFrame = 0.0f;
	float TimePassedCounter = 0.0f;

	//HackJob Boolean's (In Honour of Nash)
	bool IsPlaying = false;
	bool IsLooping = false;


	//Texture ID
	unsigned int m_TextureID = 0;







};
