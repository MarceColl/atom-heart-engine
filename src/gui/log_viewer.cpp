#include <string.h>
#include <imgui/imgui.h>

#include <gui/log_viewer.hpp>


void draw_log_viewer(log_allocator* la, const char* title) {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(title, NULL);
    char *log_msg;

    static ImGuiTextFilter filter;
    filter.Draw();
    static bool show_debug = true;
    static bool show_notice = true;
    static bool show_warning = true;
    static bool show_error = true;
    ImGui::TextUnformatted("Log entry types: ");
    ImGui::SameLine();
    ImGui::Checkbox("Debug", &show_debug);
    ImGui::SameLine();
    ImGui::Checkbox("Notice", &show_notice);
    ImGui::SameLine();
    ImGui::Checkbox("Warning", &show_warning);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &show_error);
    ImGui::Separator();

    LOG_TYPE type;
    char *nextlog = NULL;
    do {
	nextlog = next_log_(la, nextlog, &log_msg, &type);

	if(filter.PassFilter(log_msg)) {
	    switch(type) {
	    case DEBUG: {
		if(!show_debug) continue;
		ImGui::TextUnformatted("["); ImGui::SameLine();
		ImGui::PushStyleColor(0, (ImVec4)ImColor(255,255,255));
		ImGui::TextUnformatted("DEBUG"); ImGui::SameLine();
	    } break;
	    case NOTICE: {
		if(!show_notice) continue;
		ImGui::TextUnformatted("["); ImGui::SameLine();
		ImGui::PushStyleColor(0, (ImVec4)ImColor(128,128,128));
		ImGui::TextUnformatted("NOTICE"); ImGui::SameLine();
	    } break;
	    case WARNING: {
		if(!show_warning) continue;
		ImGui::TextUnformatted("["); ImGui::SameLine();
		ImGui::PushStyleColor(0, (ImVec4)ImColor(255,0,255));
		ImGui::TextUnformatted("WARNING"); ImGui::SameLine();
	    } break;
	    case ERROR: {
		if(!show_error) continue;
		ImGui::TextUnformatted("["); ImGui::SameLine();
		ImGui::PushStyleColor(0, (ImVec4)ImColor(255,0,0));
		ImGui::TextUnformatted("ERROR"); ImGui::SameLine();
	    } break;
	    }
	    ImGui::PopStyleColor();
	    ImGui::TextUnformatted("]");

	    ImGui::SameLine();
	    ImGui::TextUnformatted(log_msg);
	    ImGui::SetScrollHere(1.0f);
	}
    } while(nextlog != NULL);
    ImGui::End();
}
