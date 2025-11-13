# GIF to Bitmap Converter for ESP32 Mochi

This tool converts animated GIF files into monochrome bitmap arrays compatible with the SSD1306 OLED display and Adafruit_GFX library.

## Installation

Install Python dependencies:

```bash
pip3 install -r requirements.txt
```

Or install Pillow directly:

```bash
pip3 install Pillow
```

## Usage

### Convert Single GIF

```bash
python3 gif2bitmap.py mochi.gif
```

This will create `mochi_bitmap.h` with all frames converted to C arrays.

### Convert All GIFs in Folder

```bash
python3 gif2bitmap.py ../gif/ --batch
```

This will convert all GIFs in the `gif/` folder and save them to `gif/bitmaps/`.

### Custom Output Directory

```bash
python3 gif2bitmap.py ../gif/ --batch --output-dir ../include/animations
```

### Adjust Brightness Threshold

The threshold controls which pixels become black vs white (0-255):
- **Lower threshold** (e.g., 80) = More black pixels, darker image
- **Higher threshold** (e.g., 180) = More white pixels, lighter image

```bash
python3 gif2bitmap.py mochi.gif --threshold 100
```

### Limit Number of Frames

Useful for large GIFs to reduce memory usage:

```bash
python3 gif2bitmap.py large_animation.gif --max-frames 30
```

### Custom Display Size

Default is 128x64 for SSD1306, but you can change it:

```bash
python3 gif2bitmap.py mochi.gif --width 128 --height 32
```

## Examples

### Example 1: Basic Conversion

```bash
cd tools
python3 gif2bitmap.py ../gif/smile.gif
```

Output: `smile_bitmap.h`

### Example 2: Batch Convert All GIFs

```bash
python3 gif2bitmap.py ../gif/ --batch --output-dir ../include/animations
```

This creates individual header files for each GIF animation.

### Example 3: High-Contrast Conversion

```bash
python3 gif2bitmap.py ../gif/neon.gif --threshold 150
```

Use higher threshold for bright/neon GIFs to preserve detail.

### Example 4: Dark Image

```bash
python3 gif2bitmap.py ../gif/demon_slayer.gif --threshold 80
```

Use lower threshold for dark images to see more detail.

## Output Format

The generated `.h` file contains:

```cpp
// Frame bitmaps
const unsigned char mochi_frame0[] PROGMEM = { ... };
const unsigned char mochi_frame1[] PROGMEM = { ... };
// ... more frames

// Frame pointers array
const unsigned char* mochi_frames[] PROGMEM = {
  mochi_frame0,
  mochi_frame1,
  // ...
};

// Frame durations (milliseconds)
const uint16_t mochi_durations[] PROGMEM = { 100, 100, 100, ... };

// Defines
#define MOCHI_FRAMES 45      // Number of frames
#define MOCHI_WIDTH 128      // Width in pixels
#define MOCHI_HEIGHT 64      // Height in pixels
```

## Using in Your Code

```cpp
#include "mochi_bitmap.h"

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  // Play animation
  for (int i = 0; i < MOCHI_FRAMES; i++) {
    display.clearDisplay();
    
    // Get frame data from PROGMEM
    const unsigned char* frame = (const unsigned char*)pgm_read_ptr(&mochi_frames[i]);
    uint16_t duration = pgm_read_word(&mochi_durations[i]);
    
    // Draw frame
    display.drawBitmap(0, 0, frame, MOCHI_WIDTH, MOCHI_HEIGHT, WHITE);
    display.display();
    
    // Wait for frame duration
    delay(duration);
  }
}
```

## Tips

### Memory Considerations

Each frame uses: `(WIDTH * HEIGHT) / 8` bytes
- 128x64 display = 1024 bytes per frame
- 30 frames = ~30KB (fits in ESP32-C3's 400KB RAM)
- For many animations, consider storing only a few in RAM at once

### Optimization Tips

1. **Limit frames**: Use `--max-frames` for long animations
2. **Reduce size**: Some animations can be smaller (e.g., 64x64 centered)
3. **Store in PROGMEM**: Keep bitmaps in flash memory (already done in generated code)
4. **Skip frames**: Only use every 2nd or 3rd frame for smooth-enough animation

### Threshold Guidelines

Test different thresholds for best results:

| Content Type | Recommended Threshold |
|-------------|----------------------|
| Bright/Neon | 150-180 |
| Normal | 120-140 |
| Dark/Night | 80-100 |
| High Contrast | 128 (default) |

## Troubleshooting

### "No module named 'PIL'"

Install Pillow:
```bash
pip3 install Pillow
```

### Output looks too dark/light

Adjust threshold:
- Too dark? Increase threshold: `--threshold 150`
- Too light? Decrease threshold: `--threshold 100`

### GIF is too large/slow

Limit frames:
```bash
python3 gif2bitmap.py large.gif --max-frames 20
```

### Out of memory on ESP32

- Use fewer frames
- Compress animation (remove duplicate frames)
- Load frames on-demand from SD card (advanced)

## Command Reference

```
usage: gif2bitmap.py [-h] [-o OUTPUT] [-w WIDTH] [-h HEIGHT] 
                     [-t THRESHOLD] [-m MAX_FRAMES] [-b] 
                     [--output-dir OUTPUT_DIR] input

positional arguments:
  input                 Input GIF file or directory

optional arguments:
  -o, --output         Output .h file path
  -w, --width          Target width (default: 128)
  -h, --height         Target height (default: 64)
  -t, --threshold      Brightness threshold 0-255 (default: 128)
  -m, --max-frames     Maximum frames to extract
  -b, --batch          Batch convert all GIFs in directory
  --output-dir         Output directory for batch mode
```

## License

MIT License - Free to use and modify

