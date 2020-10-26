int STATE_UNKNOWN = -1;
int STATE_ON      = 0;
int STATE_OFF     = 1;
int STATE_STANDBY = 2;

int pwr_pin   = 0;
int red_pin   = 1;
int grn_pin   = 2;
int vol_u_pin = 3;
int vol_d_pin = 4;
int auto_pin  = 5;
int menu_pin  = 6;
int exit_pin  = 7;
int k6_pin    = 8;

int pir_pin = 9;

int out_grn_pin = 10;
int out_red_pin = 11;

int red_led = -1;
int grn_led = -1;

int prev_pir_triggered = 0;
int prev_display_state = STATE_UNKNOWN;

String inputString = "";
boolean stringComplete = false;

bool isOn(){
  return (red_led || grn_led);
}

void on() {
  if (!isOn()) {
    pressButton(pwr_pin);
  
    delay(500);
  
    for (int t = 0; t < 60; t++) {
       pressButton(auto_pin, 42);
       delay(50);
    }
  }
}

void off() {
  if (isOn()) {
    pressButton(pwr_pin);
  }
}

void toggle() {
  if (isOn()) {
    off();
  } else {
    on();
  }
}

void pressButton(int pin, int delayTime) {
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);  // Pull the signal low to activate the button
  delay(delayTime);  // Wait
  pinMode(pin, INPUT);  // Release the button.
}

void pressButton(int pin) {
  pressButton(pin, 100);
}


void setup() {
  // reserve 50 bytes for the inputString:
  inputString.reserve(50);

  Serial.begin(9600);
  
  pinMode(red_pin, INPUT);
  pinMode(grn_pin, INPUT);

  pinMode(pir_pin, INPUT);
  
  pinMode(out_red_pin, OUTPUT);
  pinMode(out_grn_pin, OUTPUT);
}

void loop() {
  // Check if PIR has detected movement
  int pir_triggered = digitalRead(pir_pin);
  
  if (pir_triggered != prev_pir_triggered) {
    if (pir_triggered) {
      Serial.println("PIR ON");
      on();
    } else {
      Serial.println("PIR OFF");
    }
    prev_pir_triggered = pir_triggered;
  }
  
  // Check if display change changed
  red_led = digitalRead(red_pin);
  grn_led = digitalRead(grn_pin);
  
  int display_state = STATE_OFF;
  if (grn_led) {
    display_state = STATE_ON;
  } else if (red_led) {
    display_state = STATE_STANDBY;
  }
  
  if (display_state != prev_display_state) {
    if (display_state == STATE_ON) {
      Serial.println("DISP ON");
      analogWrite(out_red_pin, 0);
      analogWrite(out_grn_pin, 255);
    } else if (display_state == STATE_OFF) {
      Serial.println("DISP OFF");
      analogWrite(out_red_pin, 255);
      analogWrite(out_grn_pin, 0);
    } else if (display_state == STATE_STANDBY) {
      Serial.println("DISP STANDBY");
      analogWrite(out_red_pin, 255);
      analogWrite(out_grn_pin, 255);
    }
    prev_display_state = display_state;
  }
  
  // Check if serial data received
  if (stringComplete) {
    Serial.println("ACK " + inputString);
    if (inputString == "PWR") {
      pressButton(pwr_pin);
    } else if (inputString == "VOL+") {
      pressButton(vol_u_pin);
    } else if (inputString == "VOL-") {
      pressButton(vol_d_pin);
    } else if (inputString == "AUTO") {
      pressButton(auto_pin);
    } else if (inputString == "MENU") {
      pressButton(menu_pin);
    } else if (inputString == "EXIT") {
      pressButton(exit_pin);
    } else if (inputString == "K6") {
      pressButton(k6_pin);
    } else if (inputString == "ON") {
      on();
    } else if (inputString == "OFF") {
      off();
    } else if (inputString == "TOGGLE") {
      toggle();
    }
    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  delay(10);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      inputString.trim();
      stringComplete = true;
    }
  }
}

