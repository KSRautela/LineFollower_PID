#ifndef CBI
#define CBI(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef SBI
#define SBI(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//-------- Pin definitions for the TB6612FNG Motor Driver ----
#define AIN1 4
#define AIN2 3
#define BIN1 6
#define BIN2 7
#define PWMA 9
#define PWMB 10
//------------------------------------------------------------

//-------- Settings --------
bool isBlackLine = true;             // true for black line, false for white line
unsigned int lineThickness = 15;     // Line thickness in mm (10 - 35 recommended)
const unsigned int numSensors = 7;
//----------------------------

// PID control variables
int P = 0, D = 0, I = 0, previousError = 0, PIDvalue = 0;
double error = 0;
int lsp = 0, rsp = 0;
const int lfSpeed = 150;
int currentSpeed = 30;

// Sensor variables
const int sensorWeight[7] = { 4, 2, 1, 0, -1, -2, -4 };
int minValues[7] = {0}, maxValues[7] = {0}, threshold[7] = {0};
int sensorValue[7] = {0}, sensorArray[7] = {0};
int activeSensors = 0;
bool onLine = true;

// PID tuning constants
const float Kp = 0.08;
const float Kd = 0.15;
const float Ki = 0.0;

void setup() {
  // Set ADC speed for faster analog readings
  SBI(ADCSRA, ADPS2);
  CBI(ADCSRA, ADPS1);
  CBI(ADCSRA, ADPS0);

  Serial.begin(9600);

  // Motor control pins
  const uint8_t motorPins[] = { AIN1, AIN2, BIN1, BIN2, PWMA, PWMB };
  for (uint8_t i = 0; i < sizeof(motorPins) / sizeof(motorPins[0]); i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  // Push buttons and LED
  pinMode(11, INPUT_PULLUP); // Calibrate button
  pinMode(12, INPUT_PULLUP); // Run button
  pinMode(13, OUTPUT);       // LED Indicator

  // Motor driver standby pin
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); // Enable motors

  // Constrain line thickness
  lineThickness = constrain(lineThickness, 10, 35);
}

void loop() {
  // Wait for calibration button press
  while (digitalRead(11));
  delay(1000);
  calibrate();

  // Wait for run button press
  while (digitalRead(12));
  delay(1000);

  // Main loop: Follow the line
  while (true) {
    readLine();

    if (currentSpeed < lfSpeed) currentSpeed++;

    if (onLine) {
      lineFollow();
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
      recoverySpin();
    }
  }
}

void lineFollow() {
  error = 0;
  activeSensors = 0;

  for (uint8_t i = 0; i < numSensors; i++) {
    error += sensorWeight[i] * sensorArray[i] * sensorValue[i];
    activeSensors += sensorArray[i];
  }

  if (activeSensors > 0) {
    error /= activeSensors;
  }

  // PID calculations
  P = error;
  I += error;
  D = error - previousError;
  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  previousError = error;

  // Motor speed adjustment
  lsp = constrain(currentSpeed - PIDvalue, 0, 255);
  rsp = constrain(currentSpeed + PIDvalue, 0, 255);

  motor1Run(lsp);
  motor2Run(rsp);
}

void calibrate() {
  // Initialize min and max sensor values
  for (uint8_t i = 0; i < numSensors; i++) {
    int val = analogRead(i);
    minValues[i] = maxValues[i] = val;
  }

  // Rotate robot slowly to capture sensor range
  for (uint16_t j = 0; j < 10000; j++) {
    motor1Run(50);
    motor2Run(-50);

    for (uint8_t i = 0; i < numSensors; i++) {
      int val = analogRead(i);
      if (val < minValues[i]) minValues[i] = val;
      if (val > maxValues[i]) maxValues[i] = val;
    }
  }

  // Calculate threshold for each sensor
  for (uint8_t i = 0; i < numSensors; i++) {
    threshold[i] = (minValues[i] + maxValues[i]) / 2;
    Serial.print(threshold[i]);
    Serial.print(" ");
  }
  Serial.println();

  motor1Run(0);
  motor2Run(0);
}

void readLine() {
  onLine = false;

  for (uint8_t i = 0; i < numSensors; i++) {
    int val = analogRead(i);
    sensorValue[i] = isBlackLine ? map(val, minValues[i], maxValues[i], 0, 1000)
                                 : map(val, minValues[i], maxValues[i], 1000, 0);
    sensorValue[i] = constrain(sensorValue[i], 0, 1000);
    sensorArray[i] = (sensorValue[i] > 500) ? 1 : 0;
    if (sensorArray[i]) onLine = true;
  }
}

void recoverySpin() {
  if (error > 0) {
    motor1Run(-50);
    motor2Run(lfSpeed);
  } else {
    motor1Run(lfSpeed);
    motor2Run(-50);
  }
}

//-------- Motor control functions --------
void motor1Run(int motorSpeed) {
  motorSpeed = constrain(motorSpeed, -255, 255);
  setMotor(AIN1, AIN2, PWMA, motorSpeed);
}

void motor2Run(int motorSpeed) {
  motorSpeed = constrain(motorSpeed, -255, 255);
  setMotor(BIN1, BIN2, PWMB, motorSpeed);
}

void setMotor(uint8_t IN1, uint8_t IN2, uint8_t PWM, int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(PWM, speed);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(PWM, -speed);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    analogWrite(PWM, 0);
  }
}
