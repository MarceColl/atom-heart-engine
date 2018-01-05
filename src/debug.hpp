#ifndef _DEBUG__H_
#define _DEBUG__H_

#include <imgui/imgui.h>
#include <game_introspection.hpp>

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

		switch(member->type) {
		case metatype_float:
		    ImGui::Text("%s: %f", member->name, *(float*)member_ptr);
		    break;

		case metatype_uint8_t:
		case metatype_uint16_t:
		case metatype_uint32_t:
		{
		    ImGui::Text("%s: %f", member->name, *(uint32_t*)member_ptr);
		} break;

		case metatype_bool:
		{
		    ImGui::Text("%s: %s", member->name, (*(bool*)member_ptr) ? "true" : "false");
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
