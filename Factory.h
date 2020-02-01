/*Start Header*****************************************************************/
/*!
\file	Factory.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Jan 09, 2019>
\brief	Factory is responsible for Creating, Maintaining and Destroying GameObjects

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/
#pragma once

//Engine
#include "System.h"
#include "GameObject.h"
#include "CoreEngine.h"
#include "ComponentManager.h"
#include "ArchetypeManager.h"
#include "RedoUndo.h"
#include "Tag.h"
#include "AbstractFactory.h"

//STL
#include <map>
#include <unordered_map>
#include <set>
#include <assert.h>

class Factory : public AbstractFactory
{
private:
	
	vec3 tmp = { 0.0f };
	std::vector<GameObject*> m_tobedeleted;

	MemoryPool * m_GOpool;

public:
	Factory();											//Constructor
	~Factory();											//Destructor


	void f_startup()						override;	//(override) called once at engine start-up
	void f_update()							override;	//(override) called once every fram
	void f_reboot()							override;	//(override) called once at end of level (transition)
	void f_shutdown()						override;	//(override) called once at engine shutdown

													    //Return 0 == success, Return 1 == fail		 		
	int f_initialize()						override;	//(override) called once after all Sub-Systems have called "start-up"
	int f_reinitialize()					override;	//(override) called once at the start of "Load Level"
	int f_load()	override;							//(override) load SubSystem data from XML
	int f_save()	override;							//(override) save SubSystem data to   XML
	int f_imgui()							override;	//(override) render ImGUI window to edit data

	
														
	
	//Tag creation system
	//GUID generator

	//Search For Specific Game Object (Tag, Name, Component, ...)

	GameObject* f_creatego(vec3 pos = vec3{ 0,0,0 });
	GameObject* f_creatego(guid id);
	GameObject* f_creatego(vec3 p , vec3 s, vec3 r); 

	int getvecsize() { return (int)m_govec.size(); }



	int f_removego(GameObject * go);

	int f_removego(AbstractGameObject * go) override;

	void test() override { std::cout << "test factory" << std::endl; }

	bool GetPause() override;

	bool GetOptionPressed() override;

	bool GetIfGameOver() override;
	void setGameOver(bool b) override;

	void SetIntransition(bool b) override;

	AbstractGameObject * GetMseHoverGO();

	void f_flagchangeoflevel(std::string) override;
	void f_flagLoadingScreen(std::string) override;

	std::vector<GameObject*> m_govec;
	std::unordered_map<guid, GameObject*> m_gomap;

	std::vector<AbstractGameObject*> GetAllGameObjects() override;

	AbstractCamera* ffs_GetMainCamera() override;

	//#define getGameObject(num) engine->f_getsystem<Factory>()->m_gomap[num]

	AbstractGameObject* ffs_getObjectByGUID(guid input) override;

	AbstractGameObject* ffs_CloneObjectByGUID(guid object) override;

	std::string f_getCurrentLvl() override;

	ComponentManager m_compman;
	ArchetypeManager m_arche;
	TagManager		 m_tagman;



	GameObject* findName(const std::string & name);

    RedoUndo m_redoUndo;

	void f_ToggleAnimCull( bool toggle) override;

};