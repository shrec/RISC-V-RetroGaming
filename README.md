# RetroStation 🎮

**ქართულენოვანი რეტრო კონსოლების ლაუნჩერი**  
*StarFive VisionFive 2 RISC-V Board*

## ✨ Features

- 🇬🇪 **Full Georgian UI** with BPG Glaho Traditional font
- 🎨 **9 Console Themes** (PS5, PS4, Xbox, Nintendo Switch, Steam Deck, RetroArch, ArkOS)
- 🖼️ **High-quality PNG icons**
- ⚡ **Direct Framebuffer rendering** - 60 FPS @ 4K (3840x2160)
- 🕹️ **Keyboard control** (gamepad support coming soon)
- 💾 **Lightweight** - 160KB binary, minimal dependencies

## 📦 Structure

```
retrostation/
├── src/
│   ├── retrostation.cpp    # Main application (570 lines)
│   └── themes.h             # 9 theme definitions
├── lib/
│   ├── stb_image.h          # PNG loader (276KB)
│   └── stb_truetype.h       # TrueType font renderer (194KB)
├── assets/
│   ├── icons/               # Console PNG icons
│   │   ├── Sykonist-Console-Nintendo-gray.256.png
│   │   ├── Starvingartist-Antiseptic-Videogame-Nintendo-nintendo-64.32.png
│   │   ├── Sykonist-Console-Sega-Dreamcast.256.png
│   │   └── settings.png
│   └── fonts/
│       └── bpg_glaho.ttf    # BPG Glaho Traditional (177KB)
├── build.sh                 # Build script
└── README.md
```

## 🚀 Building

```bash
cd retrostation
chmod +x build.sh
./build.sh
```

Or manually:
```bash
g++ -o retrostation src/retrostation.cpp -Ilib -Isrc -lm -O2
```

## 🎮 Running

Make sure you have access to input devices:
```bash
sudo usermod -aG input $USER
```

Then run:
```bash
sudo ./retrostation
```

## ⌨️ Controls

| Key | Action |
|-----|--------|
| ↑/↓ | Navigate menu |
| ←/→ | Change theme / Switch tabs |
| ENTER | Select |
| ESC | Back / Exit |

## 🎨 Themes

1. **PlayStation 5** - Default blue theme
2. **PlayStation 4** - Classic blue
3. **Xbox Series** - Green
4. **Nintendo Switch** - Red/Blue
5. **Steam Deck** - Dark blue
6. **RetroArch** - Classic green
7. **ArkOS** - Light blue
8. **Dark Blue** - Deep blue
9. **Dark Gray** - Monochrome

## 🖥️ Supported Consoles

- 🎮 Nintendo Famicom (NES)
- 🎮 Super Nintendo (SNES)
- 🎮 Nintendo 64
- 🎮 Sega Mega Drive / Genesis
- 🎮 Sega Master System
- 💿 Sega Mega CD
- 💿 SNK Neo Geo CD

## 🔧 Technical Details

- **Platform**: StarFive VisionFive 2 (RISC-V RV64 @ 1.5GHz)
- **OS**: Debian trixie with StarFive kernel 6.1
- **Compiler**: GCC 14.2.0+
- **Rendering**: Direct framebuffer (/dev/fb0) with double buffering
- **Resolution**: 3840x2160 @ 32bpp (4K)
- **Font Rendering**: stb_truetype (TrueType)
- **Image Loading**: stb_image (PNG with alpha blending)
- **Input**: Linux input subsystem (/dev/input/event*)
- **FPS**: 60 (16.667ms frame time)
- **Binary Size**: ~160KB

## 📝 Dependencies

```bash
# System packages
sudo apt install build-essential

# Required devices
/dev/fb0      # Framebuffer
/dev/input/*  # Keyboard/Gamepad
```

## 🛠️ Development

Built with:
- **stb_image.h** by Sean Barrett (Public Domain)
- **stb_truetype.h** by Sean Barrett (Public Domain)
- **BPG Glaho Traditional** Georgian font
- Pure C++ with minimal dependencies
- Direct Linux API calls (no SDL/X11)

## 🐛 Known Issues

- PowerVR GPU driver broken on StarFive VisionFive 2
- SDL2/X11 not available in TTY mode
- Gamepad support not implemented yet

## 📄 License

Open Source - Free to use 🎮

## 🙏 Credits

- **Icons**: [IconArchive.com](https://www.iconarchive.com/)
- **Font**: BPG Fonts ([bpgfonts.wordpress.com](https://bpgfonts.wordpress.com/))
- **stb libraries**: Sean Barrett
- **Development**: GitHub Copilot 🤖

---

**Created for StarFive VisionFive 2 RISC-V Development**  
*January 2026*
