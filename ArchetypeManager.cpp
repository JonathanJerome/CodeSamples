/*Start Header*****************************************************************/
/*!
\file	ArchetypeManager.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Jan 10, 2019>
\brief	Archetype Manager keeps track of all archetypes.
		Archetype class stores data of GameObject template

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/
#include "ArchetypeManager.h"
#include "GameObject.h"
#include "CoreEngine.h"

namespace fs = std::experimental::filesystem;

ArchetypeManager::ArchetypeManager()
	: m_archeid(1), m_archpool(nullptr)
{
	//Note, ID 0 means unique GameObject
}

ArchetypeManager::~ArchetypeManager()
{
}

unsigned int ArchetypeManager::f_create_archetype(GameObject * source, std::string name)
{
	bool cloneable = true;	//Is Game Object cloneable? -> Did all programmers overload f_gentemplate();
	//Template GameObject data:
	//Archetype* arch = new Archetype{};
	Archetype* arch = m_archpool->f_GetComponentMem<Archetype>();

	arch->m_name	= source->m_name;
	arch->m_tag		= source->m_tag;
	arch->m_archetype_status = (++m_archeid); //Assign new archetype ID
	arch->m_rot		= source->m_rot;
	arch->m_sca		= source->m_sca;
	arch->m_orientation = source->m_orientation;
	
	//Template each component's data
	for (const auto & elem : source->m_components)
	{
		Component * newcomp = elem->f_gentemplate();
		if (newcomp == nullptr)
		{
			cloneable = false;
			break;
		}
		arch->m_components.push_back(newcomp);
	}


	for (const auto & elem : source->m_scripts)
	{
		arch->m_scripts.push_back(elem->GetName());
	}

	
	for (const auto& elem : m_arche_names) //Double Check for repeat names
	{
		if (elem.second.compare(name) == 0)
			cloneable = false;
	}


	//If not cloneable (delete memory allocated, print out UNCLONEABLE
	if (cloneable == false)
	{
		//for (auto elem : arch->m_components) - This is now done in Archetype destructor
		//{
		//	delete elem;
		//}
		//delete arch;
		m_archpool->f_DeleteMem(arch);
		std::cout << "Error Archetype: " << name << " not clone-able, NO archetype created" << std::endl;
		return 1;
	}

	m_arche_names.insert( std::make_pair(arch->m_archetype_status, name) );	
	m_templates.insert(	std::make_pair(arch->m_archetype_status, arch) );
	std::cout << "Archetype: " << name << " created successfully" << std::endl;

	//f_save();
	f_save_specific(arch->m_archetype_status);
	return arch->m_archetype_status; //Return Archetype status ID

}

int ArchetypeManager::f_remove_archetype( unsigned int name )
{
	//Remove from map
	auto arche = m_templates.find(name);
	if (arche != m_templates.end())
	{
		Archetype* tmp = arche->second;
		/*for (auto& elem : tmp->m_components) - This is now done in Archetype destructor
		{
			delete elem;
		}*/
		for (auto & elem : getsys(Factory)->m_govec) //Remember, now all copies are unique, NO unifiying Master copy
		{
			if (elem->m_archetype_status == name)
				elem->m_archetype_status = 0;
		}
		
		//Remove from vector
		m_arche_names.erase(name);
		m_templates.erase(name);
		//delete tmp;
		m_archpool->f_DeleteMem(tmp);
		return 0;
	}
	else
	{
		std::cout << "Error Archetype: " << name << " not found" << std::endl;
		return 1;
	}
}

int ArchetypeManager::f_instantiate_archetype(unsigned int name, vec3 pos)
{
	auto arche = m_templates.find(name);
	
	if (arche != m_templates.end())
	{
		Archetype * tmp = arche->second;
		//Create GameObject at position
		GameObject * instant = getsys(Factory)->f_creatego(pos);
		//instant->m_pos = pos;
		instant->m_rot = tmp->m_rot;
		instant->m_sca = tmp->m_sca;
		//Change GameObject data
		instant->m_name = tmp->m_name;
		instant->m_tag = tmp->m_tag;
		instant->m_archetype_status = tmp->m_archetype_status;
		instant->m_orientation = tmp->m_orientation;
		//Clone components

		for (auto & elem : tmp->m_components)
		{
			instant->f_addcomp(elem->f_clone());
		}
		for (auto & scrp : tmp->m_scripts)
		{
			instant->f_assignscript(scrp);
		}
	}
	else
	{
		std::cout << "Error Archetype: " << name << " not found" << std::endl;
		return 1;
	}
	return 0;
}

AbstractGameObject* ArchetypeManager::f_instantiate_archetypeObj(unsigned int name, vec3 pos)
{
    if (name == 0)
    {
        std::cout << "Error Archetype: " << name << " not found" << std::endl;
        return nullptr;
    }

	auto arche = m_templates.find(name);
	GameObject* instant = nullptr;
	if (arche != m_templates.end())
	{
		Archetype * tmp = arche->second;
		//Create GameObject at position
		instant = getsys(Factory)->f_creatego(pos);
		instant->m_pos = pos;
		instant->m_rot = tmp->m_rot;
		instant->m_sca = tmp->m_sca;
		//Change GameObject data
		instant->m_name = tmp->m_name;
		instant->m_tag = tmp->m_tag;
		instant->m_archetype_status = tmp->m_archetype_status;
		instant->m_orientation = tmp->m_orientation;
		//Clone components

		for (auto & elem : tmp->m_components)
		{
			instant->f_addcomp(elem->f_clone());
		}
		for (auto & scrp : tmp->m_scripts)
		{
			instant->f_assignscript(scrp);
		}
	}
	else
	{
		std::cout << "Error Archetype: " << name << " not found" << std::endl;
		return nullptr;
	}
	return static_cast<AbstractGameObject*>(instant);
}

int ArchetypeManager::f_update_archetype(const unsigned int name, GameObject* updater)
{
	//Check if archetype exsists
	auto arche = m_templates.find(name);
	if (arche != m_templates.end())
	{
		std::vector<Vector3> positions;
		for (auto & elem : getsys(Factory)->m_govec)
		{
			if (elem == updater)
				continue;
			if (elem->m_archetype_status == arche->second->m_archetype_status)
			{
				//STRONG NOTE: very hackish way to update archetypes, GET THE PROPERTIES SYSTEM WORKING PLS!
				/************************************************************************/
				positions.push_back(elem->m_pos);
				getsys(Factory)->f_removego(elem);
				/************************************************************************/
			}
		}

		for (const auto & elem : positions)
		{
			f_instantiate_archetype(arche->second->m_archetype_status, elem);
		}
		
		
		return 0;
	}
	else
	{
		std::cout << "Error Archetype: " << name << " not found" << std::endl;
		return 1;
	}


}

int ArchetypeManager::f_update_masterCopy(const unsigned int name, GameObject * updater)
{
	//Get archetype
	auto arche = m_templates.find(name);
	if (arche != m_templates.end())
	{
		auto arch = arche->second;
		//Update main Data
		arch->m_name = updater->m_name;
		arch->m_tag = updater->m_tag;
		arch->m_archetype_status = name; //Assign new archetype ID
		arch->m_rot = updater->m_rot;
		arch->m_sca = updater->m_sca;
		arch->m_orientation = updater->m_orientation;
		//Delete all previous components
		//for (auto & elem : arch->m_components)
		//{
		//	// memory will be deleted by MM
		//	delete elem;
		//}
		arch->m_components.clear();
		arch->m_scripts.clear();

		//Clone all new components
		for (auto & elem : updater->m_components)
		{
			Component * newcomp = elem->f_gentemplate();
			assert(newcomp != nullptr);
			arch->m_components.push_back(newcomp);
		}
		for (auto & elem : updater->m_scripts)
		{
			arch->m_scripts.push_back(elem->GetName());
		}
	}
	else
	{
		std::cout << "Error Archetype: " << name << " not found" << std::endl;
		return 1;
	}
	//f_save();
	f_save_specific(name);
	
	return 0;
}

int ArchetypeManager::f_save()
{
	//Save current ID of Archetype
	tinyxml2::XMLDocument m_Doc;
	XDocument doc = &m_Doc;
	XNode levelroot = doc->NewElement("ArchetypesID");
	levelroot->SetAttribute("ArcheID", m_archeid);
	doc->InsertFirstChild(levelroot);
	if (m_Doc.SaveFile("ArchetypesIDNumber.xml") == tinyxml2::XMLError::XML_SUCCESS)
	{
		std::cout << "Archetypes ID Save Successful" << std::endl;
	}
	else
	{
		std::cout << "Archetypes ID Save Failed" << std::endl;
	}
	doc->~XMLDocument();
	
	//return f_save_new();
	////Call save on all archetypes to XML
	//tinyxml2::XMLDocument m_Doc;
	//XDocument doc = &m_Doc;

	//XNode levelroot = doc->NewElement("Archetypes");

	//levelroot->SetAttribute("ArcheID", m_archeid);

	////Save each game object inside GO vector
	//for (const auto& elem : m_arche_names)
	//{
	//	if (elem.first == 0)//0 - id is unique, not for archetype
	//		continue;
	//	
	//	XNode go = doc->NewElement("Template");				//1. Create new node for the archetype
	//	x_savestring(doc, go, "archename", elem.second);	//2. Save the name of the archetype
	//	m_templates[elem.first]->f_save(doc, go);			//3. Save the rest of the archetype data

	//	levelroot->InsertEndChild(go);
	//}


	//doc->InsertFirstChild(levelroot);

	//if (m_Doc.SaveFile("Archetypes.xml") == tinyxml2::XMLError::XML_SUCCESS)
	//{
	//	std::cout << "Archetypes Save Successful" <<std::endl;
	//	return 0;
	//}
	//else
	//{
	//	std::cout << "Archetypes Save Failed" << std::endl;
	//	return 1;
	//}
	//return 0;
	return 0;
}

int ArchetypeManager::f_load()
{
	return f_load_new(); 
	//-New load with indiviual .xml files for each archetype
	////Load all archetypes from XML
	//tinyxml2::XMLDocument m_Doc;
	//XDocument doc = &m_Doc;

	//if (m_Doc.LoadFile("Archetypes.xml") == tinyxml2::XMLError::XML_SUCCESS)
	//{
	//	XNode root = doc->FirstChildElement("Archetypes");

	//	if (root == nullptr)
	//	{
	//		std::cout << "Archetypes Load Failed" << std::endl;
	//		return 1;
	//	}
	//	root->QueryUnsignedAttribute("ArcheID", &m_archeid);

	//	XNode go = root->FirstChildElement("Template");
	//	
	//	while (go != nullptr)
	//	{
	//		//Create Archetype
	//		//Archetype * obj = new Archetype{};
	//		Archetype * obj = m_archpool->f_GetComponentMem<Archetype>();
	//		//Load Archetype
	//		std::string tmpname;
	//		x_loadstring(doc, go, "archename", tmpname);								//1. Get the name of archetype
	//		obj->f_load(doc, go);														//2. Load the data for archetype
	//		m_arche_names.insert(std::make_pair(obj->m_archetype_status, tmpname));		//3. Make pair of ID/Name
	//		m_templates.insert(std::make_pair(obj->m_archetype_status, obj));			//4. Make pair of ID/Local Copy
	//		//Iterate through XML
	//		go = go->NextSiblingElement();
	//	}
	//}
	//else
	//{
	//	std::cout << "Load Failed Archetypes.xml " << std::endl;
	//}
	return 0;
}

int ArchetypeManager::f_save_specific(const unsigned int name)
{
	//Save current ID of Archetype
	tinyxml2::XMLDocument m_Doc;
	XDocument doc = &m_Doc;
	XNode levelroot = doc->NewElement("ArchetypesID");
	levelroot->SetAttribute("ArcheID", m_archeid);
	doc->InsertFirstChild(levelroot);
	if (m_Doc.SaveFile("ArchetypesIDNumber.xml") == tinyxml2::XMLError::XML_SUCCESS)
	{
		std::cout << "Archetypes ID Save Successful" << std::endl;
	}
	else
	{
		std::cout << "Archetypes ID Save Failed" << std::endl;
	}
	doc->~XMLDocument();

	auto arche = m_templates.find(name);

	tinyxml2::XMLDocument arche_doc;
	XDocument doc_addr = &arche_doc;
	XNode go = doc_addr->NewElement("Template");								//1. Create new node for the archetype
	x_savestring(doc_addr, go, "archename", m_arche_names[arche->first]);		//2. Save the name of the archetype
	m_templates[arche->first]->f_save(doc_addr, go);							//3. Save the rest of the archetype data
	doc_addr->InsertFirstChild(go);
	if (arche_doc.SaveFile(std::string("Archetypes\\" + m_arche_names[arche->first] + ".xml").c_str()) == tinyxml2::XMLError::XML_SUCCESS)
	{
		std::cout << "Archetype: " << m_arche_names[arche->first] << " Save Successful" << std::endl;
	}
	else
	{
		std::cout << std::string("Archetypes/" + m_arche_names[arche->first]).c_str() << " Load Failed" << std::endl;
	}
	return 0;
}

int ArchetypeManager::f_save_new()
{
	//Save current ID of Archetype
	tinyxml2::XMLDocument m_Doc;
	XDocument doc = &m_Doc;
	XNode levelroot = doc->NewElement("ArchetypesID");
	levelroot->SetAttribute("ArcheID", m_archeid);
	doc->InsertFirstChild(levelroot);
	if (m_Doc.SaveFile("ArchetypesIDNumber.xml") == tinyxml2::XMLError::XML_SUCCESS)
	{
		std::cout << "Archetypes ID Save Successful" << std::endl;
	}
	else
	{
		std::cout << "Archetypes ID Save Failed" << std::endl;
	}
	doc->~XMLDocument();
	//For each of the archetypes in the system, save it into its separate .xml
	for (const auto& elem : m_arche_names)
	{
		if (elem.first == 0) //0 - id is unique, not for archetype
			continue;
		tinyxml2::XMLDocument arche_doc;
		XDocument doc_addr = &arche_doc;
		XNode go = doc_addr->NewElement("Template");				//1. Create new node for the archetype
		x_savestring(doc_addr, go, "archename", elem.second);		//2. Save the name of the archetype
		m_templates[elem.first]->f_save(doc_addr, go);				//3. Save the rest of the archetype data
		doc_addr->InsertFirstChild(go);
		if (arche_doc.SaveFile( std::string("Archetypes\\" + elem.second + ".xml").c_str() ) == tinyxml2::XMLError::XML_SUCCESS)
		{
			//std::cout << "Archetype: " << elem.second << " Save Successful" << std::endl;
		}
		else
		{
			std::cout << "Archetype: " << elem.second << " Save Failed" << std::endl;
		}


	}
	return 0;
}


int ArchetypeManager::f_load_new()
{

	//Load current ID of Archetype
	tinyxml2::XMLDocument m_Doc;
	XDocument doc = &m_Doc;
	if (m_Doc.LoadFile("ArchetypesIDNumber.xml") == tinyxml2::XMLError::XML_SUCCESS)
	{
		XNode root = doc->FirstChildElement("ArchetypesID");

		if (root == nullptr)
		{
			std::cout << "ArchetypesIDNumber Load Failed" << std::endl;
			return 1;
		}
		root->QueryUnsignedAttribute("ArcheID", &m_archeid);
	}
	else
	{
		std::cout << "Load Failed ArchetypesIDNumber.xml " << std::endl;
	}
	doc->~XMLDocument();

	//For each of the archetypes in the system, load it from its separate .xml
	for (auto& p : fs::directory_iterator("Archetypes"))
	{
		std::string tmp = p.path().filename().string();
		tinyxml2::XMLDocument arche_doc;
		XDocument doc_addr = &arche_doc;
		if (arche_doc.LoadFile(std::string("Archetypes/" + tmp).c_str()) == tinyxml2::XMLError::XML_SUCCESS)
		{
			XNode root = doc_addr->FirstChildElement("Template");

			if (root == nullptr)
			{
				std::cout << tmp.c_str() << " root Failed" << std::endl;
				continue;
			}
			//Create Archetype
			//Archetype * obj = new Archetype{};
			Archetype * obj = m_archpool->f_GetComponentMem<Archetype>();
			//Load Archetype
			std::string tmpname;
			x_loadstring(doc_addr, root, "archename", tmpname);							//1. Get the name of archetype
			obj->f_load(doc_addr, root);												//2. Load the data for archetype
			m_arche_names.insert(std::make_pair(obj->m_archetype_status, tmpname));		//3. Make pair of ID/Name
			m_templates.insert(std::make_pair(obj->m_archetype_status, obj));			//4. Make pair of ID/Local Copy
			std::cout << std::string("Archetypes/" + tmp).c_str() << " Load Successful" << std::endl;
		}
		else
		{
			std::cout << std::string("Archetypes/" + tmp).c_str() << " Load Failed" << std::endl;
		}

	}
	return 0;
}

unsigned int ArchetypeManager::f_sanitycheck(unsigned int id)
{
	auto arche = m_templates.find(id);
	if (arche != m_templates.end())
	{
		return id;
	}
	else
	{
		return 0;
	}
}

int ArchetypeManager::f_startup()
{
	m_archpool = engine->m_meman.f_AddcomponentPool<Archetype>(DEFAULTPOOLSZ, "Archetype");
	m_arche_names.insert(std::make_pair(0, "UNIQUE"));
	m_templates.insert(std::make_pair(0, nullptr));
	f_load();	//Load all acrhetypes and allocate memory for them
	return 0;
}

int ArchetypeManager::f_shutdown()
{
	f_save();								//1. Save all archetypes to XML
	for (const auto& elem : m_templates)	//2. Delete all memory allocated
	{
		if (elem.first == 0)
			continue;						//Careful! 0 is an ID to a nullptr
		//delete elem.second;
		m_archpool->f_DeleteMem(elem.second);
	}
	m_arche_names.clear();					//4. Clear map of ID/Names
	m_templates.clear();					//5. Clear map of ID/ Local copies

	return 0;
}

void ArchetypeManager::f_print_all_archetypes()
{
	std::ofstream file;
	std::ofstream file2;
	std::string filename = "ArchetypeReport.txt";
	std::string filename2 = "ArchetypeReport2.txt";
	file.open(filename, std::ios::out);
	file2.open(filename2, std::ios::out);

	if (!file.is_open() || !file2.is_open())
	{
		std::cout << "ArchetypeReport: FAIL to create" << std::endl;

	}
	else
	{
		for (const auto & elem : m_arche_names)
		{
			if (elem.first == 0)
				continue;
			file << elem.second << "\n";
			file2 << elem.first << "\n";
		}
	}
	file.close();
	file2.close();



}




Archetype::Archetype()
	:m_tag(0), m_archetype_status(0)
{
}

Archetype::~Archetype()
{
	//for(auto& elem: m_components)
	//{
	//	//memory will only be free on system shutdown
	//	delete elem;
	//}
	m_scripts.clear();
}

int Archetype::f_save(XDocument doc, XNode node)
{
	x_savevec3(doc, node, "rot", m_rot);
	x_savevec3(doc, node, "sca", m_sca);
	//Save mat3 m_orientation (function to save mat3 not implemented yet)
	x_savestring(doc, node, "name", m_name);
	x_saveuint(node, "tag", m_tag);
	x_saveuint( node, "Archetype", m_archetype_status);

	for (const auto& elem : m_components)
	{
		XNode go = doc->NewElement("Component");
		elem->f_save(doc, go);
		node->InsertEndChild(go);
	}
	XNode sp = doc->NewElement("Scripts");
	for (const auto & scrp : m_scripts)
	{
		XNode sps = doc->NewElement(scrp.c_str());

		sp->InsertEndChild(sps);
		node->InsertEndChild(sp);
	}
	return 0;
}

int Archetype::f_load(XDocument doc, XNode node)
{
	x_loadvec3(doc, node, "rot", m_rot);
	x_loadvec3(doc, node, "sca", m_sca);
	//Load mat3 m_orientation (function to load mat3 not implemented yet)
	x_loadstring(doc, node, "name", m_name);
	x_loaduint(node, "tag", m_tag);
	x_loaduint(node, "Archetype", m_archetype_status);


	//Call load function for all node
	XNode comps = node->FirstChildElement("Component");
	while (comps != nullptr)
	{
		//Load name of component
		std::string name;
		x_loadstring(doc, comps, "NAME", name);
		//1. create component
		Component* temp = getsys(Factory)->m_compman.f_createdef(name);
		Component* newcomp = temp->f_gentemplate();
		temp->f_destroy();
		//2. add it to go
		m_components.push_back(newcomp);
		//3. load component data
		newcomp->f_load(doc, comps);
		//4. iterate
		comps = comps->NextSiblingElement("Component");
	}
	XNode scripts = node->FirstChildElement("Scripts");
	if (scripts != nullptr)
	{
		XNode curr_script = scripts->FirstChildElement();
		while (curr_script != nullptr)
		{
			//Load name of Script
			m_scripts.push_back(curr_script->Name());
			//Iterate
			curr_script = curr_script->NextSiblingElement();
		}
	}

	return 0;
}

int Archetype::f_imgui()
{
	ImGui::PushID(m_archetype_status);
	ImGui::Text("ID number: %i", m_archetype_status);
	
	if(ImGui::Button("Find Archetype"))
	{
		//Button to find nearest Archetype
		for (auto & elem : getsys(Factory)->m_govec)
		{
			if (elem->m_archetype_status == m_archetype_status)
			{
				getsys(ImGuiEditor)->m_selected = elem;
				break;
			}
		}
	}
	
	if (ImGui::Button("Spawn Archetype"))
	{
		vec3 tmppos = getsys(GLUN::Graphics)->GetCamera()->GetPos();
		vec3 tmptar = getsys(GLUN::Graphics)->GetCamera()->GetTarget();

		vec3 testing = tmppos + (tmptar - tmppos);

		getsys(Factory)->m_arche.f_instantiate_archetype(m_archetype_status, { testing });
	}
	
	bool deleted = false;
	if (ImGui::Button("Delete Archetype"))
	{
		ImGui::OpenPopup("Delete Archetype");
		
	}

	if (ImGui::BeginPopupModal("Delete Archetype", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			getsys(Factory)->m_arche.f_remove_archetype(m_archetype_status);
			ImGui::CloseCurrentPopup();
			deleted = true;
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
		
	}

	if (deleted)
	{
		ImGui::PopID();
		return 1;
	}

	ImGui::PopID();
	return 0;

}
