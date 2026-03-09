#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;


const int TRIG_ENTRY = 8;
const int ECHO_ENTRY = 9;


const int TRIG_EXIT  = 6;
const int ECHO_EXIT  = 7;


const int SERVO_PIN = 2;

const int LED_R = 3;
const int LED_Y = 4;
const int LED_G = 5;


const int SLOT_MAX = 4;
int slots = SLOT_MAX;

const int DETECT_CM = 10;


const int SERVO_STOP  = 92;   
const int SERVO_SPEED = 25;   
const int OPEN_MS  = 350;     
const int CLOSE_MS = 350;     
const bool INVERT_DIR = false; 


enum State { IDLE, ENTRY_WAIT_EXIT, EXIT_WAIT_ENTRY, WAIT_CLEAR };
State state = IDLE;

bool prevEntry = false, prevExit = false;


bool gateOpen = false;


bool fullEntryWarning = false;


long getDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 999;
  return (long)(duration * 0.034 / 2);
}

void showSlots() {
  lcd.setCursor(0, 0);
  lcd.print("WELCOME!        ");
  lcd.setCursor(0, 1);
  lcd.print("Slots Left:     ");
  lcd.setCursor(12, 1);
  lcd.print("  ");
  lcd.setCursor(12, 1);
  lcd.print(slots);
}

void showFull() {
  lcd.setCursor(0, 0);
  lcd.print("SORRY :(        ");
  lcd.setCursor(0, 1);
  lcd.print("Parking Full    ");
}


void updateTrafficLight() {
  if (fullEntryWarning) {
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    return;
  }

  if (gateOpen) { 
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_Y, LOW);
  } else {        
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
  }
}

void servoStop() {
  gateServo.write(SERVO_STOP);
}

void openGate() {
  gateOpen = true;
  updateTrafficLight();

  int cmd = INVERT_DIR ? (SERVO_STOP + SERVO_SPEED) : (SERVO_STOP - SERVO_SPEED);
  gateServo.write(cmd);
  delay(OPEN_MS);
  servoStop();
}

void closeGate() {
  gateOpen = false;
  updateTrafficLight();

  int cmd = INVERT_DIR ? (SERVO_STOP - SERVO_SPEED) : (SERVO_STOP + SERVO_SPEED);
  gateServo.write(cmd);
  delay(CLOSE_MS);
  servoStop();
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(TRIG_ENTRY, OUTPUT);
  pinMode(ECHO_ENTRY, INPUT);
  pinMode(TRIG_EXIT, OUTPUT);
  pinMode(ECHO_EXIT, INPUT);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);

  gateServo.attach(SERVO_PIN);
  servoStop();

  lcd.clear();
  showSlots();

  gateOpen = false; 
  updateTrafficLight();
}

void loop() {
  long dEntry = getDistanceCM(TRIG_ENTRY, ECHO_ENTRY);
  long dExit  = getDistanceCM(TRIG_EXIT,  ECHO_EXIT);

  bool entry = (dEntry < DETECT_CM); 
  bool exitS = (dExit  < DETECT_CM); 

  bool entryEdge = entry && !prevEntry;
  bool exitEdge  = exitS && !prevExit;

  prevEntry = entry;
  prevExit  = exitS;

 
  if (state == IDLE) {
    if (slots == 0 && entry) {
      fullEntryWarning = true;
      showFull();
    } else {
      fullEntryWarning = false;
      showSlots();
    }
    updateTrafficLight();
  }

  switch (state) {
    case IDLE:
      
      if (entryEdge) {
        if (slots > 0) {
          fullEntryWarning = false;
          openGate();
          state = ENTRY_WAIT_EXIT; 
        }
      }


      else if (exitEdge) {
        fullEntryWarning = false; 
        openGate();
        state = EXIT_WAIT_ENTRY;  
      }
      break;

   
    case ENTRY_WAIT_EXIT:
      if (exitEdge) {
        slots = max(0, slots - 1);
        closeGate();
        lcd.clear();
        showSlots();
        state = WAIT_CLEAR;
      }
      break;

   
    case EXIT_WAIT_ENTRY:
      if (entryEdge) {
        slots = min(SLOT_MAX, slots + 1);
        closeGate();
        lcd.clear();
        showSlots();
        state = WAIT_CLEAR;
      }
      break;

  
    case WAIT_CLEAR:
      if (!entry && !exitS) {
        state = IDLE;
        lcd.clear();
      }
      break;
  }

  delay(50);
}
