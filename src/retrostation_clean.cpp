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
#include <dirent.h>
#include <poll.h>
#include <errno.h>
#include <termios.h>
#include <limits.h>
#include "small_themes.h"

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
    int kbd_fds[16];
    int kbd_count;
    bool use_stdin;
    struct termios orig_term;
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
    static int swap_count = 0;
    if (swap_count < 5) {
        printf("    [SwapBuffers %d: starting memcpy of %ld bytes]\n", swap_count, ctx->screensize);
        fflush(stdout);
    }
    memcpy(ctx->fbp, ctx->backbuffer, ctx->screensize);
    if (swap_count < 5) {
        printf("    [SwapBuffers %d: memcpy done]\n", swap_count);
        fflush(stdout);
    }
    swap_count++;
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
    static int draw_log_count = 0;
    if (draw_log_count < 40) {
        printf("    [DrawIcon: at %d,%d size %dx%d]\n", x, y, targetW, targetH);
        fflush(stdout);
        draw_log_count++;
    }
    
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

int GetApproxTextHeight(float scale) {
    if (!fontBuffer) return 36; // fallback box height
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    float h = (ascent - descent) * fontScale * scale;
    return (int)(h + 0.5f);
}

int GetTextWidth(const char* text, float scale) {
    if (!fontBuffer) return (int)(strlen(text) * 18 * scale);
    int width = 0;
    const char* p = text;
    int prev = 0;
    while (*p) {
        int bytes;
        uint32_t cp = DecodeUTF8(p, &bytes);
        p += bytes;
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, cp, &advance, &lsb);
        width += (int)(advance * fontScale * scale + 0.5f);
        if (prev && cp) {
            width += (int)(stbtt_GetCodepointKernAdvance(&font, prev, cp) * fontScale * scale + 0.5f);
        }
        prev = cp;
    }
    return width;
}

// Draw text vertically centered inside a box at (x, yTop) with height boxH
void DrawTextCenteredInBox(FBContext* ctx, int x, int yTop, int boxH, const char* text, uint32_t color, float scale) {
    if (!fontBuffer) {
        int th = GetApproxTextHeight(scale);
        int ty = yTop + (boxH - th) / 2;
        DrawText(ctx, x, ty, text, color, scale);
        return;
    }
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    float ascent_px = ascent * fontScale * scale;
    float descent_px = descent * fontScale * scale; // descent may be negative
    float baseline = yTop + (boxH + ascent_px + descent_px) / 2.0f;
    DrawText(ctx, x, (int)(baseline + 0.5f), text, color, scale);
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
    // Open only the primary keyboard device to avoid any duplicates
    for (int i = 0; i < 16; i++) ctx->kbd_fds[i] = -1;
    ctx->kbd_count = 0;

    struct input_id seen[16];
    int seen_count = 0;

    int primary_fd = -1;
    char primary_path[64] = {0};
    char primary_name[128] = {0};

    // First pass: find the best keyboard device without opening permanently
    for (int i = 0; i < 32; i++) {
        char p[64];
        snprintf(p, sizeof(p), "/dev/input/event%d", i);
        int fd = open(p, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        struct input_id id;
        memset(&id, 0, sizeof(id));
        ioctl(fd, EVIOCGID, &id);

        bool dup = false;
        for (int s = 0; s < seen_count; s++) {
            if (seen[s].bustype == id.bustype && seen[s].vendor == id.vendor && seen[s].product == id.product && seen[s].version == id.version) { dup = true; break; }
        }
        if (dup) { close(fd); continue; }

        // Probe capabilities to prefer a real keyboard over gamepads
        unsigned char keybits[256];
        memset(keybits, 0, sizeof(keybits));
        ioctl(fd, EVIOCGBIT(EV_KEY, (int)sizeof(keybits)), keybits);

        unsigned char evbits[64];
        memset(evbits, 0, sizeof(evbits));
        ioctl(fd, EVIOCGBIT(0, (int)sizeof(evbits)), evbits);

        bool has_up = (keybits[KEY_UP / 8] & (1 << (KEY_UP % 8))) != 0;
        bool has_down = (keybits[KEY_DOWN / 8] & (1 << (KEY_DOWN % 8))) != 0;
        bool has_enter = (keybits[KEY_ENTER / 8] & (1 << (KEY_ENTER % 8))) != 0 || (keybits[KEY_KPENTER / 8] & (1 << (KEY_KPENTER % 8))) != 0;
        bool has_letterA = (keybits[KEY_A / 8] & (1 << (KEY_A % 8))) != 0;
        bool has_abs = (evbits[EV_ABS / 8] & (1 << (EV_ABS % 8))) != 0;

        char name[128];
        memset(name, 0, sizeof(name));
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) strncpy(name, "(unknown)", sizeof(name) - 1);

        bool looks_keyboard = has_up && has_down && has_enter && has_letterA && !has_abs;
        if (looks_keyboard && primary_fd < 0) {
            primary_fd = fd;
            strncpy(primary_path, p, sizeof(primary_path) - 1);
            strncpy(primary_name, name, sizeof(primary_name) - 1);
            if (seen_count < 16) seen[seen_count++] = id;
            // Keep this fd open, close others
        } else {
            // Not primary, close it
            close(fd);
            if (seen_count < 16) seen[seen_count++] = id;
        }
    }

    // If we found a primary keyboard, configure it
    if (primary_fd >= 0) {
        // Try exclusive grab to prevent other processes from reading duplicates
        int grab = 1;
        if (ioctl(primary_fd, EVIOCGRAB, &grab) == 0) {
            printf("EVIOCGRAB succeeded on primary device\n");
            fflush(stdout);
        }

        ctx->kbd_fds[0] = primary_fd;
        ctx->kbd_count = 1;
        printf("Primary input selected: %s (fd=%d) name=%s\n", primary_path, primary_fd, primary_name);
        fflush(stdout);
    }

    return (ctx->kbd_count > 0) ? 0 : -1;
}

void EnableStdinFallback(InputContext* ctx) {
    // For simplicity we disable stdin fallback in this build; prefer evdev devices
    ctx->use_stdin = false;
}

void DisableStdinFallback(InputContext* ctx) {
    if (ctx->use_stdin) {
        tcsetattr(STDIN_FILENO, TCSANOW, &ctx->orig_term);
        ctx->use_stdin = false;
    }
}

int main() {
    printf("=== RetroStation Starting ===\n");
    fflush(stdout);

    FBContext ctx_fb;
    InputContext input;

    if (InitFramebuffer(&ctx_fb) < 0) {
        fprintf(stderr, "Failed to init framebuffer\n");
        return 1;
    }

    if (InitInput(&input) < 0) {
        printf("Warning: No input devices opened\n");
    }
    EnableStdinFallback(&input);

    if (LoadFont("/tmp/bpg_glaho.ttf", 32.0f) < 0) {
        printf("Warning: Font not loaded, continuing without text\n");
    }

    Icon* iconNintendo = LoadIcon("/tmp/icons/Sykonist-Console-Nintendo-gray.256.png");
    Icon* iconN64 = LoadIcon("/tmp/icons/Starvingartist-Antiseptic-Videogame-Nintendo-nintendo-64.32.png");
    Icon* iconSega = LoadIcon("/tmp/icons/Sykonist-Console-Sega-Dreamcast.256.png");
    Icon* iconSettings = LoadIcon("/tmp/icons/settings.png");
    Icon* iconExit = LoadIcon("/tmp/icons/exit.png");
    Icon* iconOther = LoadIcon("/tmp/icons/other.png");

    size_t allThemeCount = 0;
    const Theme* allThemes = GetAllThemes(&allThemeCount);
    int currentThemeIndex = 0;
    Theme currentTheme = allThemes[currentThemeIndex];

    int selectedIndex = 0;
    int settingsTab = 0;
    bool showSettings = false;
    bool showROMs = false;
    int selectedROMIndex = 0;

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

    printf("RetroStation loaded with %zu themes\n", allThemeCount);
    fflush(stdout);

    bool running = true;
    int frame = 0;
    uint32_t last_key_code = 0xFFFFFFFF;
    unsigned long long last_key_ts = 0; // microseconds

    while (running) {
        // Input: poll opened evdev devices and handle EV_KEY events (keyboard + gamepad buttons)
        if (input.kbd_count > 0) {
            struct pollfd pfds[16];
            int pcount = input.kbd_count;
            for (int i = 0; i < pcount; i++) { pfds[i].fd = input.kbd_fds[i]; pfds[i].events = POLLIN; pfds[i].revents = 0; }
            int pres = poll(pfds, pcount, 0);
            if (pres > 0) {
                struct input_event ev;
                for (int i = 0; i < pcount; i++) {
                    if (pfds[i].revents & POLLIN) {
                        while (read(pfds[i].fd, &ev, sizeof(ev)) == (ssize_t)sizeof(ev)) {
                            // Only handle key press events (value=1), ignore release (value=0) and repeat (value=2)
                            if (ev.type == EV_KEY && ev.value == 1) {
                                // simple debounce: ignore same key if reported within 200ms
                                unsigned long long ts = (unsigned long long)ev.time.tv_sec * 1000000ULL + (unsigned long long)ev.time.tv_usec;
                                if (ev.code == last_key_code && (ts > last_key_ts) && (ts - last_key_ts) < 200000ULL) {
                                    printf("[Debounced] key=%u (within 200ms)\n", ev.code);
                                    fflush(stdout);
                                    continue;
                                }
                                last_key_code = ev.code;
                                last_key_ts = ts;
                                // handle key/button press
                                if (ev.code == KEY_UP) {
                                    if (showSettings && settingsTab == 3) {
                                        currentThemeIndex = (currentThemeIndex - 1 + (int)allThemeCount) % (int)allThemeCount;
                                        currentTheme = allThemes[currentThemeIndex];
                                        printf("[Key] UP -> themeIndex=%d (%s)\n", currentThemeIndex, currentTheme.name);
                                    } else if (!showSettings) {
                                        selectedIndex = (selectedIndex - 1 + menuCount) % menuCount;
                                        printf("[Key] UP -> selectedIndex=%d\n", selectedIndex);
                                    }
                                } else if (ev.code == KEY_DOWN) {
                                    if (showSettings && settingsTab == 3) {
                                        currentThemeIndex = (currentThemeIndex + 1) % (int)allThemeCount;
                                        currentTheme = allThemes[currentThemeIndex];
                                        printf("[Key] DOWN -> themeIndex=%d (%s)\n", currentThemeIndex, currentTheme.name);
                                    } else if (!showSettings) {
                                        selectedIndex = (selectedIndex + 1) % menuCount;
                                        printf("[Key] DOWN -> selectedIndex=%d\n", selectedIndex);
                                    }
                                } else if ((ev.code == KEY_LEFT || ev.code == KEY_RIGHT) && showSettings) {
                                    if (ev.code == KEY_LEFT) settingsTab = (settingsTab - 1 + settingsTabCount) % settingsTabCount;
                                    else settingsTab = (settingsTab + 1) % settingsTabCount;
                                    printf("[Key] %s -> settingsTab=%d\n", (ev.code==KEY_LEFT?"LEFT":"RIGHT"), settingsTab);
                                } else if (ev.code == KEY_ENTER || ev.code == KEY_KPENTER) {
                                    if (selectedIndex == menuCount - 2) showSettings = !showSettings;
                                    else if (selectedIndex == menuCount - 1) running = false;
                                    printf("[Key] ENTER -> showSettings=%d running=%d\n", (int)showSettings, (int)running);
                                } else if (ev.code == KEY_ESC) {
                                    if (showSettings) showSettings = false; else running = false;
                                    printf("[Key] ESC -> showSettings=%d running=%d\n", (int)showSettings, (int)running);
                                } else {
                                    // map common gamepad BTN_* keys to actions
                                    if (ev.code == BTN_A || ev.code == BTN_GAMEPAD) {
                                        if (selectedIndex == menuCount - 2) showSettings = !showSettings;
                                        else if (selectedIndex == menuCount - 1) running = false;
                                        printf("[Pad] A/GAMEPAD -> showSettings=%d running=%d\n", (int)showSettings, (int)running);
                                    } else if (ev.code == BTN_THUMBL || ev.code == BTN_TRIGGER) {
                                        // treat as back
                                        if (showSettings) showSettings = false;
                                        printf("[Pad] THUMBL/TRIGGER -> showSettings=%d\n", (int)showSettings);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Render UI
        FillRect(&ctx_fb, 0, 0, ctx_fb.width, ctx_fb.height, ColorToU32(currentTheme.windowBg));
        if (showSettings) {
            int settingsX = ctx_fb.width / 6;
            int settingsY = ctx_fb.height / 8;
            int settingsW = ctx_fb.width * 2 / 3;
            int settingsH = ctx_fb.height * 3 / 4;
            FillRect(&ctx_fb, settingsX, settingsY, settingsW, settingsH, ColorToU32(currentTheme.menuBg));
            FillRect(&ctx_fb, settingsX, settingsY, settingsW, 100, ColorToU32(currentTheme.headerBg));
            DrawTextCenteredInBox(&ctx_fb, settingsX + 40, settingsY, 100, "პარამეტრები", ColorToU32(currentTheme.textPrimary), 1.6f);

            int tabY = settingsY + 110;
            int tabW = settingsW / settingsTabCount;
            for (int i = 0; i < settingsTabCount; i++) {
                int tabX = settingsX + i * tabW;
                uint32_t tabColor = (i == settingsTab) ? ColorToU32(currentTheme.selectionBg) : ColorToU32(currentTheme.buttonNormal);
                FillRect(&ctx_fb, tabX + 10, tabY, tabW - 20, 70, tabColor);
                int tabTextY = tabY + (70 - GetApproxTextHeight(1.3f)) / 2;
                DrawText(&ctx_fb, tabX + tabW/2 - 60, tabTextY, settingsTabs[i], ColorToU32(currentTheme.textPrimary), 1.3f);
            }

            if (settingsTab == 3) {
                int themeY = tabY + 100;
                for (size_t i = 0; i < allThemeCount; i++) {
                    uint32_t themeColor = (i == (size_t)currentThemeIndex) ? ColorToU32(currentTheme.selectionBg) : ColorToU32(currentTheme.buttonNormal);
                    int itemY = themeY + (int)i * 80;
                    FillRect(&ctx_fb, settingsX + 60, itemY, settingsW - 120, 70, themeColor);
                    DrawTextCenteredInBox(&ctx_fb, settingsX + 80, itemY, 70, allThemes[i].name, ColorToU32(currentTheme.textPrimary), 1.2f);
                }
            }
        } else {
            FillRect(&ctx_fb, 0, 0, ctx_fb.width, 140, ColorToU32(currentTheme.headerBg));
            DrawTextCenteredInBox(&ctx_fb, ctx_fb.width / 20, 0, 140, "რეტრო სადგური", ColorToU32(currentTheme.textHighlight), 2.2f);

            int menuY = 200;
            int itemHeight = 150;
            int itemSpacing = 35;
            int itemWidth = ctx_fb.width - ctx_fb.width / 5;
            int itemX = ctx_fb.width / 20;

            for (int i = 0; i < menuCount; i++) {
                int y = menuY + i * (itemHeight + itemSpacing);
                uint32_t bgColor = (i == selectedIndex) ? ColorToU32(currentTheme.selectionBg) : ColorToU32(currentTheme.buttonNormal);
                if (i == selectedIndex) FillRect(&ctx_fb, itemX - 15, y - 10, itemWidth + 30, itemHeight + 20, ColorToU32(currentTheme.selectionGlow));
                FillRect(&ctx_fb, itemX, y, itemWidth, itemHeight, bgColor);

                ImVec4 iconColor = currentTheme.iconFolder;
                if (i == menuCount - 2) iconColor = currentTheme.iconSettings;
                if (i == menuCount - 1) iconColor = currentTheme.iconExit;

                int iconX = itemX + 50;
                int iconY = y + 25;
                int iconSize = 110;

                if (i == 0 && iconNintendo) DrawIcon(&ctx_fb, iconNintendo, iconX, iconY, iconSize, iconSize);
                else if (i == 1 && iconNintendo) DrawIcon(&ctx_fb, iconNintendo, iconX, iconY, iconSize, iconSize);
                else if (i == 2 && iconN64) DrawIcon(&ctx_fb, iconN64, iconX, iconY, iconSize, iconSize);
                else if ((i == 3 || i == 4 || i == 5 || i == 6) && iconSega) DrawIcon(&ctx_fb, iconSega, iconX, iconY, iconSize, iconSize);
                else if (i == menuCount - 2 && iconSettings) DrawIcon(&ctx_fb, iconSettings, iconX, iconY, iconSize, iconSize);
                else if (i == menuCount - 1) { if (iconExit) DrawIcon(&ctx_fb, iconExit, iconX, iconY, iconSize, iconSize); else DrawExitIcon(&ctx_fb, iconX, iconY, iconSize, ColorToU32(iconColor)); }
                else if (iconOther) DrawIcon(&ctx_fb, iconOther, iconX, iconY, iconSize, iconSize);
                else DrawGamepadIcon(&ctx_fb, iconX, iconY, iconSize, ColorToU32(iconColor));

                int textX = iconX + iconSize + 40;
                DrawTextCenteredInBox(&ctx_fb, textX, y, itemHeight, menuItems[i], ColorToU32(currentTheme.textPrimary), 1.8f);
            }

            FillRect(&ctx_fb, 0, ctx_fb.height - 100, ctx_fb.width, 100, ColorToU32(currentTheme.headerBg));
            char footer[256];
            snprintf(footer, sizeof(footer), "თემა: %s | ზემოთ/ქვემოთ: ნავიგაცია | ENTER: არჩევა", currentTheme.name);
            DrawText(&ctx_fb, ctx_fb.width / 40, ctx_fb.height - 55, footer, ColorToU32(currentTheme.textSecondary), 1.2f);
            const char* version = "V1.0.1";
            int vW = GetTextWidth(version, 1.0f);
            int margin = ctx_fb.width / 40;
            int vx = ctx_fb.width - margin - vW;
            DrawText(&ctx_fb, vx, ctx_fb.height - 55, version, ColorToU32(currentTheme.textSecondary), 1.0f);
        }

        SwapBuffers(&ctx_fb);
        usleep(16667);
        frame++;
    }

    printf("RetroStation exited after %d frames\n", frame);

    if (fontBuffer) free(fontBuffer);
    if (iconNintendo && iconNintendo->data) { stbi_image_free(iconNintendo->data); delete iconNintendo; }
    if (iconN64 && iconN64->data) { stbi_image_free(iconN64->data); delete iconN64; }
    if (iconSega && iconSega->data) { stbi_image_free(iconSega->data); delete iconSega; }
    if (iconSettings && iconSettings->data) { stbi_image_free(iconSettings->data); delete iconSettings; }
    if (iconExit && iconExit->data) { stbi_image_free(iconExit->data); delete iconExit; }
    if (iconOther && iconOther->data) { stbi_image_free(iconOther->data); delete iconOther; }

    free(ctx_fb.backbuffer);
    munmap(ctx_fb.fbp, ctx_fb.screensize);
    close(ctx_fb.fb_fd);
    for (int i = 0; i < input.kbd_count; i++) { if (input.kbd_fds[i] >= 0) close(input.kbd_fds[i]); }
    DisableStdinFallback(&input);

    return 0;
}
