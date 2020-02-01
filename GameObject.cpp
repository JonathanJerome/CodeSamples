/*Start Header*****************************************************************/
/*!
\file	GameObject.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Jan 10, 2019>
\brief	Implementation for GameObject specific functions and member functions

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/
#pragma once
#include "GameObject.h"

//engine pointer
#include "CoreEngine.h"

//Imgui
#include "ImGuizmo/ImGuizmo.h"
#include "ImGui/imgui.h"

#include "RedoUndo.h"

#include <cmath>
#include <cfloat>



GameObject::GameObject()
	:m_pos(0.0f), m_rot(0.0f), m_sca(1.0f, 1.0f, 1.0f), m_orientation(), m_name("default"), m_tag(0), m_archetype_status(0), m_guid(0), im{ false }, isInteractable{false}
{
}

GameObject::GameObject(guid name)
    : m_pos(0.0f), m_rot(0.0f), m_sca(1.0f, 1.0f, 1.0f), m_orientation(), m_name("default"), m_tag(0), m_archetype_status(0), m_guid(name), im{ false }, isInteractable{false}
{

}

GameObject::~GameObject()
{
}

int GameObject::f_imgui()
{

    vec3 tempPos2 = m_pos;
    vec3 tempSca2 = m_sca;
    vec3 tempRot2 = m_rot;

    static bool closable = true;

    static char ReName[64] = "";
    int ReTag = 0;

    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    static float fakerot[3] = { 0.f,0.f,0.f };

    /*if (m_rot.x > 360.0f || m_rot.x < -360.0f)
        m_rot.x = 0.f;
    if (m_rot.y > 360.0f || m_rot.y < -360.0f)
        m_rot.y = 0.f;
    if (m_rot.z > 360.0f || m_rot.z < -360.0f)
        m_rot.z = 0.f;*/

    


    if (getsys(ImGuiEditor)->m_selecteds.size() > 1)
    {
        ImGui::Text("MULTIPLE OBJECTS SELECTED!");
        ImGui::Separator();

        if (ImGui::IsAnyItemHovered() == false)
        {
            if (ImGui::IsKeyPressed('Q'))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            //if (ImGui::IsKeyPressed('E'))
            //    mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed('E'))
                mCurrentGizmoOperation = ImGuizmo::SCALE;
        }
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        //if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        //    mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;


        mat4 m_transform{};

        if (ImGuizmo::IsUsing() && !im)
        {
            tempPos = m_pos;
            tempRot = m_rot;
            tempSca = m_sca;

            im = true; //is manipulating
        }
        else if (!ImGuizmo::IsUsing() && im)
        {
            //finish using imguizmo, save previous states

            if (tempPos != m_pos)
            {
                /*getsys(Factory)->m_redoUndo.RegisterManipulate(this,
                    tempPos,
                    &GameObject::f_SetPos,
                    &GameObject::f_GetPos);*/

                getsys(Factory)->m_redoUndo.RegisterManipulate2(this);
            }
            if (tempRot != m_rot)
            {
                /*getsys(Factory)->m_redoUndo.RegisterManipulate(this,
                    tempRot,
                    &GameObject::f_SetRot,
                    &GameObject::f_GetRot);*/

                getsys(Factory)->m_redoUndo.RegisterManipulate2(this);

            }
            if (tempSca != m_sca)
            {
                /*getsys(Factory)->m_redoUndo.RegisterManipulate(this,
                    tempSca,
                    &GameObject::f_SetSca,
                    &GameObject::f_GetSca);*/

                getsys(Factory)->m_redoUndo.RegisterManipulate2(this);
            }



            im = false; //not manipulating
        }

        ImGuizmo::RecomposeMatrixFromComponents(&m_pos.x, fakerot, &m_sca.x, &(m_transform[0][0]));

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed('U'))
            useSnap = !useSnap;
        ImGui::Checkbox("Use Snap", &useSnap);
       

        if (useSnap)
        {
            switch (mCurrentGizmoOperation)
            {
            case ImGuizmo::TRANSLATE:
                ImGui::InputFloat3("Snap", &snap[0]);
                break;
            case ImGuizmo::ROTATE:
                ImGui::InputFloat("Angle Snap", &snap[0]);
                break;
            case ImGuizmo::SCALE:
                ImGui::InputFloat("Scale Snap", &snap[0]);
                break;
            }
        }

        ImGuizmo::SetOrthographic(false);

        ImGui::Checkbox("Bound Sizing", &boundSizing);

        if (boundSizing)
        {
            ImGui::PushID(3);
            ImGui::Checkbox("", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


        float * CameraView = &((transpose(engine->f_getsystem<GLUN::Graphics>()->GetCamera()->GetViewMat()))[0][0]);
        float * CameraProjection = &(transpose((engine->f_getsystem<GLUN::Graphics>()->GetCamera()->GetProjMat()))[0][0]);

        ImGuizmo::Manipulate(CameraView, CameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, &(m_transform[0][0]), NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
        ImGuizmo::DecomposeMatrixToComponents(&(m_transform[0][0]), &m_pos.x, fakerot, &m_sca.x);

        ImGuiEditor* ePtr = getsys(ImGuiEditor);
        if (!ePtr->m_selecteds.empty() &&
            ePtr->m_selected->m_guid == m_guid
            && ImGuizmo::IsUsing())
        {
            //Manipulate
            vec3 difference = m_pos - tempPos2;

			//scale - no such vector operation but fk it
			vec3 scalar = { m_sca.x / tempSca2.x, 
				            m_sca.y / tempSca2.y,
							m_sca.z / tempSca2.z };

			vec3 rotDiff = m_rot - tempRot2;

            switch (mCurrentGizmoOperation)
            {
            case ImGuizmo::TRANSLATE:
                for (GameObject* gObj : ePtr->m_selecteds)
                {
                    if (gObj->m_guid == m_guid)
                        continue;

                    gObj->m_pos = gObj->m_pos + difference;
                }
                break;
            case ImGuizmo::ROTATE:
				for (GameObject* gObj : ePtr->m_selecteds)
				{
					if (gObj->m_guid == m_guid)
						continue;

					gObj->m_rot = gObj->m_rot + rotDiff;
					gObj->m_orientation = transpose(Rotation3x3(
						RAD2DEG(gObj->m_rot.x),
						RAD2DEG(gObj->m_rot.y),
						RAD2DEG(gObj->m_rot.z)));
				}
                break;
            case ImGuizmo::SCALE:
				for (GameObject* gObj : ePtr->m_selecteds)
				{
					if (gObj->m_guid == m_guid)
					{
						if (gObj->f_getcomp<RigidbodyVolume>() != nullptr)
						{
							gObj->f_getcomp<RigidbodyVolume>()->f_SyncColliderSize();
						}
						continue;
					}


					gObj->m_sca = gObj->m_sca * scalar;

					if (gObj->f_getcomp<RigidbodyVolume>() != nullptr)
					{
						gObj->f_getcomp<RigidbodyVolume>()->f_SyncColliderSize();
					}
				}
                break;
            }
        }


        if (!ePtr->m_selecteds.empty() &&
            ePtr->m_selected->m_guid == m_guid)
        {
            /* I am the master object */
            if (ImGuizmo::IsUsing() && !im)
            {
                for (GameObject* obj : ePtr->m_selecteds)
                {
                    obj->tempPos = obj->m_pos;
                    obj->tempRot = obj->m_rot;
                    obj->tempSca = obj->m_sca;
                }

                im = true; //is manipulating
            }
            else if (!ImGuizmo::IsUsing() && im)
            {
                //finish using imguizmo, save previous states
                if (tempPos != m_pos ||
                    tempRot != m_rot ||
                    tempSca != m_sca)
                {
                   // getsys(Factory)->m_redoUndo.RegisterMassManipulate(ePtr->m_selecteds);
                    getsys(Factory)->m_redoUndo.RegisterMassManipulate2(ePtr->m_selecteds);
                }



                im = false; //not manipulating
            }
        }

        if (ImGui::Button("Delete Selected Game Objects") || g_Input->CheckButtonTriggered(VK_DELETE))
        {
            getsys(ImGuiEditor)->m_selected = nullptr;
            //getsys(Factory)->m_redoUndo.RegisterMassDelete(ePtr->m_selecteds);
            getsys(Factory)->m_redoUndo.RegisterMassDelete2(ePtr->m_selecteds);

            for (GameObject* elem : ePtr->m_selecteds)
                getsys(Factory)->f_removego(elem);

            ePtr->m_selecteds.clear();
            //RedoUndo::redoUndo->RegisterDelete(this);
        }

        /*
        
        //This goes into Graphics.cpp update loop for feedback
         for (Model* elem : models_all)
        {
            elem->unselect();

            for (GameObject* obj : getsys(ImGuiEditor)->m_selecteds)
            {
                if (elem->f_getowner()->m_guid == obj->m_guid)
                    elem->select();
            }
        }
        
        */


        return 0;
    }

    ///////-Single

	//ImGui::BeginChild("Propetiess", ImVec2(320.0f, 90.0f), true);

    ImGui::Text("GUID: %I64u", m_guid);
    //ReName = m_name.c_str();

    ImGui::Text("Archetype: %s", getsys(Factory)->m_arche.m_arche_names[m_archetype_status].c_str());
    strcpy_s(ReName, m_name.c_str());
    //Name

    ImGui::Text("Name:"); ImGui::SameLine();

    if (ImGui::InputText("##name", ReName, IM_ARRAYSIZE(ReName)))
    {
        //GameObjectNames[GOselected] = ReName;
        m_name = ReName;
        //memset(ReName, 0, 64);
    }

    //Tags
	ReTag = m_tag;
    ImGui::Text("Tag: "); 

	ImGui::SameLine(); 

	//if (ImGui::Button("Change Tag"))
	//{

	//}

	ImGui::AlignTextToFramePadding();

	static auto TagGetter = [](void* vec, int id, const char** out)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (id < 0 || id >= static_cast<int>(vector.size()))
		{
			return false;
		}
		*out = vector.at(id).c_str();
		return true;
	};

	if (ImGui::Combo("Tags", &ReTag, TagGetter, static_cast<void*>(&getsys(Factory)->m_tagman.m_tagnames), static_cast<int>(getsys(Factory)->m_tagman.m_tagnames.size())))
	{
		std::string newtag = getsys(Factory)->m_tagman.m_tagnames[ReTag];

		for (const auto& elem : getsys(Factory)->m_tagman.m_tags)
		{
			if (elem.second == newtag)
				m_tag = elem.first;
		}
	}

	//ImGui::EndChild();
	



    ImGui::Separator();

    if (ImGui::IsAnyItemHovered() == false)
    {
        if (g_Input->CheckButtonHold(VK_CONTROL) && ImGui::IsKeyPressed('Q'))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        /*if (ImGui::IsKeyPressed('R'))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;*/
        if (g_Input->CheckButtonHold(VK_CONTROL) && ImGui::IsKeyPressed('E'))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
    }

    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    //if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
    //    mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    
    mat4 m_transform{};

    if (ImGuizmo::IsUsing() && !im)
    {
        tempPos = m_pos;
        tempRot = m_rot;
        tempSca = m_sca;


        im = true; //is manipulating
    }
    else if (!ImGuizmo::IsUsing() && im)
    {
        //finish using imguizmo, save previous states

        if (tempPos != m_pos)
        {
            /*getsys(Factory)->m_redoUndo.RegisterManipulate(this,
                tempPos,
                &GameObject::f_SetPos,
                &GameObject::f_GetPos);*/

            getsys(Factory)->m_redoUndo.RegisterManipulate2(this);
        }
        if (tempRot != m_rot)
        {
            /*getsys(Factory)->m_redoUndo.RegisterManipulate(this,
                tempRot,
                &GameObject::f_SetRot,
                &GameObject::f_GetRot);*/

            getsys(Factory)->m_redoUndo.RegisterManipulate2(this);
        }
        if (tempSca != m_sca)
        {
           /* getsys(Factory)->m_redoUndo.RegisterManipulate(this,
                tempSca,
                &GameObject::f_SetSca,
                &GameObject::f_GetSca);*/

            getsys(Factory)->m_redoUndo.RegisterManipulate2(this);

			if (f_getcomp<RigidbodyVolume>() != nullptr)
			{
				f_getcomp<RigidbodyVolume>()->f_SyncColliderSize();
			}



        }





        im = false; //not manipulating
    }

    ImGuizmo::RecomposeMatrixFromComponents(&m_pos.x, fakerot, &m_sca.x, &(m_transform[0][0]));

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    if (ImGui::IsKeyPressed('U'))
        useSnap = !useSnap;
    ImGui::Checkbox("Use Snap", &useSnap);
    //ImGui::SameLine();
    if (useSnap)
    {
        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        //case ImGuizmo::ROTATE:
        //    ImGui::InputFloat("Angle Snap", &snap[0]);
        //    break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);

            break;
        }
    }

    ImGuizmo::SetOrthographic(false);




    ImGui::Checkbox("Bound Sizing", &boundSizing);

    if (boundSizing)
    {
        ImGui::PushID(3);
        ImGui::Checkbox("", &boundSizingSnap);
        ImGui::SameLine();
        ImGui::InputFloat3("Snap", boundsSnap);
        ImGui::PopID();
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	if (f_getcomp<Mesh>() != nullptr)
	{

	}
	else
	{
		float * CameraView = &((transpose(engine->f_getsystem<GLUN::Graphics>()->GetCamera()->GetViewMat()))[0][0]);
		float * CameraProjection = &(transpose((engine->f_getsystem<GLUN::Graphics>()->GetCamera()->GetProjMat()))[0][0]);
		ImGuizmo::Manipulate(CameraView, CameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, &(m_transform[0][0]), NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
		ImGuizmo::DecomposeMatrixToComponents(&(m_transform[0][0]), &m_pos.x, fakerot, &m_sca.x);
	}

	//Set orientation
	m_orientation = transpose(Rotation3x3(
		RAD2DEG(m_rot.x),
		RAD2DEG(m_rot.y),
		RAD2DEG(m_rot.z)));

	//m_orientation = transpose(Cut( ToMatrix4(Quaternion(m_rot)), 3, 3));

    ImGuiEditor* ePtr = getsys(ImGuiEditor);
    if (!ePtr->m_selecteds.empty() &&
        ePtr->m_selected->m_guid == m_guid
        && ImGuizmo::IsUsing())
    {


        vec3 difference = m_pos - tempPos2;

        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            for (GameObject* gObj : ePtr->m_selecteds)
            {
                if (gObj->m_guid == m_guid)
                    continue;

                gObj->m_pos = gObj->m_pos + difference;
            }


            break;
        case ImGuizmo::ROTATE:

            break;
        case ImGuizmo::SCALE:

            break;
        }
    }


    ImGui::Text("X"); ImGui::SameLine(100); ImGui::Text("Y"); ImGui::SameLine(190); ImGui::Text("Z");
    //Create temp and store values- for slider inputs

	
    bool transChange = ImGui::DragFloat3("Trans", &m_pos.x, 0.5f);
	if (transChange)
	{
		Button* btn = f_getcomp<Button>();
		if (btn)
			btn->BtnPos = m_pos;
	}

    if (ImGui::BeginDragDropTarget())
    {
        if (auto* payload = ImGui::AcceptDragDropPayload("payload_gObj"))
        {
            IM_ASSERT(payload->DataSize == sizeof(GameObject));

            GameObject* xgobj = (GameObject*)payload->Data;

            m_pos = xgobj->m_pos;
			Button* btn = f_getcomp<Button>();
			if (btn)
				btn->BtnPos = m_pos;
            transChange = true;
        }
        ImGui::EndDragDropTarget();
    }
    
    bool scaleChange = ImGui::DragFloat3("Scale", &m_sca.x, 0.5f);
    if (ImGui::BeginDragDropTarget())
    {
        if (auto* payload = ImGui::AcceptDragDropPayload("payload_gObj"))
        {
            IM_ASSERT(payload->DataSize == sizeof(GameObject));

            GameObject* xgobj = (GameObject*)payload->Data;

            m_sca = xgobj->m_sca;
            scaleChange = true;
        }
        ImGui::EndDragDropTarget();
    }
    bool rotChange = ImGui::DragFloat3("Rotate", &m_rot.x, OUR_PI/180.0f);
    if (ImGui::BeginDragDropTarget())
    {
        if (auto* payload = ImGui::AcceptDragDropPayload("payload_gObj"))
        {
            IM_ASSERT(payload->DataSize == sizeof(GameObject));

            GameObject* xgobj = (GameObject*)payload->Data;

            m_rot = xgobj->m_rot;
            rotChange = true;
        }
        ImGui::EndDragDropTarget();
    }


    if ((transChange || scaleChange || rotChange) && !floatM && ImGui::IsMouseDown(0))
    {

        tempPos = m_pos;
        tempRot = m_rot;
        tempSca = m_sca;

        floatM = true;
    }
    else if ((transChange || scaleChange || rotChange) && !floatM && !ImGui::IsMouseDown(0))
    {

        tempPos = tempPos2;
        tempRot = tempRot2;
        tempSca = tempSca2;

        //getsys(Factory)->m_redoUndo.RegisterManipulate(this);
        getsys(Factory)->m_redoUndo.RegisterManipulate2(this);
    }
    else if (!transChange && !scaleChange && !rotChange && floatM && !ImGui::IsMouseDown(0))
    {

        //getsys(Factory)->m_redoUndo.RegisterManipulate(this);
        getsys(Factory)->m_redoUndo.RegisterManipulate2(this);

        floatM = false;
    }

    ImGui::Separator();

    for (auto & elem : m_components)
    {
        if (elem->f_imgui() == 1)
            break;
        ImGui::Separator();
    }

	if (!m_scriptNames.empty())
	{
		static int toberemoved = -1;
		int scriptit = 0;
		//std::string m_remove;
		if (ImGui::CollapsingHeader("Scripts attached"))
		{

			for (const auto& elem : m_scriptNames)
			{
				if (ImGui::Selectable(elem.c_str(), toberemoved == scriptit))
				{
					toberemoved = scriptit;
					//m_remove = elem;
				}
				++scriptit;
			}

            for (auto& elem : m_scripts)
            {
					elem->f_imgui();
            }

			if (ImGui::SmallButton("Edit Script"))
			{

				auto tmpit = m_scriptNames.begin();
				std::string editscript;
				

				for (int i = 0; i < m_scriptNames.size(); ++i)
				{
					if (i == toberemoved)
					{
						editscript = "start ../../Scripts/Scripts.sln ../../Scripts/Scripts/" + *tmpit + ".cpp ";
						editscript += "../../Scripts/Scripts/" + *tmpit + ".h";

						system(editscript.c_str());
					}
					++tmpit;
				}

				



			}

			if (ImGui::SmallButton("Remove Script"))
			{
				auto tmpit = m_scriptNames.begin();

				for (int i = 0; i < m_scriptNames.size(); ++i)
				{
					if (i == toberemoved)
					{
						f_removescript(*tmpit);
						break;
					}
					++tmpit;
				}
			}
		}
	}

	
	

	ImGui::Separator();

    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComp");
    }

	//static bool Error = false;
    if (ImGui::BeginPopup("AddComp"))
    {
        //static std::vector<std::string> CompNames = getsys(Factory)->m_compman.m_componentnames;

        for (const auto& tmp : getsys(Factory)->m_compman.m_componentnames)
        {
            if (ImGui::MenuItem(tmp.c_str()))
            {
				std::cout << tmp.c_str() << std::endl;
				if (!strcmp(tmp.c_str(), "class Button"))
				{
					if (f_getcomp<Mesh>() == nullptr)
					{
						Model * meshtest = f_getcomp<Model>();
						if (meshtest != nullptr)
						{
							getsys(Factory)->m_redoUndo.RegisterComponentDelete2(this, meshtest);
							f_removecomp(meshtest);
						}
						Component* test = f_addcomp(getsys(Factory)->m_compman.f_createdef("class Mesh"));
						if (test != nullptr)
						{
							test->f_initialize();
							getsys(Factory)->m_redoUndo.RegisterComponentCreation2(this, test);
						}
					}
				}
				
				Component* test = f_addcomp(getsys(Factory)->m_compman.f_createdef(tmp));
				
				//Save the registering action
				if (test != nullptr)
				{
					Model * meshtest = f_getcomp<Model>();
					if (typeid(Mesh) == typeid(*test) && meshtest != nullptr)
						f_removecomp(meshtest);
					test->f_initialize();
					//getsys(Factory)->m_redoUndo.RegisterComponentCreation(this, test);
					getsys(Factory)->m_redoUndo.RegisterComponentCreation2(this, test);
				}
            }
        }

        ImGui::EndPopup();
    }
	
	/*if (Error)
	{
		ImGui::OpenPopup("ERROR~!");
	}

	if (ImGui::BeginPopupModal("ERROR~!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("YOU NEED A MESH FOR BUTTON");
		if (ImGui::Button("Close"))
		{
			Error = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}*/

	static bool AddingScript = false;
	static ImGuiTextFilter filter;

	if ((ImGui::Button("Add Script") || AddingScript))
	{
		filter.Draw("Scripts");
		ImGui::BeginChild("ScriptAdder", ImVec2(ImGui::GetContentRegionAvailWidth(), 200.0f), true);
		AddingScript = true;
		auto it = ScriptNames.begin();
		static int id = 0;
		static int selectedscript = 0;



		static int hover = 0;

		for (id = 0; id < ScriptNames.size(); ++id, ++it)
		{
			char label[64];
			sprintf_s(label, it->c_str());

			bool isSelected = false;

			if (filter.PassFilter(ScriptNames[id].c_str()))
			{
				if (ImGui::IsItemHovered())
				{
					hover = id;
				}

				if (ImGui::Selectable(label, isSelected, ImGuiSelectableFlags_AllowDoubleClick))
				{
					selectedscript = id;
				}

				if (ImGui::IsMouseDoubleClicked(0))
				{
					f_assignscript(ScriptNames.at(selectedscript));

					AddingScript = false;
				}
			}

		}


		ImGui::EndChild();

		if (ImGui::Button("Add script"))
		{
			f_assignscript(ScriptNames.at(selectedscript));

			AddingScript = false;
		}

		//static auto ScriptNameGetter = [](void* vec, int id, const char** out)
		//{
		//	auto& vector = *static_cast<std::vector<std::string>*>(vec);
		//	if (id < 0 || id >= static_cast<int>(vector.size()))
		//	{
		//		return false;
		//	}
		//	*out = vector.at(id).c_str();
		//	return true;
		//};

		//if (!allScripts.empty())
		//{
		//	if (ImGui::Combo("Scripts", &id, ScriptNameGetter, static_cast<void*>(&ScriptNames), static_cast<int>(ScriptNames.size())))
		//	{
		//		f_assignscript(ScriptNames.at(id));

		//		AddingScript = false;
		//	}

		//}

		if (ImGui::Button("Cancel"))
		{
			AddingScript = false;
		}
		
		
		ImGui::Separator();
	}






    if (ImGui::Button("Focus Object (f)") || (g_Input->CheckButtonHold(VK_CONTROL) && g_Input->CheckButtonTriggered(VK_F)))
    {
        getsys(GLUN::Graphics)->GetCamera()->focus(this);
    }

    if (ImGui::Button("Duplicate (CTL-D)") || (g_Input->CheckButtonHold(VK_CONTROL) && g_Input->CheckButtonTriggered(VK_D)))
    {
        GameObject* bunshin = f_clone();
        //getsys(Factory)->m_redoUndo.RegisterCreation(bunshin);
        getsys(Factory)->m_redoUndo.RegisterCreation2(bunshin);
    }


    if (ImGui::Button("Delete Game Object") || g_Input->CheckButtonTriggered(VK_DELETE))
    {
        getsys(ImGuiEditor)->m_selected = nullptr;
        //getsys(Factory)->m_redoUndo.RegisterDeleteV2(this);
        getsys(Factory)->m_redoUndo.RegisterDelete2(this);
        getsys(Factory)->f_removego(this);
    }

    //Archetype Options************************************************************************/
    if (ImGui::Button("Create Archetype"))
    {
        ImGui::OpenPopup("CreateNewArchetype");
    }
    
	if (m_archetype_status != 0)
    {
        if (ImGui::Button("Update Master Copy"))
        {
            //1. override template
            getsys(Factory)->m_arche.f_update_masterCopy(m_archetype_status, this);
            //2. use template to override all game objects in that level, except this guy
            getsys(Factory)->m_arche.f_update_archetype(m_archetype_status, this);
        }
		if (ImGui::Button("Take From Master Copy"))
		{
			getsys(Factory)->m_arche.f_instantiate_archetype(m_archetype_status, m_pos);
			getsys(ImGuiEditor)->m_selected = nullptr;
			//getsys(Factory)->m_redoUndo.RegisterDeleteV2(this);
			getsys(Factory)->m_redoUndo.RegisterDelete2(this);
			getsys(Factory)->f_removego(this);
		}
    }


    if (ImGui::BeginPopupModal("CreateNewArchetype", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char Archename[30] = "";
        ImGui::InputText("##name", Archename, IM_ARRAYSIZE(Archename));


        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            m_archetype_status = getsys(Factory)->m_arche.f_create_archetype(const_cast<GameObject*>(this), std::string{ Archename });

            memset(Archename, 0, 30);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            memset(Archename, 0, 30);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();

    }
    //Archetype Options************************************************************************/



    return 0;
}

int GameObject::f_save(XDocument doc, XNode node)
{
    x_saveguid(doc, node, "GUID", m_guid);

    //Node, save all data to node
    x_savevec3(doc, node, "pos", m_pos);
    x_savevec3(doc, node, "rot", m_rot);
    x_savevec3(doc, node, "sca", m_sca);
    //Save mat3 m_orientation (function to save mat3 not implemented yet)
    x_savestring(doc, node, "name", m_name);
    x_saveuint(node, "tag", m_tag);
    x_saveuint(node, "Archetype", m_archetype_status);


    //Call save for all components (because we don't know what the component is, we let the comp create its own node
    for (const auto& elem : m_components)
    {
        XNode go = doc->NewElement("Component");
        elem->f_save(doc, go);
        node->InsertEndChild(go);
    }
    //Call to save all script objects
	XNode sp = doc->NewElement("Scripts");
	for (const auto& elem : m_scripts)
	{
		XNode sps = doc->NewElement(elem->GetName().c_str());
		
        elem->f_save(doc, sps);

        sp->InsertEndChild(sps);	}
	node->InsertEndChild(sp);

    return 0;
}

int GameObject::f_load(XDocument doc, XNode node)
{
    //Node, load all data from node
    x_loadvec3(doc, node, "pos", m_pos);
    x_loadvec3(doc, node, "rot", m_rot);
    x_loadvec3(doc, node, "sca", m_sca);
    //Load mat3 m_orientation (function to load mat3 not implemented yet)
    x_loadstring(doc, node, "name", m_name);
    x_loaduint(node, "tag", m_tag);
    x_loaduint(node, "Archetype", m_archetype_status);
	
	//When loading level, make sure archetype still exsists
	m_archetype_status = getsys(Factory)->m_arche.f_sanitycheck(m_archetype_status);

    m_orientation = transpose(Rotation3x3(
        RAD2DEG(m_rot.x),
        RAD2DEG(m_rot.y),
        RAD2DEG(m_rot.z)));

    //Call load function for all node
    XNode comps = node->FirstChildElement("Component");
    while (comps != nullptr)
    {
        //Load name of component
        std::string name;
        x_loadstring(doc, comps, "NAME", name);
        //1. create component
        Component* newcomp = getsys(Factory)->m_compman.f_createdef(name);
        //2. add it to go
        f_addcomp(newcomp);
        //3. load component data
        newcomp->f_load(doc, comps);
        //4. iterate
        comps = comps->NextSiblingElement("Component");
    }
	//Call to load all script objects
	XNode scripts = node->FirstChildElement("Scripts");
	if (scripts != nullptr)
	{
		XNode curr_script = scripts->FirstChildElement();
		while (curr_script != nullptr)
		{
			//Load name of Script
			ScriptObject* scObj = f_assignscript(curr_script->Name());
			if (scObj != nullptr)
				scObj->f_load(doc, curr_script, scObj);
			else
				std::cout << "FAILED TO CREATE SCRIPT: " << curr_script->Name() << std::endl;

			//Iterate
			curr_script = curr_script->NextSiblingElement();
        }
	}
	

    return 0;
}

vec3 & GameObject::f_GetPos()
{
    return m_pos;
}

vec3 & GameObject::f_GetSca()
{
    return m_sca;
}

vec3 & GameObject::f_GetRot()
{
    return m_rot;
}

mat3 & GameObject::f_GetOrientation()
{
    return m_orientation;
}

void GameObject::f_SetPos(const vec3 & pos)
{
    m_pos = pos;
}

void GameObject::f_SetSca(const vec3 & sca)
{
    m_sca = sca;

	if (f_getcomp<RigidbodyVolume>() != nullptr)
	{
		f_getcomp<RigidbodyVolume>()->f_SyncColliderSize();
	}

}

void GameObject::f_SetRot(const vec3 & rot)
{
    m_rot = rot;

	m_orientation = transpose(Rotation3x3(
		RAD2DEG(m_rot.x),
		RAD2DEG(m_rot.y),
		RAD2DEG(m_rot.z)));
}

void GameObject::f_SetOrientation(const mat3 & o)
{
    m_orientation = o;
}

void GameObject::setOcclusionTransform()
{
	OcclusionCollider.Position = m_pos;
	OcclusionCollider.Size = m_sca * 0.5f;
	OcclusionCollider.Orientation = Rotation3x3(
		RAD2DEG(m_rot.x),
		RAD2DEG(m_rot.y),
		RAD2DEG(m_rot.z)
	);
}

mat4 GameObject::f_gettransform()
{
    return mat4();
}

void GameObject::f_SetIsInteractable(const bool & state)
{
	isInteractable = state;
}

bool GameObject::f_GetIsInteractable()
{
	return isInteractable;
}

GameObject * GameObject::f_clone()
{
    GameObject* clone = getsys(Factory)->f_creatego();
    clone->m_name = m_name;
    clone->m_tag = m_tag;
    clone->m_archetype_status = m_archetype_status;
    clone->m_pos = m_pos;
    clone->m_rot = m_rot;
    clone->m_sca = m_sca;
    clone->m_orientation = m_orientation;

    //Clone all the components
    for (const auto& elem : m_components)
    {
        Component * tmp = elem->f_clone();
        if (tmp != nullptr)
        {
            clone->f_addcomp(tmp);
        }

    }
    
    //Clone all the scripts
	for (const auto& elem : m_scripts)
	{

		clone->f_assignscript(elem->GetName());

	}
    //Not yet implemented
	return clone;
}

ScriptObject*  GameObject::f_assignscript()
{
    // get first elem
    ScriptObject * scr = allScripts[0]->CreateScript(this);

    if (scr)
    {
        ScriptIdentifier.insert(std::pair<GameObject*, std::string>(this, scr->GetName()));
        UsedScriptContainer.push_back(scr);
        m_scripts.push_back(scr);
        return scr;
    }
    else
        std::cout << "cannot" << std::endl;

    return nullptr;
}

ScriptObject* GameObject::f_assignscript(std::string name, bool active)
{
    std::transform(name.begin(), name.end(), name.begin(), tolower);
    ScriptObject * scr = nullptr;

	// if script exist, don't care
	for (auto& elem : m_scripts)
	{
		if (elem->GetName() == name)
		{
			std::cout << "script already exists" << std::endl;
			return nullptr;
		}
	}

    // get the scripts from list by name
    for (auto& elem : allScripts)
    {
		if (elem->GetName() == name)
		{
			scr = elem->CreateScript(this);
		}
    }

    if (scr)
    {
		if (active)
		{
			ScriptIdentifier.insert(std::pair<GameObject*, std::string>(this, scr->GetName()));
			UsedScriptContainer.push_back(scr);
		}
        m_scripts.push_back(scr);
		m_scriptNames.push_back(scr->GetName());
        return scr;
    }
    else
        std::cout << "cannot assign script" << std::endl;

    return nullptr;
}

void GameObject::f_removescript(std::string name)
{
	std::transform(name.begin(), name.end(), name.begin(), tolower);
	
	std::vector<ScriptObject*>::iterator position;
	for (auto& elem : UsedScriptContainer)
	{
		if (elem->GetName() == name && elem->GetGameObjectOwner() == this)
		{
			position = std::find(UsedScriptContainer.begin(), UsedScriptContainer.end(), elem);
			break;
		}

	}

	if (position != UsedScriptContainer.end()) // == UsedScriptContainer.end() means the element was not found
	{

		UsedScriptContainer.erase(position);

	}

	// remove in name list
	std::string toremove;
	for (auto& elem : m_scriptNames)
	{
		if (elem == name)
		{
			toremove = elem;
			break;
		}
	}

	m_scriptNames.remove(toremove);
	

	ScriptObject * scr = nullptr;
	// remove in scripts list and delete it
	for (auto& elem : m_scripts)
	{
		if (elem->GetName() == name)
		{
			scr = elem;
			break;
		}
	}
	m_scripts.remove(scr);
	delete scr;

	// the map for hot reloading 
	std::multimap<GameObject*, std::string> temp;

	for (auto& elem : ScriptIdentifier)
	{
		if (elem.first == this && elem.second == name)
		{
			continue;
		}

		temp.insert(std::pair<GameObject*, std::string>(elem.first, elem.second));
	}

	// assign back
	ScriptIdentifier.clear();
	ScriptIdentifier = temp;

}

std::list<AbstractComponent*> GameObject::GetComponents()
{
    std::list<AbstractComponent*> temp;
    std::copy(m_components.begin(), m_components.end(),
        std::back_inserter(temp));

    return temp;
}

std::list<ScriptObjectExport*> GameObject::GetScripts()
{
    std::list<ScriptObjectExport*> temp;
    std::copy(m_scripts.begin(), m_scripts.end(),
        std::back_inserter(temp));

    return temp;
}

ScriptObjectExport* GameObject::GetScript(std::string name)
{
	for (auto& elem : m_scripts)
	{
		ScriptObject* castedresult = static_cast<ScriptObject*>(elem);
		if (castedresult->GetName() == name)
		{
			return castedresult;
		}
	}


	return nullptr;
}

AbstractComponent* GameObject::GetComponent(ComponentType type)
{
	for (auto& elem : m_components)
	{
		if (type == e_RigidBody)
		{
			AbstractRigidbody* castedresult = static_cast<AbstractRigidbody*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}

		else if (type == e_Light)
		{
			AbstractLight* castedresult = static_cast<AbstractLight*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}

		else if (type == e_Camera)
		{
			AbstractCamera* castedresult = static_cast<AbstractCamera*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}

        else if (type == e_Model)
        {
            AbstractModel* castedresult = static_cast<AbstractModel*>(elem);
            if (castedresult && castedresult->comptype == type)
            {
                return castedresult;
            }
        }

        else if (type == e_AnimatedModel)
        {
            AbstractAnimatedModel* castedresult = static_cast<AbstractAnimatedModel*>(elem);
            if (castedresult && castedresult->comptype == type)
            {
                return castedresult;
            }
        }

		else if (type == e_Mesh)
		{
			AbstractMesh* castedresult = static_cast<AbstractMesh*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}

		else if (type == e_AudioListener)
		{
			AbstractAudioListener* castedresult = static_cast<AbstractAudioListener*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}

		else if (type == e_AudioSource)
		{
			AbstractAudioSource* castedresult = static_cast<AbstractAudioSource*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}

		else if (type == e_ParticleEmitter)
		{
			AbstractParticleEmitter* castedresult = static_cast<AbstractParticleEmitter*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}

		else if (type == e_Button)
		{
			AbstractButton* castedresult = static_cast<AbstractButton*>(elem);
			if (castedresult && castedresult->comptype == type)
			{
				return castedresult;
			}
		}
	}

	return nullptr;
}

void GameObject::f_redoundosave(std::vector<xProperties::ComplexxType>& container)
{
    container.push_back(xPropertyData<GameObject, vec3>{"position",
        &GameObject::f_GetPos,
        &GameObject::f_SetPos,
        m_pos,
        m_guid});

    container.push_back(xPropertyData<GameObject, vec3>{"Scale",
        &GameObject::f_GetSca,
        &GameObject::f_SetSca,
        m_sca,
        m_guid});

    container.push_back(xPropertyData<GameObject, vec3>{ "Rotation",
        &GameObject::f_GetRot,
        &GameObject::f_SetRot,
        m_rot,
        m_guid});
}

void GameObject::f_redoundostate(std::vector<xProperties::ComplexxType>& container)
{
    container.push_back(xPropertyData<GameObject, vec3>{"position",
        &GameObject::f_GetPos,
        &GameObject::f_SetPos,
        tempPos,
        m_guid});

    container.push_back(xPropertyData<GameObject, vec3>{"Scale",
        &GameObject::f_GetSca,
        &GameObject::f_SetSca,
        tempSca,
        m_guid});

    container.push_back(xPropertyData<GameObject, vec3>{ "Rotation",
        &GameObject::f_GetRot,
        &GameObject::f_SetRot,
        tempRot,
        m_guid});
}

void GameObject::f_redoundosave2(std::vector<xProperties::xLazyType>& container)
{
    container.push_back(xLazyPropertyData<GameObject>{"Position",
                        int(offsetof(GameObject, m_pos)),
                        m_pos,
        m_guid});

    container.push_back(xLazyPropertyData<GameObject>{"Scale",
                        int(offsetof(GameObject, m_sca)),
                        m_sca,
        m_guid});

    container.push_back(xLazyPropertyData<GameObject>{"Rotation",
                        int(offsetof(GameObject, m_rot)),
                        m_rot,
        m_guid});
}

void GameObject::f_redoundostate2(std::vector<xProperties::xLazyType>& container)
{
    container.push_back(xLazyPropertyData<GameObject>{"Position",
                        int(offsetof(GameObject, m_pos)),
                        tempPos, 
                        m_guid});

    container.push_back(xLazyPropertyData<GameObject>{"Scale",
                        int(offsetof(GameObject, m_sca)),
                        tempSca, 
                        m_guid});

    container.push_back(xLazyPropertyData<GameObject>{"Rotation",
                        int(offsetof(GameObject, m_rot)),
                        tempRot, 
                        m_guid});
}

std::string GameObject::f_getName()
{
	return m_name;
}

void GameObject::f_setName(std::string name)
{
	m_name = name;
}

std::string GameObject::f_getTag()
{
	return getsys(Factory)->m_tagman.m_tags[m_tag];
}

void GameObject::f_setTag(std::string tag)
{
	getsys(Factory)->m_tagman.m_tags[m_tag] = tag;
	getsys(Factory)->m_tagman.f_updatetag();
}

guid GameObject::GetGuid()
{
	return m_guid;
}

vec3 GameObject::ForwardDirection()
{
	return normalize(m_orientation * vec3 { 0.0f, 0.0f, 1.0f });
}

void GameObject::SetForward(const vec3 & target, const vec3 & me)
{
	vec3 direction = target - me;

	vec3 axisVec = { 0.0f, 0.0f, 1.0f };

	vec3 result = { 0.0f, 0.0f, 0.0f };

	float mag = Magnitude(direction);

	if (mag < FLT_EPSILON && mag > -FLT_EPSILON)
		mag = FLT_EPSILON;

	float tmp = direction.y / Magnitude(direction);

	tmp = fclamp(tmp, -1.0f, 1.0f);

	float theta = acos(tmp);
	float alpha = atan2(direction.x, direction.z);

	alpha += OUR_PI;
	f_SetRot(vec3{ 0.0f, alpha, 0.0f });
}

void GameObject::SetForwardTrue(const vec3 & target, const vec3 & me)
{
	vec3 direction = target - me;

	vec3 axisVec = { 0.0f, 0.0f, 1.0f };

	vec3 result = { 0.0f, 0.0f, 0.0f };

	float mag = Magnitude(direction);

	if (mag < FLT_EPSILON && mag > -FLT_EPSILON)
		mag = FLT_EPSILON;

	float tmp = direction.y / Magnitude(direction);

	tmp = fclamp(tmp, -1.0f, 1.0f);

	float theta = -acos(tmp);
	float alpha = atan2(direction.x, direction.z);

	alpha += OUR_PI;
	theta += 0.5f * OUR_PI;
	f_SetRot(vec3{ theta, alpha, 0.0f });
}
