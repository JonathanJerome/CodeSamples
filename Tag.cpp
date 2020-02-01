#pragma once
#include <iostream>
#include "Tag.h"
#include "SerializationWrapper.h"

TagManager::TagManager()
	:	m_tagids(3)
{
}

TagManager::~TagManager()
{
}

int TagManager::f_save()
{
	tinyxml2::XMLDocument m_Doc;
	XDocument doc = &m_Doc;

	XNode levelroot = doc->NewElement("Tags");
	levelroot->SetAttribute("TagsID", m_tagids);

	for (const auto & elem : m_tags)
	{
		XNode go = doc->NewElement("Tag");
		x_saveuint(go, "TagNum", elem.first);
		x_savestring(doc, go, "TagName", elem.second);
		levelroot->InsertEndChild(go);
	}

	doc->InsertFirstChild(levelroot);
	if (m_Doc.SaveFile("Tags.xml") == tinyxml2::XMLError::XML_SUCCESS)
	{
		std::cout << "Tags Save Successful" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Tags Save Failed" << std::endl;
		return 1;
	}

	return 0;
}

int TagManager::f_load()
{
	tinyxml2::XMLDocument m_Doc;
	XDocument doc = &m_Doc;

	if (m_Doc.LoadFile("Tags.xml") == tinyxml2::XMLError::XML_SUCCESS)
	{
		XNode root = doc->FirstChildElement("Tags");
		if (root == nullptr)
		{
			std::cout << "Tags Load Failed" << std::endl;
			return 1;
		}
		root->QueryUnsignedAttribute("TagsID", &m_tagids);

		XNode go = root->FirstChildElement("Tag");
		while (go != nullptr)
		{
			unsigned int tmpnum;
			std::string tmpname;
			x_loaduint(go, "TagNum", tmpnum);
			x_loadstring(doc, go, "TagName", tmpname);
			m_tags.insert(std::make_pair(tmpnum, tmpname));
			go = go->NextSiblingElement();
		}

	}
	else
	{
		std::cout << "Load Failed Tags.xml " << std::endl;
	}
	return 0;
}

void TagManager::f_startup()
{
	m_tags.insert(std::make_pair(0, "DEFAULT"));
	m_tags.insert(std::make_pair(1, "FLOOR"));
	m_tags.insert(std::make_pair(2, "ENEMY"));
	f_load();
	f_updatetag();
}

void TagManager::f_shutdown()
{
	f_save();
	m_tags.clear();
}

void TagManager::f_updatetag()
{
	m_tagnames.clear();
	for (const auto& elem : m_tags)
	{
		m_tagnames.push_back(elem.second);
	}
	m_tags.clear();
	int i = 0;
	for (const auto& elem : m_tagnames)
	{
		m_tags.insert(std::make_pair(i,elem));
		++i;
	}
	m_tagids = i;

}

void TagManager::f_addtag(std::string tag)
{
	m_tags.insert(std::make_pair(m_tagids, tag));
	f_updatetag();
}

void TagManager::f_removetag(unsigned int  id)
{
	m_tags.erase(id);
	f_updatetag();
}
