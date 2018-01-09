#include <string.h>
#include <imgui/imgui.h>

#include <gui/log_viewer.hpp>


void draw_log_viewer(log_allocator* la, const char* title) {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(title, NULL);
    char *log_msg;
    LOG_TYPE type;
    char *nextlog = NULL;
     do {
	nextlog = next_log_(la, nextlog, &log_msg, &type);

	ImGui::TextUnformatted("["); ImGui::SameLine();
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

	ImGui::SameLine();
	ImGui::TextUnformatted(log_msg);
	ImGui::SetScrollHere(1.0f);
     } while(nextlog != NULL);
     ImGui::End();
}
