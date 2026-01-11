// Theme System for RetroStation
#pragma once
#include "/tmp/imgui/imgui.h"
#include <string>
#include <vector>

struct Theme {
    std::string name;
    
    // Background colors
    ImVec4 windowBg;
    ImVec4 headerBg;
    ImVec4 menuBg;
    
    // Interactive elements
    ImVec4 buttonNormal;
    ImVec4 buttonHovered;
    ImVec4 buttonActive;
    ImVec4 selectionBg;
    ImVec4 selectionGlow;
    
    // Text colors
    ImVec4 textPrimary;
    ImVec4 textSecondary;
    ImVec4 textHighlight;
    
    // Accent colors
    ImVec4 accentPrimary;
    ImVec4 accentSecondary;
    
    // Icons
    ImVec4 iconFolder;
    ImVec4 iconSettings;
    ImVec4 iconExit;
};

// Predefined themes
class ThemeManager {
public:
    static Theme DarkBlue() {
        Theme t;
        t.name = "Dark Blue (Default)";
        t.windowBg = ImVec4(0.06f, 0.06f, 0.12f, 0.95f);
        t.headerBg = ImVec4(0.08f, 0.12f, 0.20f, 1.0f);
        t.menuBg = ImVec4(0.10f, 0.12f, 0.18f, 1.0f);
        
        t.buttonNormal = ImVec4(0.15f, 0.25f, 0.45f, 1.0f);
        t.buttonHovered = ImVec4(0.20f, 0.35f, 0.60f, 1.0f);
        t.buttonActive = ImVec4(0.25f, 0.40f, 0.70f, 1.0f);
        t.selectionBg = ImVec4(0.25f, 0.40f, 0.70f, 1.0f);
        t.selectionGlow = ImVec4(0.20f, 0.35f, 0.60f, 0.5f);
        
        t.textPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        t.textSecondary = ImVec4(0.7f, 0.7f, 0.8f, 1.0f);
        t.textHighlight = ImVec4(0.8f, 0.9f, 1.0f, 1.0f);
        
        t.accentPrimary = ImVec4(0.3f, 0.6f, 1.0f, 1.0f);
        t.accentSecondary = ImVec4(0.5f, 0.8f, 1.0f, 1.0f);
        
        t.iconFolder = ImVec4(0.9f, 0.7f, 0.2f, 1.0f);
        t.iconSettings = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        t.iconExit = ImVec4(0.8f, 0.3f, 0.3f, 1.0f);
        return t;
    }
    
    static Theme RetroArchGreen() {
        Theme t;
        t.name = "RetroArch Green";
        t.windowBg = ImVec4(0.02f, 0.08f, 0.05f, 0.95f);
        t.headerBg = ImVec4(0.05f, 0.12f, 0.08f, 1.0f);
        t.menuBg = ImVec4(0.08f, 0.15f, 0.10f, 1.0f);
        
        t.buttonNormal = ImVec4(0.10f, 0.30f, 0.20f, 1.0f);
        t.buttonHovered = ImVec4(0.15f, 0.45f, 0.30f, 1.0f);
        t.buttonActive = ImVec4(0.20f, 0.55f, 0.35f, 1.0f);
        t.selectionBg = ImVec4(0.20f, 0.60f, 0.40f, 1.0f);
        t.selectionGlow = ImVec4(0.15f, 0.50f, 0.35f, 0.5f);
        
        t.textPrimary = ImVec4(0.9f, 1.0f, 0.95f, 1.0f);
        t.textSecondary = ImVec4(0.6f, 0.8f, 0.7f, 1.0f);
        t.textHighlight = ImVec4(0.7f, 1.0f, 0.8f, 1.0f);
        
        t.accentPrimary = ImVec4(0.3f, 0.9f, 0.5f, 1.0f);
        t.accentSecondary = ImVec4(0.5f, 1.0f, 0.7f, 1.0f);
        
        t.iconFolder = ImVec4(0.9f, 0.7f, 0.2f, 1.0f);
        t.iconSettings = ImVec4(0.5f, 0.7f, 0.6f, 1.0f);
        t.iconExit = ImVec4(0.8f, 0.3f, 0.3f, 1.0f);
        return t;
    }
    
    static Theme ArkOSBlue() {
        Theme t;
        t.name = "ArkOS Blue";
        t.windowBg = ImVec4(0.12f, 0.25f, 0.45f, 0.95f);
        t.headerBg = ImVec4(0.15f, 0.30f, 0.55f, 1.0f);
        t.menuBg = ImVec4(0.18f, 0.35f, 0.60f, 0.9f);
        
        t.buttonNormal = ImVec4(0.20f, 0.40f, 0.65f, 1.0f);
        t.buttonHovered = ImVec4(0.25f, 0.50f, 0.75f, 1.0f);
        t.buttonActive = ImVec4(0.30f, 0.60f, 0.85f, 1.0f);
        t.selectionBg = ImVec4(0.35f, 0.65f, 0.90f, 1.0f);
        t.selectionGlow = ImVec4(0.25f, 0.55f, 0.80f, 0.5f);
        
        t.textPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        t.textSecondary = ImVec4(0.8f, 0.85f, 0.95f, 1.0f);
        t.textHighlight = ImVec4(0.9f, 0.95f, 1.0f, 1.0f);
        
        t.accentPrimary = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);
        t.accentSecondary = ImVec4(0.6f, 0.85f, 1.0f, 1.0f);
        
        t.iconFolder = ImVec4(1.0f, 0.8f, 0.3f, 1.0f);
        t.iconSettings = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
        t.iconExit = ImVec4(0.9f, 0.4f, 0.4f, 1.0f);
        return t;
    }
    
    static Theme DarkGray() {
        Theme t;
        t.name = "Dark Gray (Minimal)";
        t.windowBg = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
        t.headerBg = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
        t.menuBg = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
        
        t.buttonNormal = ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
        t.buttonHovered = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
        t.buttonActive = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
        t.selectionBg = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
        t.selectionGlow = ImVec4(0.38f, 0.38f, 0.38f, 0.5f);
        
        t.textPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        t.textSecondary = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        t.textHighlight = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        
        t.accentPrimary = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        t.accentSecondary = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        
        t.iconFolder = ImVec4(0.9f, 0.7f, 0.2f, 1.0f);
        t.iconSettings = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        t.iconExit = ImVec4(0.8f, 0.3f, 0.3f, 1.0f);
        return t;
    }
    
    static Theme PlayStation4() {
        Theme t;
        t.name = "PlayStation 4";
        t.windowBg = ImVec4(0.02f, 0.10f, 0.18f, 0.98f);
        t.headerBg = ImVec4(0.03f, 0.13f, 0.24f, 1.0f);
        t.menuBg = ImVec4(0.04f, 0.15f, 0.28f, 0.95f);
        
        t.buttonNormal = ImVec4(0.06f, 0.20f, 0.35f, 1.0f);
        t.buttonHovered = ImVec4(0.10f, 0.30f, 0.50f, 1.0f);
        t.buttonActive = ImVec4(0.15f, 0.40f, 0.65f, 1.0f);
        t.selectionBg = ImVec4(0.00f, 0.45f, 0.85f, 1.0f);
        t.selectionGlow = ImVec4(0.00f, 0.55f, 1.00f, 0.6f);
        
        t.textPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        t.textSecondary = ImVec4(0.7f, 0.75f, 0.85f, 1.0f);
        t.textHighlight = ImVec4(0.85f, 0.92f, 1.0f, 1.0f);
        
        t.accentPrimary = ImVec4(0.00f, 0.60f, 1.00f, 1.0f);
        t.accentSecondary = ImVec4(1.00f, 0.50f, 0.10f, 1.0f); // Orange accent
        
        t.iconFolder = ImVec4(0.00f, 0.70f, 1.00f, 1.0f);
        t.iconSettings = ImVec4(0.70f, 0.75f, 0.85f, 1.0f);
        t.iconExit = ImVec4(1.00f, 0.50f, 0.10f, 1.0f);
        return t;
    }
    
    static Theme PlayStation5() {
        Theme t;
        t.name = "PlayStation 5 (Modern)";
        t.windowBg = ImVec4(0.95f, 0.95f, 0.98f, 0.98f);
        t.headerBg = ImVec4(0.92f, 0.92f, 0.96f, 1.0f);
        t.menuBg = ImVec4(0.90f, 0.90f, 0.95f, 0.95f);
        
        t.buttonNormal = ImVec4(0.85f, 0.85f, 0.92f, 1.0f);
        t.buttonHovered = ImVec4(0.75f, 0.80f, 0.90f, 1.0f);
        t.buttonActive = ImVec4(0.65f, 0.75f, 0.95f, 1.0f);
        t.selectionBg = ImVec4(0.20f, 0.50f, 1.00f, 0.25f);
        t.selectionGlow = ImVec4(0.30f, 0.60f, 1.00f, 0.15f);
        
        t.textPrimary = ImVec4(0.05f, 0.05f, 0.10f, 1.0f);
        t.textSecondary = ImVec4(0.30f, 0.30f, 0.40f, 1.0f);
        t.textHighlight = ImVec4(0.00f, 0.20f, 0.50f, 1.0f);
        
        t.accentPrimary = ImVec4(0.00f, 0.40f, 0.85f, 1.0f);
        t.accentSecondary = ImVec4(0.30f, 0.60f, 1.00f, 1.0f);
        
        t.iconFolder = ImVec4(0.20f, 0.50f, 1.00f, 1.0f);
        t.iconSettings = ImVec4(0.30f, 0.30f, 0.40f, 1.0f);
        t.iconExit = ImVec4(0.85f, 0.20f, 0.30f, 1.0f);
        return t;
    }
    
    static Theme XboxSeries() {
        Theme t;
        t.name = "Xbox Series X/S";
        t.windowBg = ImVec4(0.06f, 0.08f, 0.06f, 0.98f);
        t.headerBg = ImVec4(0.08f, 0.12f, 0.08f, 1.0f);
        t.menuBg = ImVec4(0.10f, 0.14f, 0.10f, 0.95f);
        
        t.buttonNormal = ImVec4(0.15f, 0.22f, 0.15f, 1.0f);
        t.buttonHovered = ImVec4(0.20f, 0.32f, 0.20f, 1.0f);
        t.buttonActive = ImVec4(0.25f, 0.42f, 0.25f, 1.0f);
        t.selectionBg = ImVec4(0.10f, 0.75f, 0.20f, 1.0f);
        t.selectionGlow = ImVec4(0.15f, 0.85f, 0.30f, 0.5f);
        
        t.textPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        t.textSecondary = ImVec4(0.7f, 0.8f, 0.7f, 1.0f);
        t.textHighlight = ImVec4(0.85f, 1.0f, 0.85f, 1.0f);
        
        t.accentPrimary = ImVec4(0.15f, 0.85f, 0.30f, 1.0f);
        t.accentSecondary = ImVec4(0.30f, 1.00f, 0.45f, 1.0f);
        
        t.iconFolder = ImVec4(0.90f, 0.70f, 0.20f, 1.0f);
        t.iconSettings = ImVec4(0.70f, 0.80f, 0.70f, 1.0f);
        t.iconExit = ImVec4(0.85f, 0.30f, 0.30f, 1.0f);
        return t;
    }
    
    static Theme NintendoSwitch() {
        Theme t;
        t.name = "Nintendo Switch";
        t.windowBg = ImVec4(0.14f, 0.14f, 0.14f, 0.98f);
        t.headerBg = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
        t.menuBg = ImVec4(0.20f, 0.20f, 0.20f, 0.95f);
        
        t.buttonNormal = ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
        t.buttonHovered = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
        t.buttonActive = ImVec4(0.90f, 0.10f, 0.15f, 1.0f);
        t.selectionBg = ImVec4(0.90f, 0.10f, 0.15f, 1.0f);
        t.selectionGlow = ImVec4(1.00f, 0.20f, 0.25f, 0.5f);
        
        t.textPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        t.textSecondary = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        t.textHighlight = ImVec4(1.0f, 0.95f, 0.95f, 1.0f);
        
        t.accentPrimary = ImVec4(0.90f, 0.10f, 0.15f, 1.0f);
        t.accentSecondary = ImVec4(0.20f, 0.60f, 0.90f, 1.0f); // Blue Joy-Con
        
        t.iconFolder = ImVec4(0.90f, 0.70f, 0.20f, 1.0f);
        t.iconSettings = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
        t.iconExit = ImVec4(0.90f, 0.10f, 0.15f, 1.0f);
        return t;
    }
    
    static Theme SteamDeck() {
        Theme t;
        t.name = "Steam Deck";
        t.windowBg = ImVec4(0.10f, 0.12f, 0.15f, 0.98f);
        t.headerBg = ImVec4(0.13f, 0.16f, 0.20f, 1.0f);
        t.menuBg = ImVec4(0.15f, 0.18f, 0.23f, 0.95f);
        
        t.buttonNormal = ImVec4(0.20f, 0.25f, 0.32f, 1.0f);
        t.buttonHovered = ImVec4(0.25f, 0.35f, 0.45f, 1.0f);
        t.buttonActive = ImVec4(0.35f, 0.50f, 0.70f, 1.0f);
        t.selectionBg = ImVec4(0.25f, 0.55f, 0.85f, 1.0f);
        t.selectionGlow = ImVec4(0.35f, 0.65f, 0.95f, 0.5f);
        
        t.textPrimary = ImVec4(0.95f, 0.95f, 0.98f, 1.0f);
        t.textSecondary = ImVec4(0.65f, 0.68f, 0.75f, 1.0f);
        t.textHighlight = ImVec4(0.85f, 0.92f, 1.0f, 1.0f);
        
        t.accentPrimary = ImVec4(0.25f, 0.60f, 0.90f, 1.0f);
        t.accentSecondary = ImVec4(0.40f, 0.75f, 1.00f, 1.0f);
        
        t.iconFolder = ImVec4(0.90f, 0.70f, 0.20f, 1.0f);
        t.iconSettings = ImVec4(0.65f, 0.68f, 0.75f, 1.0f);
        t.iconExit = ImVec4(0.85f, 0.35f, 0.35f, 1.0f);
        return t;
    }
    
    static void ApplyTheme(const Theme& theme) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        // Apply colors
        colors[ImGuiCol_WindowBg] = theme.windowBg;
        colors[ImGuiCol_ChildBg] = theme.menuBg;
        colors[ImGuiCol_PopupBg] = theme.menuBg;
        
        colors[ImGuiCol_Header] = theme.buttonNormal;
        colors[ImGuiCol_HeaderHovered] = theme.buttonHovered;
        colors[ImGuiCol_HeaderActive] = theme.buttonActive;
        
        colors[ImGuiCol_Button] = theme.buttonNormal;
        colors[ImGuiCol_ButtonHovered] = theme.buttonHovered;
        colors[ImGuiCol_ButtonActive] = theme.buttonActive;
        
        colors[ImGuiCol_FrameBg] = theme.menuBg;
        colors[ImGuiCol_FrameBgHovered] = theme.buttonHovered;
        colors[ImGuiCol_FrameBgActive] = theme.buttonActive;
        
        colors[ImGuiCol_Tab] = theme.buttonNormal;
        colors[ImGuiCol_TabHovered] = theme.buttonHovered;
        colors[ImGuiCol_TabActive] = theme.buttonActive;
        
        colors[ImGuiCol_TitleBg] = theme.headerBg;
        colors[ImGuiCol_TitleBgActive] = theme.headerBg;
        colors[ImGuiCol_TitleBgCollapsed] = theme.headerBg;
        
        colors[ImGuiCol_MenuBarBg] = theme.headerBg;
        colors[ImGuiCol_ScrollbarBg] = theme.menuBg;
        colors[ImGuiCol_ScrollbarGrab] = theme.buttonNormal;
        colors[ImGuiCol_ScrollbarGrabHovered] = theme.buttonHovered;
        colors[ImGuiCol_ScrollbarGrabActive] = theme.buttonActive;
        
        colors[ImGuiCol_Text] = theme.textPrimary;
        colors[ImGuiCol_TextDisabled] = theme.textSecondary;
        colors[ImGuiCol_TextSelectedBg] = theme.selectionBg;
        
        colors[ImGuiCol_Border] = theme.accentPrimary;
        colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
        
        colors[ImGuiCol_Separator] = theme.accentPrimary;
        colors[ImGuiCol_SeparatorHovered] = theme.accentSecondary;
        colors[ImGuiCol_SeparatorActive] = theme.accentSecondary;
    }
    
    static std::vector<Theme> GetAllThemes() {
        return {
            PlayStation5(),
            PlayStation4(),
            XboxSeries(),
            NintendoSwitch(),
            SteamDeck(),
            DarkBlue(),
            RetroArchGreen(),
            ArkOSBlue(),
            DarkGray()
        };
    }
};
