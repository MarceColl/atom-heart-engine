#ifndef _DEBUG__H_
#define _DEBUG__H_

#include <imgui/imgui.h>

inline void DEBUG_inspect_struct(game_state *state,
				 uint32_t member_count,
				 property_entry *member_defs,
				 void *struct_ptr, char *title);
inline void DEBUG_inspect_struct_header(game_state *state,
					uint32_t member_count,
					property_entry *member_defs,
					void *struct_ptr, char *title);

inline void DEBUG_inspect_struct_inline(game_state *state,
					u32 member_count,
					property_entry *member_defs,
					void *struct_ptr,
					char *title) {
	    for(uint32_t i = 0; i < member_count; ++i) {
		property_entry *member = member_defs + i;
		void *member_ptr = (((uint8_t*)struct_ptr) + member->offset);

		char label_input[100];
		sprintf(label_input, "%s", member->name, (uint8_t*)struct_ptr);
		switch(member->type) {
		case metatype_float:
		{
		    ImGui::InputFloat(label_input, (float*)member_ptr, 0.001f, 1.0f);
		} break;
		case metatype_u8:
		case metatype_uint8_t:
		{
		    ImGui::Text("%s: %d", label_input, (u32)*(uint8_t*)member_ptr);
		} break;
		case metatype_u16:
		case metatype_uint16_t:
		{
		    ImGui::Text("%s: %d", label_input, (u32)*(uint16_t*)member_ptr);
		} break;
		case metatype_u32:
		case metatype_uint32_t:
		{
		    ImGui::Text("%s: %d",label_input, (u32)*(uint32_t*)member_ptr);
		} break;
		case metatype_u64:
		case metatype_uint64_t:
		{
		    ImGui::Text("%s: %d",label_input, (u32)*(uint64_t*)member_ptr);
		} break;
		case metatype_s8:
		case metatype_int8_t:
		{
		    ImGui::Text("%s: %d",label_input, (s32)*(int8_t*)member_ptr);
		} break;
		case metatype_s16:
		case metatype_int16_t:
		{
		    ImGui::Text("%s: %d",label_input, (s32)*(int16_t*)member_ptr);
		} break;
		case metatype_s32:
		case metatype_int32_t:
		{
		    ImGui::Text("%s: %d",label_input, (int32_t*)member_ptr);
		} break;
		case metatype_s64:
		case metatype_int64_t:
		{
		    ImGui::Text("%s: %d",label_input, (s32)*(int64_t*)member_ptr);
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
		case metatype_animation_dict:
		{
		    ImGui::Text("%s: %f", member->name, (*(animation_dict*)member_ptr)["WALK"]->frames[0].time);
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
}

inline void DEBUG_inspect_struct_header(game_state *state,
					uint32_t member_count,
					property_entry *member_defs,
					void *struct_ptr, char *title) {
	if(ImGui::CollapsingHeader(title)) {
	    ImGui::Indent(15.0f);
	    DEBUG_inspect_struct_inline(state,
					member_count,
					member_defs,
					struct_ptr,
					title);
	    ImGui::Unindent(15.0f);
	}
}

inline void DEBUG_inspect_struct(game_state *state,
				 uint32_t member_count,
				 property_entry *member_defs,
				 void *struct_ptr, char *title) {
    ImGui::Begin("Inspector"); {
	DEBUG_inspect_struct_header(state,
				    member_count,
				    member_defs,
				    struct_ptr,
				    title);
    }
    ImGui::End();
}  

#endif
