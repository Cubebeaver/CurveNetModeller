

#include <imgui.h>

namespace ImGui {
bool DragFloat3Colored(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
    bool changed = false;
    BeginGroup();
    PushID(label);

    float full_width = CalcItemWidth();
    float item_width = (full_width - GetStyle().ItemSpacing.x * 2.0f) / 3.0f;

    // Színek a csíkokhoz
    ImU32 colors[] = {
        IM_COL32(200, 50, 50, 255), // Piros
        IM_COL32(50, 200, 50, 255), // Zöld
        IM_COL32(50, 50, 200, 255)  // Kék
    };

    for (int i = 0; i < 3; i++) {
        if (i > 0) SameLine();
        PushID(i);

        // Rajzolunk egy kis színes csíkot a widget bal szélére
        ImVec2 p = GetCursorScreenPos();
        GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + 2, p.y + GetFrameHeight()), colors[i]);

        SetNextItemWidth(item_width);
        if (DragFloat("##v", &v[i], v_speed, v_min, v_max, format, flags)) changed = true;

        PopID();
    }

    SameLine();
    TextEx(label, FindRenderedTextEnd(label));

    PopID();
    EndGroup();
    return changed;
}
}
