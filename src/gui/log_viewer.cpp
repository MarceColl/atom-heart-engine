#include <string.h>
#include <imgui/imgui.h>

#include <gui/log_viewer.hpp>


void draw_log_viewer(log_allocator* la, const char* title) {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(title, NULL);
    char *p_line = la->base;
    while((u64)p_line < (u64)la->curr) {
	ImGui::TextUnformatted("["); ImGui::SameLine();
	LOG_TYPE type = *(LOG_TYPE*)p_line;
	switch(type) {
	case DEBUG: {
	    ImGui::PushStyleColor(0, (ImVec4)ImColor(255,255,255));
	    ImGui::TextUnformatted("DEBUG"); ImGui::SameLine();
	} break;
	case NOTICE: {
	    ImGui::PushStyleColor(0, (ImVec4)ImColor(128,128,128));
	    ImGui::TextUnformatted("NOTICE"); ImGui::SameLine();
	} break;
	case WARNING: {
	    ImGui::PushStyleColor(0, (ImVec4)ImColor(255,0,255));
	    ImGui::TextUnformatted("WARNING"); ImGui::SameLine();
	} break;
	case ERROR: {
	    ImGui::PushStyleColor(0, (ImVec4)ImColor(255,0,0));
	    ImGui::TextUnformatted("ERROR"); ImGui::SameLine();
	} break;
	}
	ImGui::PopStyleColor();
	ImGui::TextUnformatted("]");
	++p_line;

	ImGui::SameLine();
	ImGui::TextUnformatted(p_line);
	// NOTE(Marce): Need to include the null char
	p_line += strlen(p_line) + 1;
    }
    ImGui::End();
}
