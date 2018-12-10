#pragma once
#ifndef __CG_IMGUI_UTIL_H__
#define __CG_IMGUI_UTIL_H__

#include <Imgui/imgui.h>

namespace CGProj
{
	// Helper to display a little (?) mark which shows a tooltip when hovered.
	static inline void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}

#endif