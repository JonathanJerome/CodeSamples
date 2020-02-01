/*Start Header*****************************************************************/
/*!
\file	ArchetypeManager.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Jan 10, 2019>
\brief	Archetype Manager keeps track of all archetypes.
		Archetype class stores data of GameObject template 

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/
#pragma once



//STL
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>

//Engine
#include "Component.h"
#include "SerializationWrapper.h"

// for script side
#include "ScriptArchetypeManager.h"

#include "ScriptObject.h"

class MemoryPool;
class Factory;
class GameObject;

class Archetype
{
public:
	Archetype();		//Constructor
	~Archetype();		//Destructor -Deletes all memory from m_compoenents

	//Has all data the a GO has except for pos and guid, only recieves pos and GUID upon instantiation
	std::list<Component*> m_components;
	std::list<std::string> m_scripts;
	vec3 m_rot;
	vec3 m_sca;
	mat3 m_orientation;
	std::string m_name;
	unsigned int m_tag;
	unsigned int m_archetype_status;

	int f_save(XDocument doc, XNode node);
	int f_load(XDocument doc, XNode node);

	int f_imgui();
	
	friend class Component;
	friend class ArchetypeManager;

};



class ArchetypeManager : public ScriptArchetypeManager
{
	unsigned int m_archeid;

	MemoryPool * m_archpool;

public:
	ArchetypeManager();		//Constructor
	~ArchetypeManager();	//Destructor


	//returns 0 if successful, 1 if failure
	unsigned int f_create_archetype(GameObject * source, const std::string name);
	int f_remove_archetype(const unsigned int name);
	int f_instantiate_archetype(const unsigned int name, vec3 pos) override;
	AbstractGameObject* f_instantiate_archetypeObj(const unsigned int name, vec3 pos) override;
	int f_update_archetype(const unsigned int name, GameObject* updater = nullptr);
	int f_update_masterCopy(const unsigned int name, GameObject* updater);

	int f_save();
	int f_load();

	//Ivan's Genius Idea, make each Archetype a separate .xml so that they are not easily overrided
	int f_save_specific(const unsigned int name);
	int f_save_new();
	int f_load_new();
	//Save Specific function makes it more optimal to save specific archetypes. --Put on hold until absolutely necessary
	//int f_save_specific();


	unsigned int f_sanitycheck(unsigned int id); //Sanity check to see if reference to archetype still exsists

	int f_startup();
	int f_shutdown();

	void f_print_all_archetypes(); //Print all archetypes into Archetype Report.txt


	void test() override { std::cout << "anyhow" << std::endl; }
	//std::vector<std::string> m_archenames;/*REDUDANT- To be deleted*/
	
	
	std::unordered_map< unsigned int, std::string> m_arche_names;		//Each archetype is represented by an unsigned int value (ID) and std::string (Name)
	std::unordered_map<	unsigned int, Archetype*> m_templates;			//m_templates -is responsible for storing copies of each archetype during run time for easy cloning.


	friend class Factory;
};