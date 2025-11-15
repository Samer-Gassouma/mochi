#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <Arduino.h>

// ===== CURATED SELECTION: Most Fun & Interesting Animations =====
// Optimized for ESP32-C3 (1.3MB flash) - ALL with FULL frames!
// Carefully selected for variety: emotions, effects, characters, and fun!

#include "animations/0_bitmap.h"           // IDLE - Default blinking animation
#include "animations/mochi_bitmap.h"       // Classic Mochi character
#include "animations/love_bitmap.h"        // Heart eyes - so cute!
#include "animations/smile_bitmap.h"       // Classic happy smile
#include "animations/scared_bitmap.h"      // Surprised/scared expression
#include "animations/sleepy_bitmap.h"      // Cute sleepy face
#include "animations/devil_bitmap.h"       // Mischievous devil face
#include "animations/rainbow_bitmap.h"     // Colorful rainbow effect
#include "animations/star_bitmap.h"        // Sparkly star effect
#include "animations/sushi_bitmap.h"       // Fun sushi animation
#include "animations/music_bitmap.h"       // Vibing to music notes

// ===== NEW CUSTOM ANIMATIONS =====
#include "animations/wink_bitmap.h"        // 😉 Winking face
#include "animations/dizzy_bitmap.h"       // 😵 Spinning dizzy eyes
#include "animations/cool_bitmap.h"        // 😎 Sunglasses sliding on
#include "animations/fire_bitmap.h"        // 🔥 Animated fire emoji
#include "animations/explode_bitmap.h"     // 🤯 Mind blown explosion

// Want different animations? See SELECT_ANIMATIONS.md for the full list!
// Swap out any of the above includes with other animations from the animations/ folder

// Animation structure
struct Animation {
  const char* name;
  const unsigned char* const * frames;  // Pointer to frame array in PROGMEM
  const uint16_t* durations;            // Pointer to duration array in PROGMEM
  uint8_t frameCount;
  uint8_t width;
  uint8_t height;
};

// ===== ONLY NEW CUSTOM ANIMATIONS =====
const Animation ANIMATIONS[] PROGMEM = {
  // Idle is first (index 0) - Default animation
  {"Idle", idle_0_frames, idle_0_durations, IDLE_0_FRAMES, IDLE_0_WIDTH, IDLE_0_HEIGHT},
  
  // ===== NEW CUSTOM ANIMATIONS ONLY =====
  {"Wink", wink_frames, wink_durations, WINK_FRAMES, WINK_WIDTH, WINK_HEIGHT},
  {"Dizzy", dizzy_frames, dizzy_durations, DIZZY_FRAMES, DIZZY_WIDTH, DIZZY_HEIGHT},
  {"Cool", cool_frames, cool_durations, COOL_FRAMES, COOL_WIDTH, COOL_HEIGHT},
  {"Fire", fire_frames, fire_durations, FIRE_FRAMES, FIRE_WIDTH, FIRE_HEIGHT},
  {"Explode", explode_frames, explode_durations, EXPLODE_FRAMES, EXPLODE_WIDTH, EXPLODE_HEIGHT}
  
  // ===== ORIGINAL ANIMATIONS (DISABLED) =====
  // Uncomment below to re-enable original animations
  /*
  {"Mochi", mochi_frames, mochi_durations, MOCHI_FRAMES, MOCHI_WIDTH, MOCHI_HEIGHT},
  {"Love", love_frames, love_durations, LOVE_FRAMES, LOVE_WIDTH, LOVE_HEIGHT},
  {"Smile", smile_frames, smile_durations, SMILE_FRAMES, SMILE_WIDTH, SMILE_HEIGHT},
  {"Scared", scared_frames, scared_durations, SCARED_FRAMES, SCARED_WIDTH, SCARED_HEIGHT},
  {"Sleepy", sleepy_frames, sleepy_durations, SLEEPY_FRAMES, SLEEPY_WIDTH, SLEEPY_HEIGHT},
  {"Devil", devil_frames, devil_durations, DEVIL_FRAMES, DEVIL_WIDTH, DEVIL_HEIGHT},
  {"Rainbow", rainbow_frames, rainbow_durations, RAINBOW_FRAMES, RAINBOW_WIDTH, RAINBOW_HEIGHT},
  {"Star", star_frames, star_durations, STAR_FRAMES, STAR_WIDTH, STAR_HEIGHT},
  {"Sushi", sushi_frames, sushi_durations, SUSHI_FRAMES, SUSHI_WIDTH, SUSHI_HEIGHT},
  {"Music", music_frames, music_durations, MUSIC_FRAMES, MUSIC_WIDTH, MUSIC_HEIGHT}
  */
};

const uint8_t ANIMATION_COUNT = sizeof(ANIMATIONS) / sizeof(Animation);

// ===== CUSTOMIZATION =====
// Want different animations? Easy!
// 1. Comment out an #include above
// 2. Add a new #include "animations/YOUR_CHOICE_bitmap.h"
// 3. Update the ANIMATIONS array entry
// 4. Recompile!
//
// See SELECT_ANIMATIONS.md for all 74 available animations

#endif
