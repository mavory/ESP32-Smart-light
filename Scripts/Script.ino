#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h> 

// WiFi!
const char* ST_SSID     = "Vory";
const char* ST_PASSWORD = "Pazout2604";

WebServer server(80);
Preferences preferences; // Storage instance

// PINOUT!!
#define PIN_SOUND    34
#define PIN_MOSFET   27
#define PIN_BUTTON   26
#define PIN_BUZZER   25
#define PIN_NEOPIXEL 13

#define RGB_R 18
#define RGB_G 19
#define RGB_B 21

#define NUM_LEDS 24

// ledcsssssss
#define CH_MOSFET  0
#define CH_BUZZER  1
#define CH_R       2
#define CH_G       3
#define CH_B       4

Adafruit_NeoPixel ring(NUM_LEDS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
OneButton button(PIN_BUTTON, true);

// variability
bool systemOn = true;
bool stripOn  = false;
bool ringOn   = true;
bool buzzerOn = true;
bool statusLedOn = true;
bool autoDemo = false; 

// LED Strip
int stripBrightness = 255;
int stripEffect = 0;
int stripCurrentPWM = 0;
unsigned long stripEffectTimer = 0;
int breathVal = 0;
bool breathDir = true;

// NeoPixel Ring
int neoBrightness = 100;
int neoMode = 0;
const int NEO_MODES_COUNT = 12; 
unsigned long neoTimer = 0;
uint16_t neoStep = 0;
unsigned long demoTimer = 0;

// Clap clap clap clap
unsigned long clapWindowStart = 0;
unsigned long lastClapTime = 0;
int clapCount = 0;
unsigned long ignoreMicUntil = 0;
const unsigned long CLAP_WINDOW = 700;
const unsigned long CLAP_DEBOUNCE = 120;

// Audio & Feedback
unsigned long beepEnd = 0;
bool beeping = false;

// RGB Status 
unsigned long feedbackTimer = 0;
uint8_t fbR=0, fbG=0, fbB=0; 

// HTML web kod
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <title>Smart Room</title>
  <style>
    :root { --primary: #007bff; --bg: #f0f2f5; --card: #ffffff; --text: #333; }
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; background-color: var(--bg); color: var(--text); margin: 0; padding: 10px; text-align: center; }
    .container { max-width: 500px; margin: 0 auto; padding-bottom: 50px; }
    
    h1 { color: var(--primary); font-weight: 200; letter-spacing: 1px; margin: 20px 0; }
    
    .card { background: var(--card); padding: 20px; border-radius: 20px; box-shadow: 0 8px 30px rgba(0,0,0,0.08); margin-bottom: 15px; transition: transform 0.2s; }
    .card:active { transform: scale(0.98); }
    .card h2 { margin: 0 0 15px 0; font-size: 1.1rem; color: #888; text-transform: uppercase; letter-spacing: 1px; border-bottom: 1px solid #eee; padding-bottom: 10px; text-align: left;}
    
    .row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 12px; }
    .label { font-weight: 500; font-size: 1rem; }
    
    /* IOS Style Switch */
    .switch { position: relative; display: inline-block; width: 50px; height: 28px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #e4e4e4; transition: .3s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 22px; width: 22px; left: 3px; bottom: 3px; background-color: white; transition: .3s; border-radius: 50%; box-shadow: 0 2px 5px rgba(0,0,0,0.2); }
    input:checked + .slider { background-color: var(--primary); }
    input:checked + .slider:before { transform: translateX(22px); }
    
    /* Inputs */
    input[type=range] { width: 100%; -webkit-appearance: none; height: 6px; border-radius: 5px; background: #ddd; outline: none; margin-top: 10px; }
    input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 22px; height: 22px; border-radius: 50%; background: var(--primary); cursor: pointer; box-shadow: 0 2px 6px rgba(0,123,255,0.4); }
    
    select { width: 100%; padding: 12px; border-radius: 12px; border: 1px solid #eee; background: #f9f9f9; font-size: 1rem; color: #333; outline: none; -webkit-appearance: none; }
    
    .status-badge { display: inline-block; padding: 4px 8px; border-radius: 4px; font-size: 0.7rem; color: white; background: #ccc; }
    .status-badge.conn { background: #28a745; }
    
    /* Demo Button */
    .demo-btn { background: linear-gradient(45deg, #ff9a9e 0%, #fad0c4 99%, #fad0c4 100%); border: none; padding: 10px 20px; color: white; border-radius: 50px; font-weight: bold; cursor: pointer; width: 100%; }
    .demo-btn.active { background: linear-gradient(45deg, #a18cd1 0%, #fbc2eb 100%); }

  </style>
</head>
<body>
  <div class="container">
    <h1>SMART ROOM <span id="connInd" class="status-badge">...</span></h1>

    <div class="card" style="border-left: 5px solid var(--primary);">
      <div class="row">
        <span class="label">MASTER SWITCH</span>
        <label class="switch"><input type="checkbox" id="sysSw" onchange="send('sys', this.checked)"><span class="slider"></span></label>
      </div>
    </div>

    <div class="card">
      <h2>Ring Light</h2>
      <div class="row">
        <span class="label">Status</span>
        <label class="switch"><input type="checkbox" id="ringSw" onchange="send('ring', this.checked)"><span class="slider"></span></label>
      </div>
      <div class="row">
        <select id="ringMode" onchange="sendVal('neoMode', this.value)">
          <option value="0">Orange (Relax)</option>
          <option value="1">Blue Radar</option>
          <option value="2">Rainbow (Flow)</option>
          <option value="3">Sparkle</option>
          <option value="4">Warning (Red Wipe)</option>
          <option value="5">Theater</option>
          <option value="6">Breathe (Purple)</option>
          <option value="7">Police</option>
          <option value="8">Matrix (Green)</option>
          <option value="9">Fire</option>
          <option value="10">Comet</option>
          <option value="11">Disco Random</option>
        </select>
      </div>
      <input type="range" id="ringBri" min="0" max="255" onchange="sendVal('neoBri', this.value)">
    </div>

    <div class="card">
      <h2>LED Strip</h2>
      <div class="row">
        <span class="label">Status</span>
        <label class="switch"><input type="checkbox" id="stripSw" onchange="send('strip', this.checked)"><span class="slider"></span></label>
      </div>
      <div class="row">
        <select id="stripEff" onchange="sendVal('stripEff', this.value)">
          <option value="0">Static light</option>
          <option value="1">Breathing (Fade)</option>
          <option value="2">Strobe</option>
        </select>
      </div>
      <input type="range" id="stripBri" min="0" max="255" onchange="sendVal('stripBri', this.value)">
    </div>

    <div class="card">
      <h2>Features</h2>
      <div class="row">
        <span class="label">Button beeps</span>
        <label class="switch"><input type="checkbox" id="buzSw" onchange="send('buz', this.checked)"><span class="slider"></span></label>
      </div>
      <div class="row">
        <span class="label">Status LED</span>
        <label class="switch"><input type="checkbox" id="rgbSw" onchange="send('rgb', this.checked)"><span class="slider"></span></label>
      </div>
      <div class="row" style="margin-top:15px;">
        <button id="demoBtn" class="demo-btn" onclick="toggleDemo()">AUTO DEMO MODE</button>
      </div>
    </div>
  </div>

<script>
  let isDragging = false;
  
  // Prevents slider updates while dragging
  document.querySelectorAll('input[type=range]').forEach(el => {
    el.addEventListener('mousedown', () => isDragging = true);
    el.addEventListener('mouseup', () => isDragging = false);
    el.addEventListener('touchstart', () => isDragging = true);
    el.addEventListener('touchend', () => isDragging = false);
  });

  function send(target, state) {
    fetch('/set?target=' + target + '&state=' + (state ? '1' : '0'));
  }
  
  function sendVal(target, val) {
    fetch('/set?target=' + target + '&val=' + val);
  }

  function toggleDemo() {
    fetch('/set?target=demo&state=toggle');
  }

  // Main AJAX loop - runs every 500ms
  function updateStatus() {
    fetch('/status')
      .then(response => response.json())
      .then(data => {
        document.getElementById('connInd').className = 'status-badge conn';
        document.getElementById('connInd').innerText = 'ONLINE';

        // Update switches only if user is not interacting
        if (document.activeElement.type !== 'checkbox') {
           document.getElementById('sysSw').checked = data.sys;
           document.getElementById('ringSw').checked = data.ring;
           document.getElementById('stripSw').checked = data.strip;
           document.getElementById('buzSw').checked = data.buz;
           document.getElementById('rgbSw').checked = data.rgb;
        }

        // Update sliders only when not dragging
        if (!isDragging) {
          document.getElementById('ringBri').value = data.nBri;
          document.getElementById('stripBri').value = data.sBri;
        }

        // Selects
        if (document.activeElement.tagName !== 'SELECT') {
           document.getElementById('ringMode').value = data.nMode;
           document.getElementById('stripEff').value = data.sEff;
        }

        // Demo button 
        const dBtn = document.getElementById('demoBtn');
        if (data.demo) {
          dBtn.classList.add('active');
          dBtn.innerText = "DEMO RUNNING...";
        } else {
          dBtn.classList.remove('active');
          dBtn.innerText = "AUTO DEMO MODE";
        }
      })
      .catch(() => {
        document.getElementById('connInd').className = 'status-badge';
        document.getElementById('connInd').innerText = 'OFFLINE';
      });
  }

  setInterval(updateStatus, 500); // Fast update
  window.onload = updateStatus;
</script>
</body>
</html>
)rawliteral";

// rgb dioda
void hwRGB(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(CH_R, r);
  ledcWrite(CH_G, g);
  ledcWrite(CH_B, b);
}

// zase rgb dioda
void handleStatusLed() {
  // dark (cerna)
  if (!statusLedOn) {
    hwRGB(0, 0, 0);
    return;
  }

  // 2. feedback
  if (millis() < feedbackTimer) {
    hwRGB(fbR, fbG, fbB);
    return;
  }

  // 3. WiFi
  if (WiFi.status() != WL_CONNECTED) {
    // Blinking blue
    bool blink = (millis() / 500) % 2;
    hwRGB(0, 0, blink ? 255 : 0);
    return;
  }

  // 4. Normal 
  if (systemOn) {
    // Power On 
    hwRGB(0, 5, 0); 
  } else {
    // Power Off 
    hwRGB(5, 0, 0);
  }
}

void triggerFeedback(uint8_t r, uint8_t g, uint8_t b) {
  fbR = r; fbG = g; fbB = b;
  feedbackTimer = millis() + 200; // 200 milise-"kund"
}

// audio 
void beep(int freq, int dur) {
  if (!buzzerOn || !systemOn) return;
  ledcWriteTone(CH_BUZZER, freq);
  beepEnd = millis() + dur;
  beeping = true;
  ignoreMicUntil = beepEnd + 250;
}

void handleBeep() {
  if (beeping && millis() > beepEnd) {
    ledcWriteTone(CH_BUZZER, 0);
    beeping = false;
  }
}

// saving 
void saveSettings() {
  preferences.putBool("sys", systemOn);
  preferences.putBool("ring", ringOn);
  preferences.putBool("strip", stripOn);
  preferences.putInt("nMode", neoMode);
  preferences.putInt("nBri", neoBrightness);
  preferences.putInt("sBri", stripBrightness);
}

// tlačítko
void btnClick() {
  if(!systemOn) return;
  stripOn = !stripOn;
  triggerFeedback(stripOn ? 0 : 255, stripOn ? 255 : 0, 0);
  beep(1000, 70);
  saveSettings();
}

void btnDouble() {
  if(!systemOn) return;
  stripBrightness = (stripBrightness >= 200) ? 50 : stripBrightness + 50;
  triggerFeedback(0, 0, 255); // Blue
  beep(1200, 60);
  saveSettings();
}

void btnTriple() {
  if(!systemOn) return;
  neoMode = (neoMode + 1) % NEO_MODES_COUNT;
  triggerFeedback(150, 0, 150); // Purple
  beep(1400, 60);
  saveSettings();
}

void btnLong() {
  systemOn = !systemOn;
  triggerFeedback(255, 255, 0); // Yellow
  if(systemOn) beep(2000, 100);
  else {
     beep(500, 300);
     ring.clear(); ring.show();
     ledcWrite(CH_MOSFET, 0);
  }
  saveSettings();
}

// clap kontrola
void handleClap() {
  if (!systemOn || digitalRead(PIN_BUTTON) == LOW) return;

  unsigned long now = millis();
  if (digitalRead(PIN_SOUND) == HIGH && now > ignoreMicUntil) {
    if (now - lastClapTime > CLAP_DEBOUNCE) {
      lastClapTime = now;
      if (clapCount == 0) clapWindowStart = now;
      clapCount++;
    }
  }

  if (clapCount > 0 && now - clapWindowStart > CLAP_WINDOW) {
    if (clapCount == 1) btnClick();
    else if (clapCount == 2) { ringOn = !ringOn; saveSettings(); }
    else if (clapCount == 3) btnTriple();
    clapCount = 0;
  }
}

// striptérka led
void handleStrip() {
  if (!systemOn || !stripOn) {
    if (stripCurrentPWM > 0) {
      stripCurrentPWM = max(0, stripCurrentPWM - 5);
      ledcWrite(CH_MOSFET, stripCurrentPWM);
    }
    return;
  }

  int target = stripBrightness;
  
  if (stripEffect == 0) { // Static
    if (stripCurrentPWM < target) stripCurrentPWM++;
    else if (stripCurrentPWM > target) stripCurrentPWM--;
    ledcWrite(CH_MOSFET, stripCurrentPWM);
  } 
  else if (stripEffect == 1) { // Breathe
    if (millis() - stripEffectTimer > 10) {
      stripEffectTimer = millis();
      if (breathDir) {
        breathVal++; if (breathVal >= target) breathDir = false;
      } else {
        breathVal--; if (breathVal <= 10) breathDir = true;
      }
      ledcWrite(CH_MOSFET, breathVal);
      stripCurrentPWM = breathVal;
    }
  }
  else if (stripEffect == 2) { // Strobe
    if (millis() - stripEffectTimer > 100) {
      stripEffectTimer = millis();
      breathDir = !breathDir;
      ledcWrite(CH_MOSFET, breathDir ? target : 0);
    }
  }
}

// neopixel ring
uint32_t wheel(byte p) {
  p = 255 - p;
  if (p < 85) return ring.Color(255 - p * 3, 0, p * 3);
  if (p < 170) { p -= 85; return ring.Color(0, p * 3, 255 - p * 3); }
  p -= 170; return ring.Color(p * 3, 255 - p * 3, 0);
}

// low taper fade
void fadeToBlack(uint8_t amt) {
  for(int i=0; i<NUM_LEDS; i++) {
    uint32_t c = ring.getPixelColor(i);
    int r = (c >> 16) & 0xFF;
    int g = (c >> 8) & 0xFF;
    int b = c & 0xFF;

    r = (r <= 10) ? 0 : (int)r - (r * amt / 256);
    g = (g <= 10) ? 0 : (int)g - (g * amt / 256);
    b = (b <= 10) ? 0 : (int)b - (b * amt / 256);
    
    ring.setPixelColor(i, ring.Color(r, g, b));
  }
}

void updateNeo() {
  // Demo Mode 
  if (autoDemo && systemOn) {
    if (millis() - demoTimer > 10000) { //10se-"kund"
      demoTimer = millis();
      neoMode = random(0, NEO_MODES_COUNT);
      ringOn = true;
    }
  }

  if (!systemOn || !ringOn) {
    ring.clear(); ring.show(); return;
  }

  int delayTime = 40;
  if (neoMode == 2) delayTime = 20;
  if (neoMode == 7) delayTime = 80;
  if (neoMode == 9) delayTime = 60; // Fireee!!!
  if (neoMode == 11) delayTime = 200; // Disco

  if (millis() - neoTimer < delayTime) return;
  neoTimer = millis();

  ring.setBrightness(neoBrightness);

  switch (neoMode) {
    case 0: for(int i=0; i<NUM_LEDS; i++) ring.setPixelColor(i, ring.Color(255, 60, 0)); break; // Orange
    case 1: // Blue Radar
      fadeToBlack(40); 
      ring.setPixelColor(neoStep % NUM_LEDS, ring.Color(0, 100, 255));
      neoStep++;
      break;
    case 2: // Rainbow
      for(int i=0; i<NUM_LEDS; i++) ring.setPixelColor(i, wheel((i * 256 / NUM_LEDS + neoStep) & 255));
      neoStep++;
      break;
    case 3: // Sparkle
      fadeToBlack(20); 
      if(random(10) > 6) ring.setPixelColor(random(NUM_LEDS), ring.Color(255, 255, 255));
      break;
    case 4: // Red Wipe
      ring.setPixelColor(neoStep % NUM_LEDS, ring.Color(255, 0, 0));
      ring.setPixelColor((neoStep+1) % NUM_LEDS, 0);
      neoStep++;
      break;
    case 5: // Theater
      for (int i=0; i<NUM_LEDS; i++) ring.setPixelColor(i, 0);
      for (int i=0; i<NUM_LEDS; i+=3) ring.setPixelColor((i+neoStep)%NUM_LEDS, ring.Color(200, 0, 200));
      neoStep++;
      break;
    case 6: // Breathe Purple
      { float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
        for(int i=0; i<NUM_LEDS; i++) ring.setPixelColor(i, ring.Color((int)val, 0, (int)val)); }
      break;
    case 7: // Police
      for(int i=0; i<NUM_LEDS; i++) ring.setPixelColor(i, (neoStep%2==0) ? (i<NUM_LEDS/2 ? ring.Color(255,0,0) : 0) : (i>=NUM_LEDS/2 ? ring.Color(0,0,255) : 0));
      neoStep++;
      break;
    case 8: // Matrix Green
      fadeToBlack(30); 
      if(random(10)>5) ring.setPixelColor(random(NUM_LEDS), ring.Color(0, 255, 0));
      break;
    case 9: // Fire
      for(int i=0; i<NUM_LEDS; i++) {
        int r = random(150, 255); int g = random(0, 80);
        ring.setPixelColor(i, ring.Color(r, g, 0));
      }
      break;
    case 10: // Comet
      fadeToBlack(60); 
      ring.setPixelColor(neoStep % NUM_LEDS, ring.Color(255, 255, 255));
      neoStep++;
      break;
    case 11: // Disco Random
      for(int i=0; i<NUM_LEDS; i++) ring.setPixelColor(i, ring.Color(random(255), random(255), random(255)));
      break;
  }
  ring.show();
}

// web server
void handleRoot() { server.send(200, "text/html", index_html); }

void handleSet() {
  if (server.hasArg("target")) {
    String t = server.arg("target");
    if (server.hasArg("state")) {
      bool s = (server.arg("state") == "1");
      if (t == "sys") systemOn = s;
      else if (t == "ring") ringOn = s;
      else if (t == "strip") stripOn = s;
      else if (t == "buz") buzzerOn = s;
      else if (t == "rgb") statusLedOn = s;
      else if (t == "demo") autoDemo = !autoDemo; // demo
    }
    if (server.hasArg("val")) {
      int v = server.arg("val").toInt();
      if (t == "neoMode") { neoMode = v; neoStep = 0; ring.clear(); }
      else if (t == "neoBri") neoBrightness = v;
      else if (t == "stripBri") stripBrightness = v;
      else if (t == "stripEff") stripEffect = v;
    }
    saveSettings(); // Save 
    server.send(200, "text/plain", "OK");
  } else server.send(400, "text/plain", "ERR");
}

void handleStatus() {
  String json = "{";
  json += "\"sys\":" + String(systemOn) + ",";
  json += "\"ring\":" + String(ringOn) + ",";
  json += "\"strip\":" + String(stripOn) + ",";
  json += "\"buz\":" + String(buzzerOn) + ",";
  json += "\"rgb\":" + String(statusLedOn) + ",";
  json += "\"demo\":" + String(autoDemo) + ",";
  json += "\"nBri\":" + String(neoBrightness) + ",";
  json += "\"sBri\":" + String(stripBrightness) + ",";
  json += "\"nMode\":" + String(neoMode) + ",";
  json += "\"sEff\":" + String(stripEffect);
  json += "}";
  server.send(200, "application/json", json);
}

// seťup
void setup() {
  Serial.begin(115200);
  preferences.begin("light-app", false);

  // data
  systemOn = preferences.getBool("sys", true);
  stripOn = preferences.getBool("strip", false);
  ringOn = preferences.getBool("ring", true);
  neoMode = preferences.getInt("nMode", 0);
  neoBrightness = preferences.getInt("nBri", 100);
  stripBrightness = preferences.getInt("sBri", 200);

  pinMode(PIN_SOUND, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  ledcSetup(CH_MOSFET, 5000, 8); ledcAttachPin(PIN_MOSFET, CH_MOSFET);
  ledcSetup(CH_BUZZER, 2000, 8); ledcAttachPin(PIN_BUZZER, CH_BUZZER);
  ledcSetup(CH_R, 5000, 8); ledcAttachPin(RGB_R, CH_R);
  ledcSetup(CH_G, 5000, 8); ledcAttachPin(RGB_G, CH_G);
  ledcSetup(CH_B, 5000, 8); ledcAttachPin(RGB_B, CH_B);

  button.attachClick(btnClick);
  button.attachDoubleClick(btnDouble);
  button.attachMultiClick(btnTriple);
  button.attachLongPressStart(btnLong);
  button.setPressMs(1000);

  ring.begin();
  ring.show();

  WiFi.begin(ST_SSID, ST_PASSWORD);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    triggerFeedback(0, 0, 255); // Blink blue
    handleStatusLed(); 
    delay(500);
    hwRGB(0,0,0); delay(100); // Darkness between blinks
    tries++;
  }

  if(WiFi.status() == WL_CONNECTED) {
    triggerFeedback(0, 255, 0); // ok
    beep(2000, 200);
  } else {
    triggerFeedback(255, 0, 0); // error
  }

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.on("/status", handleStatus);
  server.begin();
}

// loooooppppp
void loop() {
  server.handleClient();
  button.tick();
  
  handleBeep();
  handleClap();
  handleStrip();
  updateNeo();
  handleStatusLed(); // RGB Function
}