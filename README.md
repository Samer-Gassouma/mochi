# 🎭 The Mochi - Animated Emoji Display System

An ESP32 C3 Mini based animated emoji display featuring **WiFi web control** and **74 unique animations** with smooth transitions! Based on [The Mochi project](https://themochi.huykhong.com/).

## 🎨 Features

- **🌐 WiFi Web Control** - Select animations from your phone or computer
- **😴 Idle Mode** - Natural blinking Mochi when not playing animations
- **74 Full Animations** - All your GIFs converted to bitmap arrays (12 loaded at a time)
- **Smooth Transitions** - Fade and slide effects between animations
- **Frame-Perfect Playback** - Each animation plays at its original frame rate
- **Auto-Return to Idle** - Returns to blinking mode after animation completes
- **Memory Efficient** - Animations stored in PROGMEM (Flash)
- **Easy to Extend** - Add more animations by converting GIFs

## 🔧 Hardware Requirements

### Required Components:
- **ESP32 C3 Mini** development board
- **SSD1306 OLED Display** (0.96" or 1.3", 128x64, I2C)
- **USB Type-C Cable** (data-capable for programming)

### Optional Components:
- **TTP233 Touch Sensor** (for future interactive features)
- **Buzzer** (5V passive buzzer for sound)
- **TP4056 Battery Charger Module** + 3.7V LiPo battery
- **ON/OFF Switch**

## 📌 Pin Connections

### SSD1306 OLED Display:
```
Display Pin  →  ESP32 C3 Pin
─────────────────────────────
VCC          →  3.3V
GND          →  GND
SCL          →  GPIO21
SDA          →  GPIO20
```

## 🚀 Quick Start

### 1. Install PlatformIO

If you haven't already, install [PlatformIO](https://platformio.org/)

### 2. Clone and Build

```bash
pio run --target upload
pio device monitor
```

### 3. Connect to WiFi & Control

After uploading:
1. **Connect to WiFi:** SSID `Mochi-Control`, Password `mochi123`
2. **Open browser:** Visit `http://192.168.4.1`
3. **Select animations:** Click any animation button to play it!
4. **Idle mode:** Mochi will blink naturally when not playing animations


### 3. Watch the Magic! 🎬

The display will automatically:
1. Show boot screen
2. Start playing animations
3. Cycle through all 74 animations
4. Use smooth fade/slide transitions between each

## 🎬 Animation List

The system includes 74 unique animations organized into categories:

### Basic Emotions (23)
Mochi, Smile, Happy, Love, Love2, Sleepy, Angry, Angry2, Angry3, Sad, Cry, Hehe, Laugh, Crying, Wink, LookRight, LookLeft, LookDown, Dizzy, Surprised, Scared, Squint, Smirk, UwU

### Special Expressions (14)
Devil, Devil2, DevilEyes, Cool, Music, Sing, Sing2, WaterGun, WaterGun2, Tongue, Angry4, Sarcasm, KeepUp, Flooded, Smart

### Themed Animations (9)
Samurai, DemonSlayer, Gundam, AOT, Bee, Egg, Sushi, Sakura, Yakura

### Effects & Visuals (17)
Neon, Glitch, Rainbow, Rain, Star, Police, TrafficLight, Headlight, HeadlightEX, Speed, SpeedEX, Turbo, Fast, Wheels, Pong

### Special (11)
Intro, MochiDoo, NoName, Dasai, Dasai2, Distracted, Resize, Shrink, XiKhoi, XiLua, Zero

## 📺 What You'll See

```
1. Boot screen: "The Mochi Animation System - 74 animations!"
2. First animation plays (e.g., Mochi - 30 frames)
3. Smooth fade/slide transition
4. Next animation plays (e.g., Smile - 30 frames)
5. Repeat forever through all 74 animations!
```

Each animation:
- Plays all its frames at original timing
- Displays smoothly centered on screen
- Transitions gracefully to the next animation

## 🖥️ Serial Monitor Output

Watch the animation progress in real-time:

```
╔═══════════════════════════════════════╗
║    🎭 The Mochi Animation System 🎭  ║
║      ESP32 C3 + SSD1306 Display      ║
╚═══════════════════════════════════════╝

✓ Display initialized successfully!
✓ Loaded 74 animations!

▶️  Starting with: Mochi (30 frames)

─────────────────────────────────────

Finished: Mochi (30 frames)
Now playing: Smile (30 frames)
Finished: Smile (30 frames)
Now playing: Happy (24 frames)
...
```

## 🎛️ Configuration

Edit `include/definitions.h` to customize:

```cpp
// Display pins
#define I2C_SDA 20  // GPIO20 - SDA
#define I2C_SCL 21  // GPIO21 - SCL

// Display address (0x3C or 0x3D)
#define SCREEN_ADDRESS 0x3C

// Display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
```

Edit `src/main.cpp` to change transition types or speed:

```cpp
// In performTransition() - comment out transitions you don't want
TRANSITION_FADE      // Smooth fade effect
TRANSITION_SLIDE_LEFT  // Slide animation
```

## 🔧 Adding New Animations

### Convert Your Own GIFs:

```bash
cd tools
python3 gif2bitmap.py your_animation.gif --threshold 128 --max-frames 30
```

This creates `your_animation_bitmap.h` in the output folder.

### Integrate It:

1. Move the `.h` file to `include/animations/`
2. Add to `animation_manager.h`:
```cpp
#include "animations/your_animation_bitmap.h"

// In ANIMATIONS array:
{"YourName", your_animation_frames, your_animation_durations, 
 YOUR_ANIMATION_FRAMES, YOUR_ANIMATION_WIDTH, YOUR_ANIMATION_HEIGHT}
```

3. Rebuild and upload!

## 📊 Memory Usage

- Each frame: ~1KB (128x64 = 1024 bytes)
- 30 frames: ~30KB per animation
- All stored in **Flash (PROGMEM)**, not RAM
- ESP32-C3 has 400KB RAM - plenty of space!

## 🐛 Troubleshooting

### Display not working?

1. **Check connections**: Verify SDA→GPIO20, SCL→GPIO21
2. **Check I2C address**: Try changing to `0x3D` if `0x3C` doesn't work
3. **Power**: Ensure 3.3V supply can provide enough current
4. **Serial output**: Check monitor for error messages

### Animations look weird?

1. **Threshold too high/low**: Re-convert GIFs with different `--threshold` values (80-180)
2. **Too many frames**: Use `--max-frames 30` to limit

### Out of memory?

- Reduce `--max-frames` when converting
- Remove some animations from `animation_manager.h`
- Use fewer frames per animation

## 🎨 How It Works

### Architecture:

```
┌─────────────────┐
│  animation      │  ← Manages all 74 animations
│  _manager.h     │     Includes all bitmap headers
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   main.cpp      │  ← Animation playback engine
│                 │     - Loads frames from PROGMEM
│                 │     - Handles frame timing
│                 │     - Smooth transitions
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  SSD1306        │  ← Display hardware
│  128x64 OLED    │     Shows the animations
└─────────────────┘
```

### Playback Loop:

```
1. Load animation metadata from PROGMEM
2. For each frame:
   - Load frame bitmap from PROGMEM
   - Get frame duration from PROGMEM
   - Draw to display
   - Wait for frame duration
3. When animation finishes:
   - Perform transition effect
   - Load next animation
   - Repeat!
```

## 📖 Project Structure

```
ESP32-Deauther/
├── include/
│   ├── definitions.h          # Hardware config
│   ├── animation_manager.h    # Animation database
│   └── animations/            # 74 bitmap files (188KB each)
│       ├── mochi_bitmap.h
│       ├── smile_bitmap.h
│       └── ... (72 more)
├── src/
│   └── main.cpp              # Main animation engine
├── tools/
│   ├── gif2bitmap.py         # GIF converter tool
│   └── README.md             # Converter docs
└── README.md                 # This file
```

## 🔮 Future Features

- Touch sensor to cycle animations manually
- Buzzer sounds for different emotions
- WiFi control to select animations
- Battery power with sleep mode
- Custom animation playlists
- Bluetooth control from phone

## 📝 License

This project is for educational and personal use. The Mochi concept and design are from [Huykhong's Firmware](https://themochi.huykhong.com/).

## 🤝 Contributing

Want to add more animations? 
1. Convert your GIF using the converter tool
2. Add it to the animation manager
3. Share it with the community!

---

Made with ❤️ for the maker community | **74 animations and counting!** 🎭✨
