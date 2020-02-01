/*Start Header*****************************************************************/
/*!
\file	Tag.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Jan 10, 2019>
\brief	Tag system will replace (tag == string) with (tag == unsigned int) && (unsigned int -> TagSystem(string))

Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/
#pragma once
#include <map>
#include <vector>
#include <string>

class TagManager
{
private:
	unsigned int m_tagids;


public:
	std::map<unsigned int, std::string> m_tags;
	std::vector<std::string> m_tagnames;
	TagManager();	//Constructor
	~TagManager();	//Destructor

	//Save
	int f_save();
	//Load
	int f_load();

	//Startup
	void f_startup();

	//Shutdown
	void f_shutdown();


	//Add tag
	void f_updatetag();
	//Remove tag
	void f_addtag( std::string tag);
	void f_removetag(unsigned int id);


};