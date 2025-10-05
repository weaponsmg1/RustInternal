#pragma once
#include "imgui/imgui.h"
#include "sdk.h"
#include "functions.h"
#include "Dumper.hpp"
#include "imgui/imgui.h"
#include "Lists.hpp"
using namespace ImGui;

void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("[?]");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void DrawMenu()
{
	static int page = 0;
	if (ImGui::Begin("Rust", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::SetWindowPos(ImVec2(500, 500), ImGuiCond_Once);
		ImGui::SetWindowSize(ImVec2(375, 400), ImGuiCond_Once);
		ImGui::Checkbox("ESP Line", &vars::ESPLines);
		ImGui::Checkbox("ESP Box", &vars::ESPBox);
		ImGui::Checkbox("Crosshair", &vars::crosshair);

		ImGui::End();
	}
}

