#ifndef _DEBUG__H_
#define _DEBUG__H_

#include <imgui/imgui.h>

inline void DEBUG_inspect_struct(uint32_t member_count,
				 property_entry *member_defs,
				 void *struct_ptr,
				 char *title) {
    ImGui::Begin("Inspector"); {
	if(ImGui::CollapsingHeader(title)) {
	    ImGui::Indent(15.0f);
	    for(uint32_t i = 0; i < member_count; ++i) {
		property_entry *member = member_defs + i;
		void *member_ptr = (((uint8_t*)struct_ptr) + member->offset);

		char label_input[100];
		sprintf(label_input, "%s##%h", member->name, (uint8_t*)struct_ptr);
		switch(member->type) {
		case metatype_float:
		{
		    ImGui::InputFloat(label_input, (float*)member_ptr, 0.001f, 1.0f);
		} break;
		case metatype_uint8_t:
		{
		    // ImGui::InputInt(label_input, (uint8_t*)member_ptr);
		} break;
		case metatype_uint16_t:
		{
		    //ImGui::InputInt(label_input, (uint16_t*)member_ptr);
		} break;
		case metatype_uint32_t:
		{
		    // ImGui::InputInt(label_input, (uint32_t*)member_ptr);
		} break;
		case metatype_uint64_t:
		{
		    //ImGui::InputInt(label_input, (uint64_t*)member_ptr);
		} break;
		case metatype_int8_t:
		{
		    //ImGui::InputInt(label_input, (int8_t*)member_ptr);
		} break;
		case metatype_int16_t:
		{
		    //ImGui::InputInt(label_input, (int16_t*)member_ptr);
		} break;
		case metatype_int32_t:
		{
		    ImGui::InputInt(label_input, (int32_t*)member_ptr);
		} break;
		case metatype_int64_t:
		{
		    //ImGui::InputInt(label_input, (int64_t*)member_ptr);
		} break;
		case metatype_char:
		{
		    ImGui::Text("%s: %c", member->name, *(char*)member_ptr);
		} break;
		case metatype_pchar:
		{
		    ImGui::Text("%s: %s", member->name, *(char**)member_ptr);
		} break;
		case metatype_bool:
		{
		    ImGui::Text("%s: %s", member->name, (*(bool*)member_ptr) ? "true" : "false");
		} break;
		case metatype_eid:
		{
		    ImGui::Text("%s: %u", member->name, *(eid*)member_ptr);
		} break;
		case metatype_flags_t:
		{
		    ImGui::Text("%s: %u", member->name, *(flags_t*)member_ptr);
		} break;
		case metatype_glm__mat4:
		{
		    glm::mat4 *mat = (glm::mat4*)member_ptr;
		    ImGui::Text("%s:", member->name);
		    ImGui::Columns(4);
		    ImGui::Separator();
		    for(int i = 0; i < 4; ++i) {
			for(int j = 0; j < 4; ++j) {
			    ImGui::Text("%f", (*mat)[j][i]); ImGui::NextColumn();
			}
			ImGui::Separator();
		    }
		    ImGui::Columns(1);

		} break;
		case metatype_glm__vec2:
		{
		    glm::vec2 *mat = (glm::vec2*)member_ptr;
		    ImGui::Text("%s:", member->name);
		    ImGui::Columns(1);
		    ImGui::Separator();
		    ImGui::Text("x: %f", (*mat)[0]); ImGui::NextColumn();
		    ImGui::Text("y: %f", (*mat)[1]); ImGui::NextColumn();
		    ImGui::Separator();
		    ImGui::Columns(1);
		} break;
		case metatype_mat_id:
		{
		    ImGui::Text("%s: %u", member->name, *(mat_id*)member_ptr);
		} break;

		INTROSPECTION_SWITCH_TYPE_HELPER

		default:
		    break;
		}
	    }
	    ImGui::Unindent(15.0f);
	}
    ImGui::End();
    }
}  

#endif
