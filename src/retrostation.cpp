#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <vector>
#include "themes.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Font globals
stbtt_fontinfo font;
unsigned char* fontBuffer = nullptr;
float fontScale = 1.0f;

// Icon structure
struct Icon {
    unsigned char* data;
    int width;
    int height;
    int channels;
};

struct FBContext {
    int fb_fd;
    char* fbp;
    uint32_t* backbuffer;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long screensize;
    int width, height;
};

struct InputContext {
    int kbd_fd;
};

uint32_t ColorToU32(ImVec4 color) {
    uint8_t r = (uint8_t)(color.x * 255.0f);
    uint8_t g = (uint8_t)(color.y * 255.0f);
    uint8_t b = (uint8_t)(color.z * 255.0f);
    return (r << 16) | (g << 8) | b;
}

void FillRect(FBContext* ctx, int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h && py < ctx->height; py++) {
        for (int px = x; px < x + w && px < ctx->width; px++) {
            if (px >= 0 && py >= 0) {
                ctx->backbuffer[py * ctx->width + px] = color;
            }
        }
    }
}

void SwapBuffers(FBContext* ctx) {
    memcpy(ctx->fbp, ctx->backbuffer, ctx->screensize);
}

// Load TrueType font
int LoadFont(const char* fontPath, float pixelHeight) {
    FILE* f = fopen(fontPath, "rb");
    if (!f) {
        printf("Failed to open font: %s\n", fontPath);
        return -1;
    }
    
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    fontBuffer = (unsigned char*)malloc(size);
    fread(fontBuffer, 1, size, f);
    fclose(f);
    
    if (!stbtt_InitFont(&font, fontBuffer, 0)) {
        printf("Failed to init font\n");
        free(fontBuffer);
        fontBuffer = nullptr;
        return -1;
    }
    
    fontScale = stbtt_ScaleForPixelHeight(&font, pixelHeight);
    printf("Font loaded: %s (%.0fpx)\n", fontPath, pixelHeight);
    return 0;
}

// Load PNG icon
Icon* LoadIcon(const char* path) {
    Icon* icon = new Icon();
    icon->data = stbi_load(path, &icon->width, &icon->height, &icon->channels, 4); // Force RGBA
    if (!icon->data) {
        printf("Failed to load icon: %s\n", path);
        delete icon;
        return nullptr;
    }
    printf("Loaded icon: %s (%dx%d)\n", path, icon->width, icon->height);
    return icon;
}

// Draw PNG icon with alpha blending
void DrawIcon(FBContext* ctx, Icon* icon, int x, int y, int targetW, int targetH) {
    if (!icon || !icon->data) return;
    
    float scaleX = (float)targetW / icon->width;
    float scaleY = (float)targetH / icon->height;
    
    for (int dy = 0; dy < targetH; dy++) {
        for (int dx = 0; dx < targetW; dx++) {
            int srcX = (int)(dx / scaleX);
            int srcY = (int)(dy / scaleY);
            
            if (srcX >= icon->width || srcY >= icon->height) continue;
            
            int px = x + dx;
            int py = y + dy;
            
            if (px < 0 || px >= ctx->width || py < 0 || py >= ctx->height) continue;
            
            int idx = (srcY * icon->width + srcX) * 4;
            uint8_t r = icon->data[idx];
            uint8_t g = icon->data[idx + 1];
            uint8_t b = icon->data[idx + 2];
            uint8_t a = icon->data[idx + 3];
            
            if (a > 10) {
                uint32_t bg = ctx->backbuffer[py * ctx->width + px];
                uint8_t bgR = (bg >> 16) & 0xFF;
                uint8_t bgG = (bg >> 8) & 0xFF;
                uint8_t bgB = bg & 0xFF;
                
                float alpha = a / 255.0f;
                uint8_t outR = (uint8_t)(r * alpha + bgR * (1.0f - alpha));
                uint8_t outG = (uint8_t)(g * alpha + bgG * (1.0f - alpha));
                uint8_t outB = (uint8_t)(b * alpha + bgB * (1.0f - alpha));
                
                ctx->backbuffer[py * ctx->width + px] = (outR << 16) | (outG << 8) | outB;
            }
        }
    }
}

// Decode UTF-8 to codepoint
uint32_t DecodeUTF8(const char* text, int* bytesRead) {
    unsigned char c = text[0];
    *bytesRead = 1;
    
    if ((c & 0x80) == 0) {
        return c;
    } else if ((c & 0xE0) == 0xC0) {
        *bytesRead = 2;
        return ((c & 0x1F) << 6) | (text[1] & 0x3F);
    } else if ((c & 0xF0) == 0xE0) {
        *bytesRead = 3;
        return ((c & 0x0F) << 12) | ((text[1] & 0x3F) << 6) | (text[2] & 0x3F);
    } else if ((c & 0xF8) == 0xF0) {
        *bytesRead = 4;
        return ((c & 0x07) << 18) | ((text[1] & 0x3F) << 12) | ((text[2] & 0x3F) << 6) | (text[3] & 0x3F);
    }
    return 0;
}

// Draw text with TrueType font
void DrawText(FBContext* ctx, int x, int y, const char* text, uint32_t color, float scale) {
    if (!fontBuffer) {
        // Fallback: simple rectangles
        int len = strlen(text);
        for (int i = 0; i < len && i < 80; i++) {
            if (text[i] != ' ') {
                FillRect(ctx, x + i * 25, y, 18, 36, color);
            }
        }
        return;
    }
    
    int pen_x = x;
    int pen_y = y;
    
    const char* p = text;
    while (*p) {
        int bytesRead;
        uint32_t codepoint = DecodeUTF8(p, &bytesRead);
        p += bytesRead;
        
        if (codepoint == ' ') {
            pen_x += 15 * scale;
            continue;
        }
        
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, codepoint, &advance, &lsb);
        
        int ix0, iy0, ix1, iy1;
        stbtt_GetCodepointBitmapBox(&font, codepoint, fontScale * scale, fontScale * scale, &ix0, &iy0, &ix1, &iy1);
        
        int w = ix1 - ix0;
        int h = iy1 - iy0;
        
        if (w > 0 && h > 0) {
            unsigned char* bitmap = (unsigned char*)malloc(w * h);
            stbtt_MakeCodepointBitmap(&font, bitmap, w, h, w, fontScale * scale, fontScale * scale, codepoint);
            
            // Render with alpha blending
            for (int by = 0; by < h; by++) {
                for (int bx = 0; bx < w; bx++) {
                    unsigned char alpha = bitmap[by * w + bx];
                    if (alpha > 30) {
                        int px = pen_x + lsb * fontScale * scale + bx + ix0;
                        int py = pen_y + by + iy0;
                        
                        if (px >= 0 && px < ctx->width && py >= 0 && py < ctx->height) {
                            uint32_t bg = ctx->backbuffer[py * ctx->width + px];
                            uint8_t bgR = (bg >> 16) & 0xFF;
                            uint8_t bgG = (bg >> 8) & 0xFF;
                            uint8_t bgB = bg & 0xFF;
                            
                            uint8_t fgR = (color >> 16) & 0xFF;
                            uint8_t fgG = (color >> 8) & 0xFF;
                            uint8_t fgB = color & 0xFF;
                            
                            float t = alpha / 255.0f;
                            uint8_t outR = (uint8_t)(fgR * t + bgR * (1.0f - t));
                            uint8_t outG = (uint8_t)(fgG * t + bgG * (1.0f - t));
                            uint8_t outB = (uint8_t)(fgB * t + bgB * (1.0f - t));
                            
                            ctx->backbuffer[py * ctx->width + px] = (outR << 16) | (outG << 8) | outB;
                        }
                    }
                }
            }
            free(bitmap);
        }
        
        pen_x += advance * fontScale * scale;
    }
}

// Large gamepad icon
void DrawGamepadIcon(FBContext* ctx, int x, int y, int size, uint32_t color) {
    // Body
    FillRect(ctx, x + size/10, y + size/4, size*4/5, size/2, color);
    
    // Grips
    FillRect(ctx, x + size/10, y + size*3/5, size/5, size/3, color);
    FillRect(ctx, x + size*7/10, y + size*3/5, size/5, size/3, color);
    
    // D-pad
    int dpadX = x + size/4;
    int dpadY = y + size/2;
    FillRect(ctx, dpadX - size/16, dpadY - size/6, size/8, size/3, color);
    FillRect(ctx, dpadX - size/6, dpadY - size/16, size/3, size/8, color);
    
    // Buttons
    int btnSize = size/6;
    int btnX = x + size*3/4;
    int btnY = y + size/2;
    int btnSpacing = size/5;
    
    FillRect(ctx, btnX - btnSize/2, btnY - btnSpacing - btnSize/2, btnSize, btnSize, color);
    FillRect(ctx, btnX + btnSpacing - btnSize/2, btnY - btnSize/2, btnSize, btnSize, color);
    FillRect(ctx, btnX - btnSize/2, btnY + btnSpacing - btnSize/2, btnSize, btnSize, color);
    FillRect(ctx, btnX - btnSpacing - btnSize/2, btnY - btnSize/2, btnSize, btnSize, color);
}

// Gear icon
void DrawGearIcon(FBContext* ctx, int x, int y, int size, uint32_t color) {
    int centerX = x + size/2;
    int centerY = y + size/2;
    
    // 8 teeth
    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            FillRect(ctx, centerX - size/12, centerY - size/2, size/6, size/4, color);
            FillRect(ctx, centerX - size/12, centerY + size/4, size/6, size/4, color);
            FillRect(ctx, centerX - size/2, centerY - size/12, size/4, size/6, color);
            FillRect(ctx, centerX + size/4, centerY - size/12, size/4, size/6, color);
        }
    }
    
    // Outer ring
    FillRect(ctx, centerX - size/3, centerY - size/3, size*2/3, size*2/3, color);
    
    // Center hole
    FillRect(ctx, centerX - size/6, centerY - size/6, size/3, size/3, ColorToU32(ImVec4(0.05f, 0.05f, 0.05f, 1.0f)));
}

// Exit icon
void DrawExitIcon(FBContext* ctx, int x, int y, int size, uint32_t color) {
    // Door
    FillRect(ctx, x + size/6, y + size/8, size*2/3, size/12, color);
    FillRect(ctx, x + size/6, y + size/8, size/12, size*3/4, color);
    FillRect(ctx, x + size*2/3, y + size/8, size/12, size*3/4, color);
    
    // Arrow
    FillRect(ctx, x + size/3, y + size/2 - size/16, size/3, size/8, color);
    FillRect(ctx, x + size*2/3 - size/8, y + size/3, size/8, size/3, color);
}

// Disc icon
void DrawDiscIcon(FBContext* ctx, int x, int y, int size, uint32_t color) {
    int centerX = x + size/2;
    int centerY = y + size/2;
    
    // Outer circle
    for (int r = size/3; r < size/2; r++) {
        for (int angle = 0; angle < 360; angle += 10) {
            int px = centerX + (int)(r * cos(angle * 3.14159f / 180));
            int py = centerY + (int)(r * sin(angle * 3.14159f / 180));
            if (px >= 0 && px < ctx->width && py >= 0 && py < ctx->height) {
                ctx->backbuffer[py * ctx->width + px] = color;
            }
        }
    }
    
    // Hole
    FillRect(ctx, centerX - size/8, centerY - size/8, size/4, size/4, ColorToU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f)));
}

int InitFramebuffer(FBContext* ctx) {
    ctx->fb_fd = open("/dev/fb0", O_RDWR);
    if (ctx->fb_fd < 0) return -1;
    
    ioctl(ctx->fb_fd, FBIOGET_VSCREENINFO, &ctx->vinfo);
    ioctl(ctx->fb_fd, FBIOGET_FSCREENINFO, &ctx->finfo);
    
    ctx->width = ctx->vinfo.xres;
    ctx->height = ctx->vinfo.yres;
    ctx->screensize = ctx->vinfo.xres * ctx->vinfo.yres * (ctx->vinfo.bits_per_pixel / 8);
    
    ctx->fbp = (char*)mmap(0, ctx->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fb_fd, 0);
    if (ctx->fbp == (char*)-1) return -1;
    
    ctx->backbuffer = (uint32_t*)malloc(ctx->screensize);
    if (!ctx->backbuffer) return -1;
    
    printf("Framebuffer: %dx%d @ %dbpp (double buffered)\n", ctx->width, ctx->height, ctx->vinfo.bits_per_pixel);
    return 0;
}

int InitInput(InputContext* ctx) {
    ctx->kbd_fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    if (ctx->kbd_fd < 0) {
        ctx->kbd_fd = open("/dev/input/event1", O_RDONLY | O_NONBLOCK);
    }
    return ctx->kbd_fd;
}

int main() {
    FBContext ctx;
    InputContext input;
    
    if (InitFramebuffer(&ctx) < 0) {
        fprintf(stderr, "Failed to init framebuffer\n");
        return 1;
    }
    
    InitInput(&input);
    
    // Load BPG Glaho Georgian font
    printf("Loading BPG Glaho font...\n");
    if (LoadFont("/tmp/bpg_glaho.ttf", 32.0f) < 0) {
        printf("Warning: Font not loaded, continuing without text\n");
    }
    
    // Load PNG icons
    Icon* iconNintendo = LoadIcon("/tmp/icons/Sykonist-Console-Nintendo-gray.256.png");
    Icon* iconN64 = LoadIcon("/tmp/icons/Starvingartist-Antiseptic-Videogame-Nintendo-nintendo-64.32.png");
    Icon* iconSega = LoadIcon("/tmp/icons/Sykonist-Console-Sega-Dreamcast.256.png");
    Icon* iconSettings = LoadIcon("/tmp/icons/settings.png");
    
    // Get all themes
    ThemeManager themeManager;
    std::vector<Theme> allThemes = themeManager.GetAllThemes();
    
    int currentThemeIndex = 0; // Start with PS5
    Theme currentTheme = allThemes[currentThemeIndex];
    
    int selectedIndex = 0;
    int settingsTab = 0;
    bool showSettings = false;
    
    const char* menuItems[] = {
        "ნინტენდო ფამიკომი (NES)",
        "სუპერ ნინტენდო (SNES)",
        "ნინტენდო 64",
        "სეგა მეგა დრაივი",
        "სეგა მასტერ სისტემა",
        "სეგა მეგა CD",
        "SNK ნეო გეო CD",
        "პარამეტრები",
        "გასვლა"
    };
    int menuCount = 9;
    
    const char* settingsTabs[] = {"ჯოისტიკი", "ეკრანი", "ხმა", "თემა"};
    int settingsTabCount = 4;
    
    printf("RetroStation loaded with %d themes\n", (int)allThemes.size());
    
    bool running = true;
    int frame = 0;
    
    while (running) {
        // Input
        struct input_event ev;
        while (input.kbd_fd >= 0 && read(input.kbd_fd, &ev, sizeof(ev)) > 0) {
            if (ev.type == EV_KEY && ev.value == 1) {
                if (ev.code == KEY_UP) {
                    if (showSettings && settingsTab == 3) {
                        currentThemeIndex = (currentThemeIndex - 1 + allThemes.size()) % allThemes.size();
                        currentTheme = allThemes[currentThemeIndex];
                    } else if (!showSettings) {
                        selectedIndex = (selectedIndex - 1 + menuCount) % menuCount;
                    }
                } else if (ev.code == KEY_DOWN) {
                    if (showSettings && settingsTab == 3) {
                        currentThemeIndex = (currentThemeIndex + 1) % allThemes.size();
                        currentTheme = allThemes[currentThemeIndex];
                    } else if (!showSettings) {
                        selectedIndex = (selectedIndex + 1) % menuCount;
                    }
                } else if (ev.code == KEY_LEFT && showSettings) {
                    settingsTab = (settingsTab - 1 + settingsTabCount) % settingsTabCount;
                } else if (ev.code == KEY_RIGHT && showSettings) {
                    settingsTab = (settingsTab + 1) % settingsTabCount;
                } else if (ev.code == KEY_ENTER) {
                    if (selectedIndex == menuCount - 2) {
                        showSettings = !showSettings;
                    } else if (selectedIndex == menuCount - 1) {
                        running = false;
                    }
                } else if (ev.code == KEY_ESC) {
                    if (showSettings) {
                        showSettings = false;
                    } else {
                        running = false;
                    }
                }
            }
        }
        
        // Clear
        FillRect(&ctx, 0, 0, ctx.width, ctx.height, ColorToU32(currentTheme.windowBg));
        
        if (showSettings) {
            // Settings UI (simplified for now)
            int settingsX = ctx.width / 6;
            int settingsY = ctx.height / 8;
            int settingsW = ctx.width * 2 / 3;
            int settingsH = ctx.height * 3 / 4;
            
            FillRect(&ctx, settingsX, settingsY, settingsW, settingsH, ColorToU32(currentTheme.menuBg));
            
            // Header
            FillRect(&ctx, settingsX, settingsY, settingsW, 100, ColorToU32(currentTheme.headerBg));
            DrawText(&ctx, settingsX + 40, settingsY + 42, "პარამეტრები", ColorToU32(currentTheme.textPrimary), 1.6f);
            
            // Tabs
            int tabY = settingsY + 110;
            int tabW = settingsW / settingsTabCount;
            for (int i = 0; i < settingsTabCount; i++) {
                int tabX = settingsX + i * tabW;
                uint32_t tabColor = (i == settingsTab) ? ColorToU32(currentTheme.selectionBg) : ColorToU32(currentTheme.buttonNormal);
                FillRect(&ctx, tabX + 10, tabY, tabW - 20, 70, tabColor);
                DrawText(&ctx, tabX + tabW/2 - 60, tabY + 26, settingsTabs[i], ColorToU32(currentTheme.textPrimary), 1.3f);
            }
            
            // Content
            if (settingsTab == 3) {
                int themeY = tabY + 100;
                for (size_t i = 0; i < allThemes.size(); i++) {
                    uint32_t themeColor = (i == currentThemeIndex) ? ColorToU32(currentTheme.selectionBg) : ColorToU32(currentTheme.buttonNormal);
                    int itemY = themeY + i * 80;
                    FillRect(&ctx, settingsX + 60, itemY, settingsW - 120, 70, themeColor);
                    DrawText(&ctx, settingsX + 80, itemY + 25, allThemes[i].name.c_str(), ColorToU32(currentTheme.textPrimary), 1.2f);
                }
            }
            
        } else {
            // Main menu
            FillRect(&ctx, 0, 0, ctx.width, 140, ColorToU32(currentTheme.headerBg));
            DrawText(&ctx, ctx.width / 20, 55, "რეტრო სადგური", ColorToU32(currentTheme.textHighlight), 2.2f);
            
            int menuY = 200;
            int itemHeight = 150;
            int itemSpacing = 35;
            int itemWidth = ctx.width - ctx.width / 5;
            int itemX = ctx.width / 20;
            
            for (int i = 0; i < menuCount; i++) {
                int y = menuY + i * (itemHeight + itemSpacing);
                
                uint32_t bgColor = (i == selectedIndex) ? ColorToU32(currentTheme.selectionBg) : ColorToU32(currentTheme.buttonNormal);
                
                // Selection glow
                if (i == selectedIndex) {
                    FillRect(&ctx, itemX - 15, y - 10, itemWidth + 30, itemHeight + 20, ColorToU32(currentTheme.selectionGlow));
                }
                
                // Item bg
                FillRect(&ctx, itemX, y, itemWidth, itemHeight, bgColor);
                
                // Icon
                ImVec4 iconColor = currentTheme.iconFolder;
                if (i == menuCount - 2) iconColor = currentTheme.iconSettings;
                if (i == menuCount - 1) iconColor = currentTheme.iconExit;
                
                int iconX = itemX + 50;
                int iconY = y + 25;
                int iconSize = 110;
                
                // Use PNG icons instead of vector icons
                if (i == 0 && iconNintendo) {
                    DrawIcon(&ctx, iconNintendo, iconX, iconY, iconSize, iconSize);
                } else if (i == 1 && iconNintendo) {
                    DrawIcon(&ctx, iconNintendo, iconX, iconY, iconSize, iconSize);
                } else if (i == 2 && iconN64) {
                    DrawIcon(&ctx, iconN64, iconX, iconY, iconSize, iconSize);
                } else if ((i == 3 || i == 4 || i == 5 || i == 6) && iconSega) {
                    DrawIcon(&ctx, iconSega, iconX, iconY, iconSize, iconSize);
                } else if (i == menuCount - 2 && iconSettings) {
                    DrawIcon(&ctx, iconSettings, iconX, iconY, iconSize, iconSize);
                } else if (i == menuCount - 1) {
                    DrawExitIcon(&ctx, iconX, iconY, iconSize, ColorToU32(iconColor));
                } else {
                    // Fallback to vector icons
                    DrawGamepadIcon(&ctx, iconX, iconY, iconSize, ColorToU32(iconColor));
                }
                
                // Text (responsive position after icon)
                DrawText(&ctx, iconX + iconSize + 40, y + 58, menuItems[i], ColorToU32(currentTheme.textPrimary), 1.8f);
            }
            
            // Footer
            FillRect(&ctx, 0, ctx.height - 100, ctx.width, 100, ColorToU32(currentTheme.headerBg));
            char footer[256];
            snprintf(footer, sizeof(footer), "თემა: %s | ზემოთ/ქვემოთ: ნავიგაცია | ENTER: არჩევა", currentTheme.name.c_str());
            DrawText(&ctx, ctx.width / 40, ctx.height - 55, footer, ColorToU32(currentTheme.textSecondary), 1.2f);
        }
        
        SwapBuffers(&ctx);
        usleep(16667);
        frame++;
    }
    
    printf("RetroStation exited after %d frames\n", frame);
    
    // Cleanup
    if (fontBuffer) free(fontBuffer);
    if (iconNintendo && iconNintendo->data) { stbi_image_free(iconNintendo->data); delete iconNintendo; }
    if (iconN64 && iconN64->data) { stbi_image_free(iconN64->data); delete iconN64; }
    if (iconSega && iconSega->data) { stbi_image_free(iconSega->data); delete iconSega; }
    if (iconSettings && iconSettings->data) { stbi_image_free(iconSettings->data); delete iconSettings; }
    free(ctx.backbuffer);
    munmap(ctx.fbp, ctx.screensize);
    close(ctx.fb_fd);
    if (input.kbd_fd >= 0) close(input.kbd_fd);
    
    return 0;
}
