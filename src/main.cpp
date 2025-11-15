#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "definitions.h"
#include "animation_manager.h"

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create web server
WebServer server(80);

// Animation playback state
struct MochiAnimState {
  uint8_t currentAnimation;     // Current animation index (0 = idle)
  uint8_t currentFrame;         // Current frame index
  unsigned long lastFrameTime;  // Timestamp of last frame change
  unsigned long nextChangeTime; // When to change to next animation
};

MochiAnimState animState = {0, 0, 0, 0};

// ===== DISPLAY FUNCTIONS =====

bool initDisplay() {
  Wire.begin(I2C_SDA, I2C_SCL);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    DEBUG_PRINTLN(F("SSD1306 allocation failed"));
    return false;
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 15);
  display.println(F("The Mochi"));
  display.setTextSize(1);
  display.setCursor(10, 40);
  display.println(F("Web Control"));
  display.display();
  delay(2000);
  
  return true;
}

// Get animation from PROGMEM
Animation getAnimation(uint8_t index) {
  Animation anim;
  memcpy_P(&anim, &ANIMATIONS[index], sizeof(Animation));
  return anim;
}

// Draw animation frame (full screen 128x64)
void drawAnimationFrame(uint8_t animIndex, uint8_t frameIndex) {
  Animation anim = getAnimation(animIndex);
  
  // Safety check
  if (frameIndex >= anim.frameCount) {
    DEBUG_PRINT("ERROR: Frame ");
    DEBUG_PRINT(frameIndex);
    DEBUG_PRINT(" >= ");
    DEBUG_PRINTLN(anim.frameCount);
    return;
  }
  
  const unsigned char* frame = (const unsigned char*)pgm_read_ptr(&anim.frames[frameIndex]);
  
  // Check if frame pointer is valid
  if (frame == nullptr) {
    DEBUG_PRINT("ERROR: Null frame pointer at index ");
    DEBUG_PRINTLN(frameIndex);
    return;
  }
  
  display.clearDisplay();
  display.drawBitmap(0, 0, frame, anim.width, anim.height, SSD1306_WHITE);
  display.display();
}

// ===== ANIMATION UPDATE =====

void updateAnimation() {
  unsigned long currentTime = millis();
  Animation anim = getAnimation(animState.currentAnimation);
  
  uint16_t frameDuration = pgm_read_word(&anim.durations[animState.currentFrame]);
  
  if (currentTime - animState.lastFrameTime >= frameDuration) {
    animState.lastFrameTime = currentTime;
    animState.currentFrame++;
    
    // Loop current animation continuously (full frames, no cutting!)
    if (animState.currentFrame >= anim.frameCount) {
      animState.currentFrame = 0;  // Loop back to start
      
      // Debug: Animation completed loop
      #ifdef SERIAL_DEBUG
      char name[32];
      strcpy_P(name, (char*)pgm_read_ptr(&anim.name));
      DEBUG_PRINT("Animation loop: ");
      DEBUG_PRINT(name);
      DEBUG_PRINT(" (");
      DEBUG_PRINT(anim.frameCount);
      DEBUG_PRINTLN(" frames)");
      #endif
    }
    
    drawAnimationFrame(animState.currentAnimation, animState.currentFrame);
  }
}

// ===== WEB SERVER HANDLERS =====

void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width'><title>Mochi Control</title>";
  html += "<style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:'Segoe UI',Arial,sans-serif;";
  html += "background:linear-gradient(135deg,#0a0a0a 0%,#1a0505 50%,#0f0000 100%);min-height:100vh;padding:20px;color:#fff}";
  html += ".container{max-width:600px;margin:0 auto}h1{text-align:center;font-size:2.8em;margin-bottom:5px;";
  html += "color:#ff3333;text-shadow:0 0 20px rgba(255,51,51,0.5),0 0 40px rgba(255,0,0,0.3)}";
  html += ".subtitle{text-align:center;margin-bottom:25px;opacity:0.85;font-size:1.1em;color:#ff6666}";
  html += ".status{background:rgba(255,0,0,0.15);backdrop-filter:blur(10px);";
  html += "padding:20px;border-radius:15px;margin-bottom:20px;text-align:center;font-size:1.3em;";
  html += "box-shadow:0 8px 32px rgba(255,0,0,0.3),inset 0 0 30px rgba(255,0,0,0.1);";
  html += "border:2px solid rgba(255,51,51,0.4)}";
  html += ".controls{background:rgba(20,0,0,0.6);backdrop-filter:blur(10px);";
  html += "padding:25px;border-radius:15px;box-shadow:0 8px 32px rgba(0,0,0,0.5);border:1px solid rgba(255,51,51,0.2)}";
  html += ".btn{display:block;width:100%;padding:15px;margin:10px 0;border:none;border-radius:12px;";
  html += "font-size:1.1em;font-weight:bold;cursor:pointer;color:#fff;transition:all 0.3s ease;";
  html += "box-shadow:0 4px 15px rgba(255,0,0,0.4)}";
  html += ".btn:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(255,0,0,0.6)}";
  html += ".btn:active{transform:translateY(2px);box-shadow:0 2px 10px rgba(255,0,0,0.3)}";
  html += ".btn-idle{background:linear-gradient(135deg,#cc0000,#ff3333,#cc0000);font-size:1.2em;padding:18px;";
  html += "border:2px solid rgba(255,51,51,0.5);box-shadow:0 4px 20px rgba(255,0,0,0.5),inset 0 0 20px rgba(255,51,51,0.2)}";
  html += ".btn-anim{background:linear-gradient(135deg,#8b0000,#cc0000);border:1px solid rgba(255,0,0,0.3)}";
  html += ".animation-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(140px,1fr));";
  html += "gap:12px;margin-top:15px}.anim-btn{padding:15px;font-size:1em}";
  html += "h3{margin:20px 0 15px 0;font-size:1.3em;color:#ff4444;opacity:0.95;text-shadow:0 0 10px rgba(255,68,68,0.3)}";
  html += ".section-header{border-bottom:2px solid rgba(255,51,51,0.4);padding-bottom:8px;margin-bottom:15px}";
  html += "</style></head><body><div class='container'>";
  html += "<h1>Mochi Control Panel</h1>";
  html += "<p class='subtitle'>NEW Custom Animations Only! (6 Total)</p>";
  html += "<div class='status' id='status'>Current: Idle (Blinking)</div>";
  html += "<div class='controls'>";
  html += "<button class='btn btn-idle' onclick='setIdle()'>Return to Idle</button>";
  html += "<h3 class='section-header'>All Animations (" + String(ANIMATION_COUNT) + ")</h3>";
  html += "<div class='animation-grid'>";
  
  // Add animation buttons
  for (uint8_t i = 0; i < ANIMATION_COUNT; i++) {
    Animation anim;
    memcpy_P(&anim, &ANIMATIONS[i], sizeof(Animation));
    char name[32];
    strcpy_P(name, (char*)pgm_read_ptr(&anim.name));
    
    html += "<button class='btn btn-anim anim-btn' onclick='playAnim(" + String(i) + ")'>";
    html += String(name) + "</button>";
  }
  
  html += "</div></div></div>";
  html += "<script>function setIdle(){fetch('/idle').then(r=>r.text()).then(t=>{";
  html += "updateStatus(t);document.querySelectorAll('.btn-anim').forEach(b=>b.style.opacity='1')})}";
  html += "function playAnim(id){fetch('/play?id='+id).then(r=>r.text()).then(t=>{";
  html += "updateStatus(t);document.querySelectorAll('.btn-anim').forEach((b,i)=>";
  html += "b.style.opacity=i==id?'0.6':'1')})}";
  html += "function updateStatus(text){document.getElementById('status').innerHTML='<strong>Now Playing:</strong><br>'+text}";
  html += "setInterval(()=>{fetch('/status').then(r=>r.text()).then(t=>";
  html += "document.getElementById('status').innerHTML='<strong>Now Playing:</strong><br>'+t)},3000);</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleIdle() {
  // Return to idle animation (index 0)
  animState.currentAnimation = 0;
  animState.currentFrame = 0;
  animState.lastFrameTime = millis();
  
  drawAnimationFrame(0, 0);
  
  server.send(200, "text/plain", "Idle");
}

void handlePlay() {
  if (!server.hasArg("id")) {
    server.send(400, "text/plain", "Missing id parameter");
    return;
  }
  
  int animId = server.arg("id").toInt();
  
  if (animId < 0 || animId >= ANIMATION_COUNT) {
    server.send(400, "text/plain", "Invalid animation id");
    return;
  }
  
  // Switch to selected animation (will loop continuously with full frames)
  animState.currentAnimation = animId;
  animState.currentFrame = 0;
  animState.lastFrameTime = millis();
  
  Animation anim = getAnimation(animId);
  char name[32];
  strcpy_P(name, (char*)pgm_read_ptr(&anim.name));
  
  // Debug info
  DEBUG_PRINT("Playing: ");
  DEBUG_PRINT(name);
  DEBUG_PRINT(" - Frames: ");
  DEBUG_PRINT(anim.frameCount);
  DEBUG_PRINT(", Size: ");
  DEBUG_PRINT(anim.width);
  DEBUG_PRINT("x");
  DEBUG_PRINTLN(anim.height);
  
  drawAnimationFrame(animId, 0);
  
  server.send(200, "text/plain", String(name));
}

void handleStatus() {
  Animation anim = getAnimation(animState.currentAnimation);
  char name[32];
  strcpy_P(name, (char*)pgm_read_ptr(&anim.name));
  
  String status = String(name) + " (Frame " + String(animState.currentFrame + 1) + "/" + String(anim.frameCount) + ")";
  
  server.send(200, "text/plain", status);
}

// ===== SETUP =====

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\n\n==========================================="));
  Serial.println(F("   Mochi Web Control System"));
  Serial.println(F("===========================================\n"));
  
  // Initialize display
  if (!initDisplay()) {
    Serial.println(F("❌ Display failed!"));
    while(1) delay(1000);
  }
  
  Serial.println(F("✓ Display initialized"));
  
  // Start WiFi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  
  IPAddress IP = WiFi.softAPIP();
  
  Serial.print(F("✓ WiFi AP started: "));
  Serial.println(WIFI_SSID);
  Serial.print(F("  Password: "));
  Serial.println(WIFI_PASSWORD);
  Serial.print(F("  IP Address: "));
  Serial.println(IP);
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/idle", handleIdle);
  server.on("/play", handlePlay);
  server.on("/status", handleStatus);
  server.begin();
  
  Serial.println(F("✓ Web server started"));
  Serial.print(F("✓ Loaded "));
  Serial.print(ANIMATION_COUNT);
  Serial.println(F(" animations (ALL with FULL frames!)"));
  Serial.println(F("\n>> Connect to WiFi and visit:"));
  Serial.print(F("   http://"));
  Serial.println(IP);
  Serial.println(F("\n─────────────────────────────────────\n"));
  
  // Start with idle animation (index 0)
  animState.currentAnimation = 0;
  animState.currentFrame = 0;
  animState.lastFrameTime = millis();
  drawAnimationFrame(0, 0);
  
  // Show WiFi info on display
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("WiFi: Mochi-Control"));
  display.println(F("Pass: mochi123"));
  display.println();
  display.print(F("IP: "));
  display.println(IP);
  display.println();
  display.println(F("Visit IP in browser"));
  display.display();
  delay(5000);
  
  drawAnimationFrame(0, 0);  // Show first frame of idle
}

// ===== MAIN LOOP =====

void loop() {
  // Handle web requests
  server.handleClient();
  
  // Update animation
  updateAnimation();
  
  delay(1);
}
