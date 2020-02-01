#include "ComponentManager.h"

//include systems with components you want to register
#include "Physics.h"
#include "Graphics.h"
#include "AudioManager.h"



void ComponentManager::f_initialize()
{
	regComponent(RigidbodyVolume, f_CreateRBV);
	regComponent(Mesh, GLUN::f_CreateMesh2D);
	regComponent(Model, GLUN::f_CreateModel);
    regComponent(AnimatedModel, GLUN::f_CreateAnimatedModel);
    regComponent(Camera, GLUN::f_CreateCamera);
    regComponent(Light, GLUN::f_CreateLight);
    regComponent(ParticleEmitter, GLUN::f_CreateParticleEmitter);
	regComponent(Button, f_CreateButton);
	regComponent(AudioSource, f_CreateAudioSource);}

void ComponentManager::f_shutdown()
{
	m_createdefault.clear();
}

