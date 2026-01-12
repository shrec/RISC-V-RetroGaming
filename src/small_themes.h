// Minimal themes header avoiding C++ globals and std::string

#ifndef D6591F0F_033F_4A41_A433_55E52BFEF893
#define D6591F0F_033F_4A41_A433_55E52BFEF893

// Minimal ImVec4 replacement (no ImGui dependency)
struct ImVec4 {
    float x, y, z, w;
    ImVec4() : x(0), y(0), z(0), w(0) {}
    ImVec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

struct Theme {
    const char* name;
    ImVec4 windowBg;
    ImVec4 headerBg;
    ImVec4 menuBg;

    ImVec4 buttonNormal;
    ImVec4 buttonHovered;
    ImVec4 buttonActive;
    ImVec4 selectionBg;
    ImVec4 selectionGlow;

    ImVec4 textPrimary;
    ImVec4 textSecondary;
    ImVec4 textHighlight;

    ImVec4 accentPrimary;
    ImVec4 accentSecondary;

    ImVec4 iconFolder;
    ImVec4 iconSettings;
    ImVec4 iconExit;
};

static const Theme ALL_THEMES[] = {
    {"PlayStation 5 (Modern)", ImVec4(0.95f,0.95f,0.98f,0.98f), ImVec4(0.92f,0.92f,0.96f,1.0f), ImVec4(0.90f,0.90f,0.95f,0.95f), ImVec4(0.85f,0.85f,0.92f,1.0f), ImVec4(0.75f,0.80f,0.90f,1.0f), ImVec4(0.65f,0.75f,0.95f,1.0f), ImVec4(0.20f,0.50f,1.00f,0.25f), ImVec4(0.30f,0.60f,1.00f,0.15f), ImVec4(0.05f,0.05f,0.10f,1.0f), ImVec4(0.30f,0.30f,0.40f,1.0f), ImVec4(0.00f,0.20f,0.50f,1.0f), ImVec4(0.00f,0.40f,0.85f,1.0f), ImVec4(0.30f,0.60f,1.00f,1.0f), ImVec4(0.20f,0.50f,1.00f,1.0f), ImVec4(0.30f,0.30f,0.40f,1.0f), ImVec4(0.85f,0.20f,0.30f,1.0f)},
    {"PlayStation 4", ImVec4(0.02f,0.10f,0.18f,0.98f), ImVec4(0.03f,0.13f,0.24f,1.0f), ImVec4(0.04f,0.15f,0.28f,0.95f), ImVec4(0.06f,0.20f,0.35f,1.0f), ImVec4(0.10f,0.30f,0.50f,1.0f), ImVec4(0.15f,0.40f,0.65f,1.0f), ImVec4(0.00f,0.45f,0.85f,1.0f), ImVec4(0.00f,0.55f,1.00f,0.6f), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(0.7f,0.75f,0.85f,1.0f), ImVec4(0.85f,0.92f,1.0f,1.0f), ImVec4(0.00f,0.60f,1.00f,1.0f), ImVec4(1.00f,0.50f,0.10f,1.0f), ImVec4(0.00f,0.70f,1.00f,1.0f), ImVec4(0.70f,0.75f,0.85f,1.0f), ImVec4(1.00f,0.50f,0.10f,1.0f)},
    {"Xbox Series X/S", ImVec4(0.06f,0.08f,0.06f,0.98f), ImVec4(0.08f,0.12f,0.08f,1.0f), ImVec4(0.10f,0.14f,0.10f,0.95f), ImVec4(0.15f,0.22f,0.15f,1.0f), ImVec4(0.20f,0.32f,0.20f,1.0f), ImVec4(0.25f,0.42f,0.25f,1.0f), ImVec4(0.10f,0.75f,0.20f,1.0f), ImVec4(0.15f,0.85f,0.30f,0.5f), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(0.7f,0.8f,0.7f,1.0f), ImVec4(0.85f,1.0f,0.85f,1.0f), ImVec4(0.15f,0.85f,0.30f,1.0f), ImVec4(0.30f,1.00f,0.45f,1.0f), ImVec4(0.90f,0.70f,0.20f,1.0f), ImVec4(0.70f,0.80f,0.70f,1.0f), ImVec4(0.85f,0.30f,0.30f,1.0f)},
    {"Nintendo Switch", ImVec4(0.14f,0.14f,0.14f,0.98f), ImVec4(0.18f,0.18f,0.18f,1.0f), ImVec4(0.20f,0.20f,0.20f,0.95f), ImVec4(0.28f,0.28f,0.28f,1.0f), ImVec4(0.35f,0.35f,0.35f,1.0f), ImVec4(0.90f,0.10f,0.15f,1.0f), ImVec4(0.90f,0.10f,0.15f,1.0f), ImVec4(1.00f,0.20f,0.25f,0.5f), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(0.7f,0.7f,0.7f,1.0f), ImVec4(1.0f,0.95f,0.95f,1.0f), ImVec4(0.90f,0.10f,0.15f,1.0f), ImVec4(0.20f,0.60f,0.90f,1.0f), ImVec4(0.90f,0.70f,0.20f,1.0f), ImVec4(0.70f,0.70f,0.70f,1.0f), ImVec4(0.90f,0.10f,0.15f,1.0f)},
    {"Steam Deck", ImVec4(0.10f,0.12f,0.15f,0.98f), ImVec4(0.13f,0.16f,0.20f,1.0f), ImVec4(0.15f,0.18f,0.23f,0.95f), ImVec4(0.20f,0.25f,0.32f,1.0f), ImVec4(0.25f,0.35f,0.45f,1.0f), ImVec4(0.35f,0.50f,0.70f,1.0f), ImVec4(0.25f,0.55f,0.85f,1.0f), ImVec4(0.35f,0.65f,0.95f,0.5f), ImVec4(0.95f,0.95f,0.98f,1.0f), ImVec4(0.65f,0.68f,0.75f,1.0f), ImVec4(0.85f,0.92f,1.0f,1.0f), ImVec4(0.25f,0.60f,0.90f,1.0f), ImVec4(0.40f,0.75f,1.00f,1.0f), ImVec4(0.90f,0.70f,0.20f,1.0f), ImVec4(0.65f,0.68f,0.75f,1.0f), ImVec4(0.85f,0.35f,0.35f,1.0f)},
    {"Dark Blue (Default)", ImVec4(0.06f,0.06f,0.12f,0.95f), ImVec4(0.08f,0.12f,0.20f,1.0f), ImVec4(0.10f,0.12f,0.18f,1.0f), ImVec4(0.15f,0.25f,0.45f,1.0f), ImVec4(0.20f,0.35f,0.60f,1.0f), ImVec4(0.25f,0.40f,0.70f,1.0f), ImVec4(0.25f,0.40f,0.70f,1.0f), ImVec4(0.20f,0.35f,0.60f,0.5f), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(0.7f,0.7f,0.8f,1.0f), ImVec4(0.8f,0.9f,1.0f,1.0f), ImVec4(0.3f,0.6f,1.0f,1.0f), ImVec4(0.5f,0.8f,1.0f,1.0f), ImVec4(0.9f,0.7f,0.2f,1.0f), ImVec4(0.6f,0.6f,0.6f,1.0f), ImVec4(0.8f,0.3f,0.3f,1.0f)},
    {"RetroArch Green", ImVec4(0.02f,0.08f,0.05f,0.95f), ImVec4(0.05f,0.12f,0.08f,1.0f), ImVec4(0.08f,0.15f,0.10f,1.0f), ImVec4(0.10f,0.30f,0.20f,1.0f), ImVec4(0.15f,0.45f,0.30f,1.0f), ImVec4(0.20f,0.55f,0.35f,1.0f), ImVec4(0.20f,0.60f,0.40f,1.0f), ImVec4(0.15f,0.50f,0.35f,0.5f), ImVec4(0.9f,1.0f,0.95f,1.0f), ImVec4(0.6f,0.8f,0.7f,1.0f), ImVec4(0.7f,1.0f,0.8f,1.0f), ImVec4(0.3f,0.9f,0.5f,1.0f), ImVec4(0.5f,1.0f,0.7f,1.0f), ImVec4(0.9f,0.7f,0.2f,1.0f), ImVec4(0.5f,0.7f,0.6f,1.0f), ImVec4(0.8f,0.3f,0.3f,1.0f)},
    {"ArkOS Blue", ImVec4(0.12f,0.25f,0.45f,0.95f), ImVec4(0.15f,0.30f,0.55f,1.0f), ImVec4(0.18f,0.35f,0.60f,0.9f), ImVec4(0.20f,0.40f,0.65f,1.0f), ImVec4(0.25f,0.50f,0.75f,1.0f), ImVec4(0.30f,0.60f,0.85f,1.0f), ImVec4(0.35f,0.65f,0.90f,1.0f), ImVec4(0.25f,0.55f,0.80f,0.5f), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(0.8f,0.85f,0.95f,1.0f), ImVec4(0.9f,0.95f,1.0f,1.0f), ImVec4(0.4f,0.7f,1.0f,1.0f), ImVec4(0.6f,0.85f,1.0f,1.0f), ImVec4(1.0f,0.8f,0.3f,1.0f), ImVec4(0.85f,0.85f,0.85f,1.0f), ImVec4(0.9f,0.4f,0.4f,1.0f)}
};

static inline const Theme* GetAllThemes(size_t* outCount) {
    *outCount = sizeof(ALL_THEMES) / sizeof(ALL_THEMES[0]);
    return ALL_THEMES;
}


#endif /* D6591F0F_033F_4A41_A433_55E52BFEF893 */
