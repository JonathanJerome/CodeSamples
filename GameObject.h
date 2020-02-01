/*Start Header*****************************************************************/
/*!
\file	GameObject.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Oct 09, 2018>
\brief	Until further research is done, we will follow a GameObject orientated,
		component based engine architecture

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/
#pragma once

#include <assert.h>

//Engine
#include "MathUtility.h"
#include "SerializationWrapper.h"

//Components
#include "Component.h"

// abstract class
#include "AbstractGameObject.h"

// scripts
#include "ScriptObject.h"

// property system
#include "PropertySystem.h"

#include "Geometry.h"

//STL
#include <string>
#include <list>
#include <utility>
#include <algorithm>


//Old Commments from FireChickenNumberOne
//The GameObject Class, is the foundation for all dynamic and static objects that will populate our game world.
//A GameObject has a vector of components which gives that object its properties <Sprite, Collider, Mass>
//All GameObjects will be managed by the GameObject Factory
//GameObjects will be unserialized from .xml files by the factory and created by the factory for each scene<Level>

class Factory;

class GameObject : public AbstractGameObject
{
private:
    //dragfloat manipulate?
	bool floatM = false;
	//imgui manipulating?
    bool im = false;

	//To check if it's interactable
	bool isInteractable;

    //imgui temp vars
    vec3 tempPos;
    vec3 tempRot;
    vec3 tempSca;

public:
    //Components
    std::list<Component*> m_components;
	//scripts
	std::list<ScriptObject*> m_scripts;
	std::list<std::string> m_scriptNames;

    //Transferable Data
    vec3 m_pos;
    vec3 m_rot;
    vec3 m_sca;

    mat3 m_orientation;

	OBB OcclusionCollider;

    std::string m_name;
    unsigned int m_tag;
    unsigned int m_archetype_status;

    //Unique Data 
    guid		m_guid;

    //Constructors
	GameObject();
    GameObject(guid name);
    ~GameObject();

    int f_imgui() override;
    int f_save(XDocument doc, XNode node);
    int f_load(XDocument doc, XNode node);


    vec3 & f_GetPos() override;
    vec3 & f_GetSca() override;
    vec3 & f_GetRot() override;

    mat3 & f_GetOrientation() override;

    void f_SetPos(const vec3 & pos) override;
    void f_SetSca(const vec3 & sca) override;
    void f_SetRot(const vec3 & rot) override;

    void f_SetOrientation(const mat3 & o) override;

	void setOcclusionTransform();

    //Not done
    mat4 f_gettransform() override;	//returns model matrix						

	//To Check if it is good or not
	void f_SetIsInteractable(const bool&);

	bool f_GetIsInteractable();

    //Not done
    GameObject* f_clone() override;	//returns new game object with exact copies of components 

    //FUNCTION TEMPLATES
    //Add component
    template <typename T>
    T* f_addcomp(T* com)
    {
        assert(com != nullptr);	//Cannot insert nullptr to list of components
        com->m_Owner = this;
        for (auto & elem : m_components)
        {
            if (typeid(*elem) == typeid(*com))//Component already exsists
            {
                com->f_destroy();
                return nullptr;
            }

        }

        m_components.push_back(com);
        return com;
    }


    //Remove component
    template <typename T>
    int f_removecomp(T* com)
    {
        assert(com != nullptr);

        Component * todelete = nullptr;
        for (auto & elem : m_components)
        {
            if (typeid(*elem) == typeid(*com))
            {
                todelete = elem;
                break;
            }
        }

        if (todelete == nullptr)
        {
            return 2; //Component does not exsist
        }

        m_components.erase(std::remove_if(m_components.begin(), m_components.end(), [&](const Component * c)
        {
            return typeid(*c) == typeid(*com);
        }), m_components.end());

        todelete->f_destroy();

        return 0;
    }

    //Get component
    template <typename T>
    T* f_getcomp()
    {
        for (auto * elem : m_components)
        {
            if (typeid(*elem) == typeid(T))
            {
                return static_cast<T*>(elem);
            }
        }

        return nullptr; //Component does not exist
    }

	// assign script
	ScriptObject* f_assignscript() override;

    ScriptObject* f_assignscript(std::string name, bool active = true) override;
	void f_removescript(std::string name) override;

	// for scriptcomponents
	std::list<AbstractComponent*> GetComponents() override;
	AbstractComponent* GetComponent(ComponentType type) override;

	std::list<ScriptObjectExport*> GetScripts() override;
	ScriptObjectExport* GetScript(std::string) override;

    // save test
    void f_redoundosave(std::vector<xProperties::ComplexxType>& container);
	void f_redoundostate(std::vector<xProperties::ComplexxType>& container);

    void f_redoundosave2(std::vector<xProperties::xLazyType>& container);
    void f_redoundostate2(std::vector<xProperties::xLazyType>& container);

	// gettor and settor for name and tag
	std::string f_getName() override;
	void f_setName(std::string name) override;
	std::string f_getTag() override;
	void f_setTag(std::string name) override;

	guid GetGuid() override;
	
	vec3 ForwardDirection() override;
	void SetForward(const vec3 & target, const vec3 & me);
	void SetForwardTrue(const vec3 & target, const vec3 & me);

	//Friend Declaration
	friend class Factory;
	friend class Component;
	friend class ArchetypeManager;

};
