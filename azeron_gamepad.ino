
/*
   "Azeron clone" Game Pad controller
   A custom made keyboard/joystick controller using HID on Arduino Pro Micro
   https://www.instructables.com/id/Azeron-Game-Pad-DIY-Under-35/
   JerkWagon & Anon Engineering @Discord 9/2020
   Rev. 091620vs
*/

//#define DEBUG // Comment out for Keyboard, uncomment for Serial debugging

#ifdef DEBUG
#define KEY_BEGIN()
#define KEY_PRESS(x)
#define KEY_RELEASE(x)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
// Key constants, numeric values used in DEBUG mode
#define KEY_LEFT_CTRL 128
#define KEY_LEFT_SHIFT 129
#define KEY_LEFT_ALT 130
#define KEY_LEFT_GUI 131
#define KEY_RIGHT_CTRL 132
#define KEY_RIGHT_SHIFT 133
#define KEY_RIGHT_ALT 134
#define KEY_RIGHT_GUI 135
#define KEY_UP_ARROW 218
#define KEY_DOWN_ARROW 217
#define KEY_LEFT_ARROW 216
#define KEY_RIGHT_ARROW 215
#define KEY_BACKSPACE 178
#define KEY_TAB 179
#define KEY_RETURN 176
#define KEY_ESC 177
#define KEY_INSERT 209
#define KEY_DELETE 212
#define KEY_PAGE_UP 211
#define KEY_PAGE_DOWN 214
#define KEY_HOME 210
#define KEY_END 213
#define KEY_CAPS_LOCK 193
#define KEY_F1 194
#define KEY_F2 195
#define KEY_F3 196
#define KEY_F4 197
#define KEY_F5 198
#define KEY_F6 199
#define KEY_F7 200
#define KEY_F8 201
#define KEY_F9 202
#define KEY_F10 203
#define KEY_F11 204
#define KEY_F12 205
#else
#include <Keyboard.h>
#define KEY_BEGIN() Keyboard.begin()
#define KEY_PRESS(x) Keyboard.press(x)
#define KEY_RELEASE(x) Keyboard.release(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

//Globals
const int xCenter = 250;    // Tweak for your D-pad's center values, should be 250
const int yCenter = 250;
const int xDeadband = 150;  // Sets the size of the center deadband
const int yDeadband = 150;

// D-pad: UP = w, RIGHT = d, DOWN = s, LEFT = a, DPadNone is "center value"
const char dPadUp = 'w';
const char dPadRt = 'd';
const char dPadDn = 's';
const char dPadLt = 'a';
const char dPadNone = '0';

typedef struct {
  int pin;
  char cmd;
  bool wasPressed;
} dKey;

// Switch pins, {PIN, KEY, False}
dKey dSwitch[] = {
  {19, 'u', false}, // Pin 19 = A1
  {2, KEY_LEFT_SHIFT, false},
  {3, '1', false},
  {4, 'f', false},
  {5, 'q', false},
  {6, '2', false},
  {7, 'e', false},
  {8, 'x', false},
  {9, KEY_TAB, false},
  {10, 't', false},
  {16, 'b', false},
  {14, 'r', false},
  {15, '3', false}
};

void setup() {
  Serial.begin(9600);
  KEY_BEGIN();
  // Pin Mode setup
  for (int i = 0; i <= 12; i++) {
    pinMode(dSwitch[i].pin, INPUT_PULLUP);
  }
  // Shows key mapping if in DEBUG
  while (!Serial);  // Wait for serial to come up
  DEBUG_PRINTLN("Key mapping: ");
  DEBUG_PRINT("Pin\t");
  DEBUG_PRINTLN("Key");
  for (int i = 0; i <= 12; i++) {
    DEBUG_PRINT(dSwitch[i].pin);
    DEBUG_PRINT('\t');
    DEBUG_PRINTLN(dSwitch[i].cmd);
  }
  DEBUG_PRINTLN();
}

void loop() {
  readJoystick();
  readKeys();
}

void readJoystick() {
  float angle = 0.0;
  static char lastKeyPressed = dPadNone;
  int joyX = analogRead(A2);  // Or A3, rotates 0 angle (0 degrees is full right by default)
  int joyY = analogRead(A3);  // Or A2, rotates 0 angle
  double mapJoyX = map(joyX, 0, 1023, 0, 500);
  double mapJoyY = map(joyY, 0, 1023, 0, 500);
  // For test, use to determine xCenter, yCenter
  //DEBUG_PRINT("Mapped X value: ");
  //DEBUG_PRINTLN(mapJoyX);
  //DEBUG_PRINT("Mapped Y value: ");
  //DEBUG_PRINTLN(mapJoyY);
  /*
    // Get joystick angle
    if X AND Y = center then angle = 1000     // In deadband
      release key(last key)
    else angle = arctangent y/x * 57.2957795  // Radians to degrees
    // Which quadrant?
    if angle <> 1000                          // If NOT in deadband
      if angle >= 45 and <= 135 then UP
      if angle < 45 and angle > -45 then RIGHT
      if angle <= -45 and angle >= -135 then DOWN
      if angle < -135 and angle >= -180 OR <= 180 and angle > 135 then LEFT 
  */
  // Determine if joystick is centered...
  if ((mapJoyX <= xCenter + xDeadband && mapJoyX >= xCenter - xDeadband) && (mapJoyY <= yCenter + yDeadband && mapJoyY >= yCenter - yDeadband))  {
    angle = 1000.0;
    if (lastKeyPressed != dPadNone)  {
      KEY_RELEASE(lastKeyPressed);
      DEBUG_PRINT("Center: release ");
      DEBUG_PRINTLN(lastKeyPressed);
      lastKeyPressed = dPadNone;
    }
  }
  else  { // Else determine its angle
    angle = atan2(mapJoyY - yCenter , mapJoyX - xCenter) * 57.2957795;
  }
  //DEBUG_PRINT("Angle: ");
  //DEBUG_PRINTLN(angle);
  if ((angle >= 45 && angle <= 135)) {  // UP
    if (lastKeyPressed != dPadUp)  {
      if (lastKeyPressed != dPadNone)  {
        KEY_RELEASE(lastKeyPressed);
        DEBUG_PRINT("Release key ");
        DEBUG_PRINTLN(lastKeyPressed);
      }
      lastKeyPressed = dPadUp;
      KEY_PRESS(dPadUp)
      DEBUG_PRINT("Key pressed: ");
      DEBUG_PRINTLN (dPadUp);
    }
  }
  else if (angle < 45 && angle > -45) {  // RIGHT
    if (lastKeyPressed != dPadRt)  {
      if (lastKeyPressed != dPadNone)  {
        KEY_RELEASE(lastKeyPressed);
        DEBUG_PRINT("Release key ");
        DEBUG_PRINTLN(lastKeyPressed);
      }
      lastKeyPressed = dPadRt;
      KEY_PRESS(dPadRt)
      DEBUG_PRINT("Key pressed: ");
      DEBUG_PRINTLN (dPadRt);
    }
  }
  else if (angle <= -45 && angle >= -135) {  // DOWN
    if (lastKeyPressed != dPadDn)  {
      if (lastKeyPressed != dPadNone)  {
        KEY_RELEASE(lastKeyPressed);
        DEBUG_PRINT("Release key ");
        DEBUG_PRINTLN(lastKeyPressed);
      }
      lastKeyPressed = dPadDn;
      KEY_PRESS(dPadDn)
      DEBUG_PRINT("Key pressed: ");
      DEBUG_PRINTLN (dPadDn);
    }
  }
  else if ((angle < -135 && angle >= -180) || (angle <= 180 && angle > 135))  { // LEFT
    if (lastKeyPressed != dPadLt)  {
      if (lastKeyPressed != dPadNone)  {
        KEY_RELEASE(lastKeyPressed);
        DEBUG_PRINT("Release key ");
        DEBUG_PRINTLN(lastKeyPressed);
      }
      lastKeyPressed = dPadLt;
      KEY_PRESS(dPadLt)
      DEBUG_PRINT("Key pressed: ");
      DEBUG_PRINTLN (dPadLt);
    }
  }
}

void readKeys() {
  for (int i = 0; i <= 12; i++)  {
    if (digitalRead(dSwitch[i].pin) == LOW) {
      if (dSwitch[i].wasPressed == false)  {
        KEY_PRESS(dSwitch[i].cmd)
        DEBUG_PRINT("Key press:\t");
        DEBUG_PRINT(dSwitch[i].pin);
        DEBUG_PRINT('\t');
        DEBUG_PRINTLN (dSwitch[i].cmd);
        dSwitch[i].wasPressed = true;
      }
    }
    else  {
      if (dSwitch[i].wasPressed == true)  {
        KEY_RELEASE(dSwitch[i].cmd);
        DEBUG_PRINT("Key released:\t");
        DEBUG_PRINT(dSwitch[i].pin);
        DEBUG_PRINT('\t');
        DEBUG_PRINTLN (dSwitch[i].cmd);
        dSwitch[i].wasPressed = false;
      }
    }
  }
}
