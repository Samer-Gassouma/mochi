#!/usr/bin/env python3
"""
GIF to Monochrome Bitmap Converter for ESP32 Mochi Display
Converts animated GIFs to C array format for SSD1306 OLED displays

Usage:
    python3 gif2bitmap.py input.gif [options]
    python3 gif2bitmap.py gif/ --batch  # Convert all GIFs in folder

Output: C header files with bitmap arrays compatible with Adafruit_GFX
"""

import os
import sys
from PIL import Image, ImageSequence, ImageOps
import argparse
from pathlib import Path

class GifToBitmapConverter:
    def __init__(self, width=128, height=64, threshold=128):
        """
        Initialize converter
        
        Args:
            width: Target width in pixels (default 128 for SSD1306)
            height: Target height in pixels (default 64 for SSD1306)
            threshold: Brightness threshold for black/white conversion (0-255)
        """
        self.width = width
        self.height = height
        self.threshold = threshold
    
    def process_frame(self, frame):
        """
        Process a single frame: resize, convert to monochrome, return bitmap data
        
        Args:
            frame: PIL Image object
            
        Returns:
            List of bytes representing the bitmap
        """
        # Convert to RGB if needed
        if frame.mode == 'P':
            frame = frame.convert('RGBA')
        if frame.mode == 'RGBA':
            # Create white background for transparency
            background = Image.new('RGB', frame.size, (255, 255, 255))
            background.paste(frame, mask=frame.split()[3] if len(frame.split()) > 3 else None)
            frame = background
        elif frame.mode != 'RGB':
            frame = frame.convert('RGB')
        
        # Resize maintaining aspect ratio
        frame.thumbnail((self.width, self.height), Image.Resampling.LANCZOS)
        
        # Create a new image with target size (center the resized image)
        result = Image.new('RGB', (self.width, self.height), (255, 255, 255))
        offset_x = (self.width - frame.width) // 2
        offset_y = (self.height - frame.height) // 2
        result.paste(frame, (offset_x, offset_y))
        
        # Convert to grayscale
        gray = ImageOps.grayscale(result)
        
        # Convert to pure black and white using threshold
        bw = gray.point(lambda x: 255 if x > self.threshold else 0, mode='1')
        
        # Convert to bitmap array (horizontal byte format for Adafruit_GFX)
        return self._image_to_bitmap(bw)
    
    def _image_to_bitmap(self, image):
        """
        Convert PIL Image to bitmap byte array in format compatible with Adafruit_GFX
        
        Format: Horizontal byte arrangement, MSB first
        Each byte represents 8 horizontal pixels
        """
        pixels = list(image.getdata())
        bitmap = []
        
        # Process row by row, 8 pixels at a time horizontally
        bytes_per_row = (self.width + 7) // 8
        
        for y in range(self.height):
            for byte_x in range(bytes_per_row):
                byte_val = 0
                for bit in range(8):
                    x = byte_x * 8 + bit
                    if x < self.width:
                        pixel_index = y * self.width + x
                        # White pixels = 1, Black pixels = 0 (inverted for display)
                        if pixels[pixel_index] > 0:
                            byte_val |= (1 << (7 - bit))
                bitmap.append(byte_val)
        
        return bitmap
    
    def bitmap_to_c_array(self, bitmap, name, frame_num=None):
        """
        Convert bitmap to C array string
        
        Args:
            bitmap: List of bytes
            name: Base name for the array
            frame_num: Frame number (None for single image)
            
        Returns:
            String containing C array definition
        """
        if frame_num is not None:
            array_name = f"{name}_frame{frame_num}"
        else:
            array_name = name
        
        # Format as C array
        lines = [f"const unsigned char {array_name}[] PROGMEM = {{"]
        
        # Format bytes in rows of 12
        for i in range(0, len(bitmap), 12):
            chunk = bitmap[i:i+12]
            hex_values = ', '.join(f'0x{b:02X}' for b in chunk)
            if i + 12 < len(bitmap):
                lines.append(f"  {hex_values},")
            else:
                lines.append(f"  {hex_values}")
        
        lines.append("};")
        return '\n'.join(lines)
    
    def convert_gif(self, gif_path, output_path=None, max_frames=None):
        """
        Convert GIF file to C header file
        
        Args:
            gif_path: Path to input GIF file
            output_path: Path to output .h file (auto-generated if None)
            max_frames: Maximum number of frames to extract (None = all)
            
        Returns:
            dict with conversion info
        """
        gif_path = Path(gif_path)
        if not gif_path.exists():
            raise FileNotFoundError(f"GIF file not found: {gif_path}")
        
        # Generate output path if not provided
        if output_path is None:
            output_path = gif_path.parent / f"{gif_path.stem}_bitmap.h"
        else:
            output_path = Path(output_path)
        
        # Open GIF and extract frames
        img = Image.open(gif_path)
        frames = []
        frame_durations = []
        
        print(f"Processing: {gif_path.name}")
        
        for i, frame in enumerate(ImageSequence.Iterator(img)):
            if max_frames and i >= max_frames:
                break
            
            # Get frame duration (in milliseconds)
            duration = frame.info.get('duration', 100)
            frame_durations.append(duration)
            
            # Process frame
            bitmap = self.process_frame(frame.copy())
            frames.append(bitmap)
            
            print(f"  Frame {i+1}/{img.n_frames if hasattr(img, 'n_frames') else '?'} processed (duration: {duration}ms)")
        
        # Generate C header file
        self._generate_header_file(gif_path.stem, frames, frame_durations, output_path)
        
        return {
            'input': str(gif_path),
            'output': str(output_path),
            'frames': len(frames),
            'size': f"{self.width}x{self.height}",
            'total_duration': sum(frame_durations)
        }
    
    def _generate_header_file(self, name, frames, durations, output_path):
        """
        Generate C header file with all frames
        """
        # Sanitize name for C identifier
        c_name = ''.join(c if c.isalnum() else '_' for c in name).lower()
        
        lines = []
        lines.append(f"// Auto-generated bitmap data from {name}.gif")
        lines.append(f"// Frames: {len(frames)}, Size: {self.width}x{self.height}")
        lines.append(f"// Generated by gif2bitmap.py")
        lines.append("")
        lines.append(f"#ifndef {c_name.upper()}_BITMAP_H")
        lines.append(f"#define {c_name.upper()}_BITMAP_H")
        lines.append("")
        lines.append("#include <Arduino.h>")
        lines.append("")
        lines.append(f"// Animation properties")
        lines.append(f"#define {c_name.upper()}_FRAMES {len(frames)}")
        lines.append(f"#define {c_name.upper()}_WIDTH {self.width}")
        lines.append(f"#define {c_name.upper()}_HEIGHT {self.height}")
        lines.append("")
        
        # Generate frame arrays
        for i, (bitmap, duration) in enumerate(zip(frames, durations)):
            lines.append(f"// Frame {i} (duration: {duration}ms)")
            lines.append(self.bitmap_to_c_array(bitmap, c_name, i))
            lines.append("")
        
        # Generate frame pointer array
        lines.append(f"// Array of frame pointers")
        lines.append(f"const unsigned char* {c_name}_frames[] PROGMEM = {{")
        for i in range(len(frames)):
            comma = "," if i < len(frames) - 1 else ""
            lines.append(f"  {c_name}_frame{i}{comma}")
        lines.append("};")
        lines.append("")
        
        # Generate duration array
        lines.append(f"// Frame durations in milliseconds")
        lines.append(f"const uint16_t {c_name}_durations[] PROGMEM = {{")
        duration_strs = ', '.join(str(d) for d in durations)
        lines.append(f"  {duration_strs}")
        lines.append("};")
        lines.append("")
        
        lines.append(f"#endif // {c_name.upper()}_BITMAP_H")
        lines.append("")
        
        # Write to file
        with open(output_path, 'w') as f:
            f.write('\n'.join(lines))
        
        print(f"✓ Generated: {output_path}")
        print(f"  Total size: {len(frames) * len(frames[0])} bytes")
        print(f"  Animation duration: {sum(durations)}ms")


def main():
    parser = argparse.ArgumentParser(
        description='Convert GIF files to monochrome bitmap arrays for ESP32 displays',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Convert single GIF
  python3 gif2bitmap.py mochi.gif
  
  # Convert with custom size and threshold
  python3 gif2bitmap.py mochi.gif --width 128 --height 64 --threshold 128
  
  # Convert all GIFs in folder
  python3 gif2bitmap.py gif/ --batch
  
  # Limit frames (useful for large GIFs)
  python3 gif2bitmap.py large_anim.gif --max-frames 30
        """
    )
    
    parser.add_argument('input', help='Input GIF file or directory')
    parser.add_argument('-o', '--output', help='Output .h file path (auto-generated if not specified)')
    parser.add_argument('-w', '--width', type=int, default=128, help='Target width in pixels (default: 128)')
    parser.add_argument('-H', '--height', type=int, default=64, help='Target height in pixels (default: 64)')
    parser.add_argument('-t', '--threshold', type=int, default=128, 
                       help='Brightness threshold 0-255 (default: 128). Lower = more black pixels')
    parser.add_argument('-m', '--max-frames', type=int, help='Maximum number of frames to extract')
    parser.add_argument('-b', '--batch', action='store_true', help='Batch convert all GIFs in directory')
    parser.add_argument('--output-dir', help='Output directory for batch conversion')
    
    args = parser.parse_args()
    
    # Create converter
    converter = GifToBitmapConverter(
        width=args.width,
        height=args.height,
        threshold=args.threshold
    )
    
    input_path = Path(args.input)
    
    # Batch mode
    if args.batch:
        if not input_path.is_dir():
            print(f"Error: {input_path} is not a directory")
            sys.exit(1)
        
        gif_files = sorted(input_path.glob('*.gif'))
        if not gif_files:
            print(f"No GIF files found in {input_path}")
            sys.exit(1)
        
        print(f"Found {len(gif_files)} GIF files")
        print("=" * 60)
        
        output_dir = Path(args.output_dir) if args.output_dir else input_path / 'bitmaps'
        output_dir.mkdir(exist_ok=True)
        
        results = []
        for gif_file in gif_files:
            try:
                output_file = output_dir / f"{gif_file.stem}_bitmap.h"
                result = converter.convert_gif(gif_file, output_file, args.max_frames)
                results.append(result)
                print()
            except Exception as e:
                print(f"✗ Error processing {gif_file.name}: {e}")
                print()
        
        # Summary
        print("=" * 60)
        print(f"Conversion complete! {len(results)}/{len(gif_files)} files converted")
        print(f"Output directory: {output_dir}")
        
    # Single file mode
    else:
        if not input_path.is_file():
            print(f"Error: {input_path} is not a file")
            sys.exit(1)
        
        try:
            result = converter.convert_gif(input_path, args.output, args.max_frames)
            print()
            print("=" * 60)
            print("Conversion complete!")
            print(f"Input:  {result['input']}")
            print(f"Output: {result['output']}")
            print(f"Frames: {result['frames']}")
            print(f"Size:   {result['size']}")
        except Exception as e:
            print(f"Error: {e}")
            sys.exit(1)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nConversion cancelled by user")
        sys.exit(0)

