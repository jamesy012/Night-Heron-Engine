#include "ImGuiHelper.h"

#include "ImGui/imgui.h"


bool ImGuiHelper::Selectable(int* a_Index, CMArray<CMString> a_Names) {
	static ImGuiTextFilter filter;

	filter.Draw("", 150);

	for (uint i = 0; i < a_Names.Length(); i++) {
		CMString text = a_Names[i];
		if (filter.PassFilter(text.Get())) {
			if (ImGui::Selectable(text.Get(), *a_Index == i)) {
				*a_Index = i;
				return true;
			}
		}
	}
	return false;
}
