// ============================================
// GRAIN STORAGE RODENT DETECTION SYSTEM
// Dual Sensor: PIR Motion + Vibration Detection
// For Proteus Simulation
// Trigger Conditions:
// - 2 Motion Events OR
// - 2 Vibration Events OR
// - 1 Motion + 1 Vibration Event
// ============================================

// -------- Pin Definitions --------
const int pirPin     = 4;      // PIR motion sensor output
const int vibPin     = 5;      // Vibration sensor output
const int piezoPin   = 8;      // Ultrasonic buzzer
const int greenLED   = 6;      // System armed indicator
const int redLED     = 7;      // Rodent detected indicator

// -------- TIMING CONSTANTS --------
const unsigned long activeTime = 10000;        // 10 seconds repellent active
const unsigned long cooldownTime = 3000;       // 3 seconds between triggers

const unsigned long detectionWindow = 5000;    // 5 seconds detection window
const unsigned long debounceTime = 150;        // 150ms debounce
const unsigned long resetCounterTime = 8000;   // Reset counters after 8 seconds

// -------- State Variables --------
bool rodentActive = false;
bool inCooldown = false;
unsigned long startTime = 0;
unsigned long cooldownStart = 0;

// ------ Detection Counters --------
int motionCount = 0;
int vibCount = 0;
unsigned long lastMotionTime = 0;
unsigned long lastVibTime = 0;
unsigned long lastResetTime = 0;

// ------ Current Sensor States --------
bool pirState = LOW;
bool vibState = LOW;
bool lastPirState = LOW;
bool lastVibState = LOW;

// ------ Ultrasonic Frequency Variables --------
int freq = 22000;
int freqDirection = 1;
unsigned long lastFreqChange = 0;
const int freqChangeInterval = 40;

// ------ Status Printing --------
unsigned long lastPrint = 0;
unsigned long totalTriggers = 0;
String lastTriggerReason = "";

void setup() {
  // Initialize pins
  pinMode(pirPin, INPUT);
  pinMode(vibPin, INPUT);
  pinMode(piezoPin, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  
  Serial.begin(9600);
  
  Serial.println("=========================================");
  Serial.println("GRAIN STORAGE RODENT DETECTION SYSTEM");
  Serial.println("PIR Motion + Vibration Detection");
  Serial.println("=========================================");
  Serial.println();
  Serial.println("TRIGGER CONDITIONS:");
  Serial.println("- 2 Motion Events");
  Serial.println("- 2 Vibration Events"); 
  Serial.println("- 1 Motion + 1 Vibration");
  Serial.println();
  Serial.println("SYSTEM IS ON");
  Serial.println();
  Serial.println("TIMING SETTINGS:");
  Serial.print("- Detection Window: ");
  Serial.print(detectionWindow / 1000);
  Serial.println(" seconds");
  Serial.print("- Debounce Time: ");
  Serial.print(debounceTime);
  Serial.println(" ms");
  Serial.print("- Counter Reset Time: ");
  Serial.print(resetCounterTime / 1000);
  Serial.println(" seconds");
  Serial.println();
  Serial.println("System Status: ARMED");
  Serial.println("Green LED = Armed | Red LED = Active | Flashing = Cooldown");
  Serial.println();
  
  // NO DELAY - system arms immediately
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, LOW);
  
  // Initialize lastResetTime to current time
  lastResetTime = millis();
  
  Serial.println("System READY - Instant Start!");
  Serial.println("-----------------------------------------");
}

void loop() {
  // READ SENSORS DIRECTLY
  pirState = digitalRead(pirPin);
  vibState = digitalRead(vibPin);
  
  // DEBUG: Print raw sensor values every 3 seconds
  static unsigned long lastDebugPrint = 0;
  if(millis() - lastDebugPrint > 3000) {
    Serial.print("STATUS - Motion: ");
    Serial.print(motionCount);
    Serial.print(" | Vibration: ");
    Serial.print(vibCount);
    Serial.print(" | Window: ");
    Serial.print((millis() - lastResetTime) / 1000);
    Serial.print("s/");
    Serial.print(resetCounterTime / 1000);
    Serial.print("s | PIR: ");
    Serial.print(pirState);
    Serial.print(" | VIB: ");
    Serial.println(vibState);
    lastDebugPrint = millis();
  }
  
  // PROCESS PIR MOTION DETECTION (rising edge detection)
  if(pirState == HIGH && lastPirState == LOW) {
    // Check debounce
    if(millis() - lastMotionTime > debounceTime) {
      motionCount++;
      lastMotionTime = millis();
      Serial.print(">>> MOTION! Total: ");
      Serial.print(motionCount);
      Serial.print(" | Time in window: ");
      Serial.print((millis() - lastResetTime) / 1000);
      Serial.println("s");
    }
  }
  
  // PROCESS VIBRATION DETECTION (rising edge detection)
  if(vibState == HIGH && lastVibState == LOW) {
    // Check debounce
    if(millis() - lastVibTime > debounceTime) {
      vibCount++;
      lastVibTime = millis();
      Serial.print(">>> VIBRATION! Total: ");
      Serial.print(vibCount);
      Serial.print(" | Time in window: ");
      Serial.print((millis() - lastResetTime) / 1000);
      Serial.println("s");
    }
  }
  
  // Store last states for edge detection
  lastPirState = pirState;
  lastVibState = vibState;
  
  // Reset counters after resetCounterTime (8 seconds)
  if(millis() - lastResetTime >= resetCounterTime) {
    if(motionCount > 0 || vibCount > 0) {
      Serial.println("--- Counter reset after 8 seconds ---");
    }
    motionCount = 0;
    vibCount = 0;
    lastResetTime = millis();
  }
  
  // Check trigger conditions (only if system is idle)
  if(!rodentActive && !inCooldown) {
    checkTriggerConditions();
  }
  
  // Handle different states
  if(rodentActive) {
    handleActiveState();
  }
  else if(inCooldown) {
    handleCooldownState();
  }
  else {
    // Idle state - green LED on
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
  }
  
  delay(20); // Small delay for stability
}

void checkTriggerConditions() {
  // Calculate time remaining in current window
  unsigned long timeInWindow = millis() - lastResetTime;
  
  // Only trigger if we're within the detection window (first 5 seconds)
  if(timeInWindow <= detectionWindow) {
    bool trigger = false;
    String reason = "";
    
    // CONDITION 1: 2 motion events
    if(motionCount >= 2) {
      trigger = true;
      reason = "2 MOTION EVENTS";
    }
    // CONDITION 2: 2 vibration events
    else if(vibCount >= 2) {
      trigger = true;
      reason = "2 VIBRATION EVENTS";
    }
    // CONDITION 3: 1 motion + 1 vibration
    else if(motionCount >= 1 && vibCount >= 1) {
      trigger = true;
      reason = "1 MOTION + 1 VIBRATION";
    }
    
    if(trigger) {
      activateSystem(reason);
    }
  }
  else {
    // Beyond detection window, don't trigger even if counts are high
    // This ensures triggering only happens in first 5 seconds
    static bool warned = false;
    if((motionCount >= 2 || vibCount >= 2 || (motionCount >=1 && vibCount >=1)) && !warned) {
      Serial.println("Events detected but outside detection window (after 5s)");
      warned = true;
    }
  }
}

void activateSystem(String reason) {
  // Activate the system
  rodentActive = true;
  startTime = millis();
  totalTriggers++;
  lastTriggerReason = reason;
  
  // Reset frequency
  freq = 22000;
  freqDirection = 1;
  
  // Print detection
  Serial.println();
  Serial.println("!!! RODENT DETECTED !!!");
  Serial.print("Trigger condition: ");
  Serial.println(reason);
  Serial.print("Motion count: ");
  Serial.print(motionCount);
  Serial.print(", Vibration count: ");
  Serial.println(vibCount);
  Serial.print("Time in window: ");
  Serial.print((millis() - lastResetTime) / 1000);
  Serial.println("s");
  Serial.print("Total triggers: ");
  Serial.println(totalTriggers);
  Serial.println("ULTRASONIC REPELLENT ACTIVATED for 10 seconds");
  Serial.println("-----------------------------------------");
  
  // Start buzzer
  tone(piezoPin, freq);
  
  // Update LEDs
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, HIGH);
}

void handleActiveState() {
  // Frequency sweep (22kHz to 32kHz and back)
  if(millis() - lastFreqChange >= freqChangeInterval) {
    freq += 150 * freqDirection;
    
    if(freq > 32000) {
      freq = 32000;
      freqDirection = -1;
    }
    else if(freq < 22000) {
      freq = 22000;
      freqDirection = 1;
    }
    
    tone(piezoPin, freq);
    lastFreqChange = millis();
  }
  
  // Print status every second
  if(millis() - lastPrint >= 1000) {
    unsigned long elapsed = millis() - startTime;
    int remaining = (activeTime / 1000) - (elapsed / 1000);
    
    Serial.print("Active: ");
    Serial.print(remaining);
    Serial.print("s | Frequency: ");
    Serial.print(freq);
    Serial.print(" Hz | ");
    Serial.println(freqDirection == 1 ? "UP" : "DOWN");
    
    lastPrint = millis();
  }
  
  // Check if active time is over
  if(millis() - startTime >= activeTime) {
    deactivateSystem();
  }
}

void deactivateSystem() {
  // Deactivate
  rodentActive = false;
  inCooldown = true;
  cooldownStart = millis();
  noTone(piezoPin);
  
  Serial.println("-----------------------------------------");
  Serial.println("Repellent cycle complete");
  Serial.println("Entering COOLDOWN for 3 seconds...");
  Serial.println();
}

void handleCooldownState() {
  // Flash LEDs during cooldown
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  if(millis() - lastBlink >= 200) {
    ledState = !ledState;
    digitalWrite(greenLED, ledState ? HIGH : LOW);
    digitalWrite(redLED, ledState ? LOW : HIGH);
    lastBlink = millis();
  }
  
  // Exit cooldown after 3 seconds
  if(millis() - cooldownStart >= cooldownTime) {
    inCooldown = false;
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    
    Serial.println("System RE-ARMED and ready");
    Serial.println("-----------------------------------------");
  }
}