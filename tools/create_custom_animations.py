#!/usr/bin/env python3
"""
Custom Animation Creator for ESP32 Mochi Display
Creates unique bitmap animations from scratch
"""

import math
from PIL import Image, ImageDraw, ImageFont
import os

# Animation parameters
WIDTH = 128
HEIGHT = 64
FRAMES = 30
OUTPUT_DIR = "custom_gifs"

def create_wink_animation():
    """Create a winking face animation"""
    print("Creating WINK animation...")
    frames = []
    
    for i in range(FRAMES):
        img = Image.new('RGB', (WIDTH, HEIGHT), 'white')
        draw = ImageDraw.Draw(img)
        
        # Draw face circle
        face_x, face_y = WIDTH // 2, HEIGHT // 2
        face_radius = 28
        draw.ellipse([face_x - face_radius, face_y - face_radius,
                     face_x + face_radius, face_y + face_radius],
                    outline='black', fill='white', width=2)
        
        # Left eye (always open)
        left_eye_x = face_x - 12
        eye_y = face_y - 8
        draw.ellipse([left_eye_x - 4, eye_y - 4, left_eye_x + 4, eye_y + 4],
                    fill='black')
        
        # Right eye (winks)
        right_eye_x = face_x + 12
        wink_cycle = (i % 30) / 30.0
        
        if 0.3 < wink_cycle < 0.5:  # Winking
            # Draw wink line
            draw.line([right_eye_x - 5, eye_y, right_eye_x + 5, eye_y],
                     fill='black', width=2)
        else:  # Open
            draw.ellipse([right_eye_x - 4, eye_y - 4, right_eye_x + 4, eye_y + 4],
                        fill='black')
        
        # Smile
        smile_y = face_y + 8
        draw.arc([face_x - 15, smile_y - 8, face_x + 15, smile_y + 8],
                start=0, end=180, fill='black', width=2)
        
        frames.append(img)
    
    return frames


def create_dizzy_animation():
    """Create spinning dizzy eyes animation"""
    print("Creating DIZZY animation...")
    frames = []
    
    for i in range(FRAMES):
        img = Image.new('RGB', (WIDTH, HEIGHT), 'white')
        draw = ImageDraw.Draw(img)
        
        # Draw face circle
        face_x, face_y = WIDTH // 2, HEIGHT // 2
        face_radius = 28
        draw.ellipse([face_x - face_radius, face_y - face_radius,
                     face_x + face_radius, face_y + face_radius],
                    outline='black', fill='white', width=2)
        
        # Spiral eyes
        angle = (i / FRAMES) * 2 * math.pi
        
        # Left spiral eye
        left_center_x = face_x - 12
        eye_y = face_y - 8
        for j in range(8):
            spiral_angle = angle + j * math.pi / 4
            radius = j * 2
            x = left_center_x + int(math.cos(spiral_angle) * radius)
            y = eye_y + int(math.sin(spiral_angle) * radius)
            draw.ellipse([x - 1, y - 1, x + 1, y + 1], fill='black')
        
        # Right spiral eye
        right_center_x = face_x + 12
        for j in range(8):
            spiral_angle = -angle + j * math.pi / 4
            radius = j * 2
            x = right_center_x + int(math.cos(spiral_angle) * radius)
            y = eye_y + int(math.sin(spiral_angle) * radius)
            draw.ellipse([x - 1, y - 1, x + 1, y + 1], fill='black')
        
        # Wavy mouth (dizzy)
        mouth_y = face_y + 10
        points = []
        for x in range(-15, 16, 2):
            y = mouth_y + int(math.sin((x + i * 3) * 0.3) * 3)
            points.append((face_x + x, y))
        
        if len(points) > 1:
            draw.line(points, fill='black', width=2)
        
        frames.append(img)
    
    return frames


def create_cool_animation():
    """Create sunglasses sliding on animation"""
    print("Creating COOL animation...")
    frames = []
    
    for i in range(FRAMES):
        img = Image.new('RGB', (WIDTH, HEIGHT), 'white')
        draw = ImageDraw.Draw(img)
        
        # Draw face circle
        face_x, face_y = WIDTH // 2, HEIGHT // 2
        face_radius = 28
        draw.ellipse([face_x - face_radius, face_y - face_radius,
                     face_x + face_radius, face_y + face_radius],
                    outline='black', fill='white', width=2)
        
        # Sunglasses animation (slide down)
        progress = min(1.0, i / 15.0)  # First half of animation
        glasses_y = face_y - 20 + int(progress * 12)
        
        # Left lens
        left_x = face_x - 12
        draw.rectangle([left_x - 8, glasses_y - 5, left_x + 8, glasses_y + 5],
                      fill='black')
        
        # Right lens
        right_x = face_x + 12
        draw.rectangle([right_x - 8, glasses_y - 5, right_x + 8, glasses_y + 5],
                      fill='black')
        
        # Bridge
        draw.line([left_x + 8, glasses_y, right_x - 8, glasses_y],
                 fill='black', width=2)
        
        # Cool smirk
        smirk_y = face_y + 10
        draw.arc([face_x - 10, smirk_y - 5, face_x + 12, smirk_y + 5],
                start=0, end=180, fill='black', width=2)
        
        frames.append(img)
    
    return frames


def create_fire_animation():
    """Create fire emoji animation"""
    print("Creating FIRE animation...")
    frames = []
    
    for i in range(FRAMES):
        img = Image.new('RGB', (WIDTH, HEIGHT), 'white')
        draw = ImageDraw.Draw(img)
        
        center_x, center_y = WIDTH // 2, HEIGHT // 2
        
        # Animated flame shape
        phase = (i / FRAMES) * 2 * math.pi
        
        # Main flame body
        flame_points = []
        for angle_deg in range(0, 360, 15):
            angle = math.radians(angle_deg)
            # Add flicker effect
            flicker = math.sin(phase + angle * 3) * 3
            radius = 25 + flicker + abs(math.sin(angle * 2)) * 5
            
            x = center_x + int(math.cos(angle) * radius)
            y = center_y + int(math.sin(angle) * radius * 0.8) - 5
            flame_points.append((x, y))
        
        # Draw outer flame
        if len(flame_points) > 2:
            draw.polygon(flame_points, outline='black', fill='white', width=2)
        
        # Inner flame detail
        inner_points = []
        for angle_deg in range(0, 360, 20):
            angle = math.radians(angle_deg)
            flicker = math.sin(phase * 1.5 + angle * 2) * 2
            radius = 15 + flicker
            
            x = center_x + int(math.cos(angle) * radius)
            y = center_y + int(math.sin(angle) * radius * 0.8)
            inner_points.append((x, y))
        
        if len(inner_points) > 2:
            draw.polygon(inner_points, outline='black', width=1)
        
        frames.append(img)
    
    return frames


def create_explode_animation():
    """Create mind blown explosion animation"""
    print("Creating EXPLODE animation...")
    frames = []
    
    for i in range(FRAMES):
        img = Image.new('RGB', (WIDTH, HEIGHT), 'white')
        draw = ImageDraw.Draw(img)
        
        face_x, face_y = WIDTH // 2, HEIGHT // 2
        face_radius = 28
        
        progress = i / FRAMES
        
        if progress < 0.5:
            # Normal face getting shocked
            draw.ellipse([face_x - face_radius, face_y - face_radius,
                         face_x + face_radius, face_y + face_radius],
                        outline='black', fill='white', width=2)
            
            # Wide shocked eyes
            eye_size = 3 + int(progress * 10)
            draw.ellipse([face_x - 12 - eye_size, face_y - 8 - eye_size,
                         face_x - 12 + eye_size, face_y - 8 + eye_size],
                        fill='black')
            draw.ellipse([face_x + 12 - eye_size, face_y - 8 - eye_size,
                         face_x + 12 + eye_size, face_y - 8 + eye_size],
                        fill='black')
            
            # Mouth opening
            mouth_size = int(progress * 15)
            draw.ellipse([face_x - mouth_size, face_y + 8 - mouth_size // 2,
                         face_x + mouth_size, face_y + 8 + mouth_size // 2],
                        fill='black')
        else:
            # Explosion effect
            explosion_progress = (progress - 0.5) * 2
            
            # Draw face (slightly shaking)
            shake_x = int(math.sin(i * 2) * 2)
            shake_y = int(math.cos(i * 2) * 2)
            
            draw.ellipse([face_x - face_radius + shake_x, face_y - face_radius + shake_y,
                         face_x + face_radius + shake_x, face_y + face_radius + shake_y],
                        outline='black', fill='white', width=2)
            
            # Explosion lines radiating out
            num_lines = 12
            for j in range(num_lines):
                angle = (j / num_lines) * 2 * math.pi
                length = face_radius + explosion_progress * 20
                
                start_x = face_x + int(math.cos(angle) * face_radius)
                start_y = face_y + int(math.sin(angle) * face_radius)
                end_x = face_x + int(math.cos(angle) * length)
                end_y = face_y + int(math.sin(angle) * length)
                
                draw.line([start_x, start_y, end_x, end_y], fill='black', width=2)
                
                # Add small explosion dots
                if explosion_progress > 0.5:
                    dot_x = end_x + int(math.cos(angle) * 5)
                    dot_y = end_y + int(math.sin(angle) * 5)
                    draw.ellipse([dot_x - 2, dot_y - 2, dot_x + 2, dot_y + 2],
                                fill='black')
        
        frames.append(img)
    
    return frames


def save_as_gif(frames, filename):
    """Save frames as animated GIF"""
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    output_path = os.path.join(OUTPUT_DIR, filename)
    
    frames[0].save(
        output_path,
        save_all=True,
        append_images=frames[1:],
        duration=60,  # 60ms per frame
        loop=0
    )
    
    print(f"  Saved: {output_path}")
    return output_path


def main():
    """Create all custom animations"""
    print("\n" + "="*50)
    print("Creating Custom Animations for ESP32 Mochi")
    print("="*50 + "\n")
    
    animations = {
        'wink.gif': create_wink_animation,
        'dizzy.gif': create_dizzy_animation,
        'cool.gif': create_cool_animation,
        'fire.gif': create_fire_animation,
        'explode.gif': create_explode_animation,
    }
    
    created_gifs = []
    
    for filename, create_func in animations.items():
        frames = create_func()
        gif_path = save_as_gif(frames, filename)
        created_gifs.append(gif_path)
    
    print("\n" + "="*50)
    print(f"✅ Created {len(created_gifs)} new animations!")
    print("="*50)
    print("\nNext steps:")
    print("1. Convert to bitmaps:")
    for gif_path in created_gifs:
        basename = os.path.basename(gif_path).replace('.gif', '')
        print(f"   python3 tools/gif2bitmap.py {gif_path} -o include/animations/{basename}_bitmap.h")
    
    print("\n2. Add to animation_manager.h")
    print("3. Recompile and upload!")
    
    return created_gifs


if __name__ == '__main__':
    main()

