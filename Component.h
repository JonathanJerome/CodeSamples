/*Start Header*****************************************************************/
/*!
\file	BaseComponent.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Oct 09, 2018>
\brief	BaseComponent for inheritance to achieve component based engine architecture

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/

#pragma once

#include "SerializationWrapper.h"								//Component's data needs to be serialized into XML
//#include "GameObject.h"
#include "PropertySystem.h"
#include "AbstractComponent.h"
#include <vector>

class GameObject; 												//Each component will always be attached to a GameObject
class Archetype;												//Archetypes will also store components

//Container | Variable name | data value (can be any temp var) | variable to save | this class type | this object's guid
#define regComp(VECTOR, VARNAME, DATAVALUE, DATALOCATION, CLASS, OBJGUID) VECTOR.push_back(xLazyPropertyData<CLASS>{VARNAME, (int)offsetof(CLASS, DATALOCATION), DATAVALUE, OBJGUID});

class Component : public AbstractComponent
{
private:
	
	GameObject * m_Owner = nullptr;								//Pointer to the owner of this Component: The GameObject

public:

	friend class GameObject;									//So the GameObject can touch the Component's privates

	virtual void f_startup()						= 0;		//(overide) called once on component creation
	virtual void f_initialize()						= 0;		//(overide) called once after load
	virtual void f_destroy()						= 0;		//(overide) called once before component deletion
	
	virtual int f_imgui()							= 0;		//(override) called for ImGUI window to edit component data
	virtual int f_save(XDocument doc, XNode node)	= 0;		//(override) save data to  XML
	virtual int f_load(XDocument doc, XNode node)	= 0;		//(override) load datafrom XML

	virtual Component* f_gentemplate()				= 0;		//(override) Similar to f_clone() but System relinquishes control of component. (i.e. not stored in System's container, NOT updated) 


	virtual Component * f_clone() = 0;							//Clone must be pure virtual
	GameObject * f_getowner() { return m_Owner; }				//Get the owner of this Component

    
	//Ivan, what is this? - Jonathan
    //redo undo function _ save (...);

    //An experiment to see if it is possible to save without getters/setters.
    //To idiot proof the process
    virtual void ruf_save2(std::vector<xProperties::xLazyType>& container);
    virtual void ruf_saveState2(std::vector<xProperties::xLazyType>& container);
};

