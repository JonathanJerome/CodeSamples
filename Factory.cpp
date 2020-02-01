#include "Factory.h"
#include "SerializationUnitTest.h"
#include "GUID.h"
#include "Input.h"
#include "InputStateType.h"
#include "Random.h"
#include "MathUtility.h"

Factory::Factory()
	:m_GOpool(nullptr)
{
}

Factory::~Factory()
{
}

void Factory::f_startup()
{
	m_GOpool = engine->m_meman.f_AddcomponentPool<GameObject>(DEFAULTPOOLSZ, "GameObject");
	m_compman.f_initialize();
	m_tagman.f_startup();
	m_arche;
}

void Factory::f_update()
{
	if (!m_tobedeleted.empty())
	{
		for (auto  elem : m_tobedeleted)
		{

			for (auto & comp : elem->m_components)
			{
				comp->f_destroy();
			}

			std::list<std::string> tmp; 
			for (const auto &elem2 : elem->m_scriptNames)
			{
				tmp.push_back(elem2);
			}

			for (auto & scr : tmp)
			{
				elem->f_removescript(scr);
			}

			RemoveFromVector(m_govec, elem);
			m_gomap.erase(elem->m_guid);
			m_GOpool->f_DeleteMem(elem);
		}

		m_tobedeleted.clear();
	}
}

void Factory::f_reboot()
{
	//Flag all compnents for destruction
	for (auto & elem : m_govec)
	{
		for (auto & comp : elem->m_components)
		{
			comp->f_destroy();
		}

		// call destructor for scripts as well
		std::list<std::string> tmp;
		for (const auto &elem2 : elem->m_scriptNames)
		{
			tmp.push_back(elem2);
		}
		if (tmp.size() != elem->m_scripts.size())
		{
			assert(false);
		}

		for (auto & scr : tmp)
		{
			elem->f_removescript(scr);
		}

        //delete elem;
		m_GOpool->f_DeleteMem(elem);
	}

	m_redoUndo.Purge2();

	m_govec.clear();
	m_gomap.clear();
	
	//engine->m_meman.f_DeleteComponentPool(m_GOpool->f_GetID());
	//m_GOpool = engine->m_meman.f_AddcomponentPool<GameObject>(DEFAULTPOOLSZ);
}

void Factory::f_shutdown()
{
	f_reboot();
	m_compman.f_shutdown();
	m_arche.f_shutdown();
	m_tagman.f_shutdown();
}

int Factory::f_initialize()
{
    return 0;
}

int Factory::f_reinitialize()
{
	return 0;
}

int Factory::f_load()
{
	//UnitTestLoad();
	return 0;
}

int Factory::f_save()
{
	//UnitTestSave();
	return 0;
}

int Factory::f_imgui()
{
	return 0;
}

GameObject * Factory::f_creatego(vec3 pos)
{
	guid newborn = GUIDGenerator::Generate();

	GameObject* bastardChild = m_GOpool->f_GetComponentMem<GameObject>(newborn);
	bastardChild->m_pos = pos;
	bastardChild->tempPos = pos;

	m_govec.push_back(bastardChild);
	m_gomap.insert(std::make_pair(newborn, m_govec.back()));
	return m_gomap[newborn];
}

GameObject * Factory::f_creatego(guid id)
{
	guid newborn = id;
	//m_govec.push_back(new GameObject{ newborn });
	m_govec.push_back(m_GOpool->f_GetComponentMem<GameObject>(newborn));
	m_gomap.insert(std::make_pair(newborn, m_govec.back()));
	return m_gomap[newborn];
}

GameObject * Factory::f_creatego(vec3 p, vec3 s, vec3 r)
{
	guid newborn = GUIDGenerator::Generate();
	//m_govec.push_back(new GameObject{ newborn });
	m_govec.push_back(m_GOpool->f_GetComponentMem<GameObject>(newborn));
	m_gomap.insert(std::make_pair(newborn, m_govec.back()));

	m_gomap[newborn]->f_SetPos(p);
	m_gomap[newborn]->f_SetSca(s);
	m_gomap[newborn]->f_SetRot(r);

	mat3 tmp = Rotation3x3(
		RAD2DEG(r.x),
		RAD2DEG(r.y),
		RAD2DEG(r.z));

	
	return m_gomap[newborn];
}

int Factory::f_removego(GameObject * go)
{
	assert(go != nullptr);
    for (const auto & elem : m_tobedeleted)
    {
        if (elem == go)
            return 0;
    }
	m_tobedeleted.push_back(go);
    if (getsys(ImGuiEditor)->m_selected != nullptr && getsys(ImGuiEditor)->m_selected->m_guid == go->m_guid)
        getsys(ImGuiEditor)->m_selected = nullptr;


	return 1;
}

int Factory::f_removego(AbstractGameObject * go)
{
	return f_removego(static_cast<GameObject*>(go));
}

bool Factory::GetPause()
{
	return engine->m_GamePaused;
}

bool Factory::GetOptionPressed()
{
	return getsys(GameLogic)->OptionPressed;
}

bool Factory::GetIfGameOver()
{
	return getsys(GameLogic)->GameOver;
}

void Factory::setGameOver(bool b)
{
	getsys(GameLogic)->GameOver = b;
}

void Factory::SetIntransition(bool b)
{
	getsys(GameLogic)->InTransition = b;
}

AbstractGameObject * Factory::GetMseHoverGO()
{
	vec2 mousepos = g_Input->GetMousePosition();
	return static_cast<AbstractGameObject*>(getsys(GLUN::Graphics)->PickingSreen((int)mousepos.x, (int)mousepos.y));
}

void Factory::f_flagchangeoflevel(std::string level)
{
	engine->m_lvs.m_currlevel = level;
	engine->m_scriptchangelevel = true;
	engine->m_scriptloadcontinuous = true;
	engine->m_InLoadingScreen = false;
}

void Factory::f_flagLoadingScreen(std::string level)
{
	engine->m_lvs.m_currlevel = level;
	engine->m_scriptchangelevel = true;
	engine->m_scriptloadcontinuous = true;
	engine->m_loadingscreen = true;
	engine->m_InLoadingScreen = true;
}

std::vector<AbstractGameObject*> Factory::GetAllGameObjects()
{
	std::vector<AbstractGameObject*> go;
	for (auto& elem : m_govec)
	{
		go.push_back(elem);
	}

	return go;
}

AbstractCamera * Factory::ffs_GetMainCamera()
{
	return getsys(GLUN::Graphics)->GetCamera();
}

AbstractGameObject * Factory::ffs_getObjectByGUID(guid input)
{
	auto test = m_gomap.find(input);

	if (test != m_gomap.end())
	{
		return test->second;
	}

	return nullptr;
}

AbstractGameObject * Factory::ffs_CloneObjectByGUID(guid object)
{
	auto test = m_gomap.find(object);
	GameObject* tmpobj = nullptr;

	if (test != m_gomap.end())
	{
		tmpobj = m_gomap[object]->f_clone();
	}

	return tmpobj;
}

std::string Factory::f_getCurrentLvl()
{
	return engine->m_lvs.m_currlevel;
}

GameObject * Factory::findName(const std::string & name)
{
	for (auto & elem : m_govec)
	{
		if (elem->f_getName().compare(name) == 0)
			return elem;
	}

	return nullptr;
}

void Factory::f_ToggleAnimCull(bool toggle)
{
	getsys(GLUN::Graphics)->bCull = toggle;
}
