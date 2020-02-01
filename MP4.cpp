
#include "MP4.h"
#include "GLEW/INCLUDE/glew.h"
#include "CoreEngine.h"

Video::Video()
	: m_vidcap{}, m_CurrentFrame{}
{
	m_name = "Videos\\1mintest.mp4";
}

Video::~Video()
{
	Unload();
}

void Video::Load(std::string vid_name)
{
	m_vidcap.open(vid_name);
	if (!m_vidcap.isOpened())
	{
		std::cout << "Video: (" << vid_name << ") failed to load." << std::endl;

	}
	else
	{
	std::cout << "Video: (" << vid_name << ") loaded." << std::endl;
	m_name = vid_name;
	m_vidcap >> m_CurrentFrame;

	if (!m_CurrentFrame.empty())
	{
		LoadVidTexture(m_CurrentFrame.cols, m_CurrentFrame.rows, m_CurrentFrame.data);
	}

	//Get Videos FrameRate and calculate variables for Update
	FrameRate = static_cast<unsigned int>(m_vidcap.get(cv::CAP_PROP_FPS));
	TimeTillNextFrame = 1.0f / float(FrameRate);

	}

}
void Video::Update()
{
	if (IsPlaying)
	{
		TimePassedCounter += g_Timer->m_dt;
		if (TimePassedCounter < TimeTillNextFrame)
			return;
		else
		{
			m_vidcap >> m_CurrentFrame;
			TimePassedCounter = 0.0f;

			if (!m_CurrentFrame.empty())
			{
				LoadVidTexture(m_CurrentFrame.cols, m_CurrentFrame.rows, m_CurrentFrame.data);
			}
			else
			{
				//Video is done, stop playing or loop
				if (IsLooping)
				{
					Reset();
					Play();
				}
				else
				{
					IsPlaying = false;
				}
			}
		}
	}
	else
	{
		TimePassedCounter = 0.0f;
	}
}

void Video::Pause()
{
	IsPlaying = false;
}

void Video::Play()
{
	IsPlaying = true;
}

void Video::SetLoop(bool loop)
{
	IsLooping = loop;
}

bool Video::GetLoop()
{
	return IsLooping;
}


void Video::Reset()
{
	IsPlaying = false; //For safety sake
	m_vidcap.set(cv::CAP_PROP_POS_AVI_RATIO, 0);
	m_vidcap >> m_CurrentFrame;
	if (!m_CurrentFrame.empty())
	{
		LoadVidTexture(m_CurrentFrame.cols, m_CurrentFrame.rows, m_CurrentFrame.data);
	}
	TimePassedCounter = 0.0f;
}

void Video::Unload()
{
	IsPlaying = false; //For safety sake
	if (m_vidcap.isOpened())
	{
		
		m_vidcap.release();
	}

	if(!m_CurrentFrame.empty())
	{
		m_CurrentFrame.release();
	}

	if (m_TextureID != 0)
	{
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
	}

}

void Video::LoadVidTexture(int col, int row, void * data)
{
	if (m_TextureID != 0)
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, col, row, GL_BGR, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, col, row, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
