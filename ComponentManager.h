#pragma once
/*Start Header*****************************************************************/
/*!
\file	ComponentManager.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Feb 01, 2019>
\brief	The Component Manager is responsible for:
		1. Registering all avaliable types of components (Keep a map)
		2. Register create/add component functions


Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/


//STL libraries
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <cassert>

//Engine libraries
#include "MathUtility.h"
#include "Component.h"

#define ComponentName(NAME) typeid(NAME).name()
#define Stringify(NAME) #NAME
#define regComponent(NAME , FUNCTION)  m_componentnames.push_back(ComponentName(NAME));  m_createdefault.insert( std::make_pair(ComponentName(NAME), &(FUNCTION)) )


class ComponentManager
{
private:
	std::map<std::string, std::function<Component*()>> m_createdefault;		//Map that maps Component Name to function that accesses system to create that component
	std::map<unsigned int, std::string> m_poolmap;							//Map to memory pool ID nums
public:

	std::vector<std::string> m_componentnames;

	void f_initialize();
	void f_shutdown();

	

	//Create component by type
	template<typename T>
	Component* f_createdef()
	{
		//use typeid to get name to call function pointer to return component *
		auto function = m_createdefault.find(ComponentName(T));
		assert(function != m_createdefault.end());
		return function->second();
	}

	//Create component by name, must be same as class
	Component* f_createdef(const std::string& name)
	{
		auto function = m_createdefault.find(name);
		assert(function != m_createdefault.end());
		return function->second();
	}

};