// include the library code:
#include <LiquidCrystal.h>
#include <TimerOne.h>

// Lidar stuff
#include <LIDARLite.h>
LIDARLite myLidarLite;
int dist_r(void);

// compass library
#include <Wire.h>
#define addr 0x60 // I2C Address

#define mFwd 1
#define mBwd 0
#define mPinL 7
#define mPinR 8
#define mPwmL 9
#define mPwmR 10

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;

// the pins for switching on and off via joystick button; pins for pwm counting
const int buttonPin = 19, ENCBleft = 2, ENCBright = 3;

// variables for pwm counting
volatile int iL = 0, iR = 0, frequencyHzL, frequencyHzR;

// joystick button (set to be off initially)
boolean joyStickMode = false;

// for compass
byte raw = 0;
String B;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    VOID SETUP
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  // Initialize LCD set up the LCD's number of columns and rows:
  lcd.begin(20, 4);

  // Initialize compass
  Wire.begin();
  // Initialize serial
  Serial.begin(115200);

  myLidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz
  myLidarLite.configure(1); // Change this number to try out alternate configurations

  // for switching on and off via joystick button
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), joyStick, FALLING);                                           // Button uncomment to effect

  // for counting pwm (therefore gaining distance)
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCBleft), countENCBL, FALLING);                                          // Wheel distance uncomment to effect
  attachInterrupt(digitalPinToInterrupt(ENCBright), countENCBR, FALLING);                                          // Wheel distance uncomment to effect

  // input via joystick
  pinMode(A7, INPUT);
  pinMode(A6, INPUT);

}




// ===============================  Global variables for moving
int lidTest;
int mSpeed;
int motorRight = 0, motorLeft = 0;
byte targetPos;
boolean turnR = false;
volatile int currentDist;
int targetDist;
int moveDist;
boolean forward = true;
boolean forceStop;
String mode = "";
int resLoop = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    VOID LOOP START
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  // initializing variables
  mSpeed = 255;
  iL = 0;
  currentDist = dist_ave(50);
  forceStop = false;

  // Check position
  carPos();
  targetPos = raw;

  if (Serial.available() > 0)
  {
    String message = Serial.readStringUntil('\n');
    if (message.indexOf(':') != -1 && message.indexOf('{') == -1)
    {
      int pos_turn = message.indexOf("Turn");
      int pos_dire = message.indexOf("Dire");
      int pos_move = message.indexOf("Move");
      int pos_dist = message.indexOf("Dist");
      int pos_JSM = message.indexOf("JSM");
      int pos_s = message.indexOf(":");


      //==========================================================================================================¤ TURNING WITH COMPASS

      //============================================= Turn by adding to raw
      if (pos_turn > -1)
      {
        Serial.println("Command = Turn ");
        if (pos_s > -1) {
          Serial.print("State=");
          Serial.println("Turning");
          int stat = message.substring(pos_s + 1).toInt();
          targetPos = stat > 0 ? raw + toByte(stat) : raw - toByte(abs(stat));
          turnR = stat == 180 ? true : false;
          Serial.print("Dire=");
          Serial.println(toDegrees(targetPos));
          turn();
        }
      }
      //============================================= Turn to exact heading position
      else if (pos_dire > -1)
      {
        Serial.println("Command = Dire ");
        if (pos_s > -1) {
          Serial.print("State=");
          Serial.println("Turning");
          int stat = message.substring(pos_s + 1).toInt();
          targetPos = toByte(stat);
          Serial.print("Dire=");
          Serial.println(toDegrees(targetPos));
          turn();
        }
      }

      //==========================================================================================================¤ MOVING WITH LIDAR

      //============================================= Move forward or backward
      else if (pos_move > -1)
      {
        Serial.println("Command = Move ");
        if (pos_s > -1) {
          Serial.print("State=");
          Serial.println("Moving linear");
          int stat = message.substring(pos_s + 1).toInt();
          forward = stat > 0 ? true : false;
          moveDist = 180 / 13.7 * abs(stat);
          Serial.print("Dist=");
          Serial.println(stat);
          movePWM();
        }
      }
      //============================================= Move to exact obsticle distance
      else if (pos_dist > -1)
      {
        Serial.println("Command = Dist ");
        if (pos_s > -1) {
          Serial.print("State=");
          Serial.println("Moving linear");
          int stat = message.substring(pos_s + 1).toInt();
          targetDist = stat;
          Serial.print("Dist=");
          Serial.println(targetDist);
          moveLid();
        }
      }
      //==========================================================================================================¤ CONTROL MODE
      else if (pos_JSM > -1)
      {
        Serial.println("Command = Joystick MODE ");
        joyStickMode = true;
      }
      Serial.print("State=");
      Serial.println("Done!");
    }
  }

  while (joyStickMode) {
    Serial.print("State=");
    Serial.println("JoyStick Mode!");
    if (Serial.available() > 0 && Serial.readStringUntil('\n').indexOf(":") != -1)
    {
      Serial.println("Command = STOP");
      joyStickMode = false;
    }
    JScontrol();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("JoyStick Mode!");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current pos: ");
  lcd.print(toDegrees(raw));
  lcd.print(char(223));
  lcd.setCursor(0, 2);
  lcd.print("Current dist: ");
  lcd.print(currentDist);

  // Send response mqtt response

  if (resLoop == 0) {
    Serial.print("Com=");
    Serial.println(toDegrees(raw));
    resLoop++;
  }
  else if (resLoop == 1) {
    Serial.print("Lid=");
    Serial.println(currentDist);
    resLoop++;
  }
  else {
    Serial.print("State=");
    Serial.println("Ready!");
    resLoop = 0;
  }
}
//============================================ VOID LOOP  END !!!!!!!!!!!!!!!!!!





///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    MOVING WITH JOYSTICK
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void JScontrol()
{
  int sensorValueX = analogRead(A7);
  int sensorValueY = analogRead(A6);
  int dirBwd = map(sensorValueY, 1023, 550, 255, 0);
  int dirFwd = map(sensorValueY, 0, 470, 255, 0);
  int dirRight = map(sensorValueX, 1023, 550, 255, 0);
  int dirLeft = map(sensorValueX, 0, 470, 255, 0);
  // Y-Axis value
  if (sensorValueY <= 470)
  {
    digitalWrite(mPinR, mFwd);
    digitalWrite(mPinL, mFwd);
    motorLeft = dirFwd;
    motorRight = dirFwd;
  }
  else if (sensorValueY >= 550)
  {
    digitalWrite(mPinR, mBwd);
    digitalWrite(mPinL, mBwd);
    motorLeft = dirBwd;
    motorRight = dirBwd;
  }
  else
  {
    motorLeft = 0;
    motorRight = 0;
  }

  // X-Axis value
  if (sensorValueX >= 550)
  {
    if (sensorValueY >= 550 || sensorValueY <= 470)
    {
      motorLeft = (motorLeft >= dirRight) ? motorLeft : dirRight;
      motorRight = motorLeft - dirRight / 2 - (motorLeft - motorRight) / 2;
    }
    else
    {
      digitalWrite(mPinR, mBwd);
      digitalWrite(mPinL, mFwd);
      motorLeft = dirRight;
      motorRight = dirRight;
    }
  }
  else if (sensorValueX <= 470)
  {
    if (sensorValueY >= 550 || sensorValueY <= 470)
    {
      motorRight = (motorRight >= dirLeft) ? motorRight : dirLeft;
      motorLeft = motorRight - dirLeft / 2 - (motorRight - motorLeft) / 2;
    }
    else
    {
      digitalWrite(mPinR, mFwd);
      digitalWrite(mPinL, mBwd);
      motorLeft = dirLeft;
      motorRight = dirLeft;
    }
  }
  analogWrite(mPwmL, motorLeft);
  analogWrite(mPwmR, motorRight);
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    MOVING USING PWM
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void movePWM() {
  while (moveDist > iL)
  {
    // Emergency stop
    if (Serial.available() > 0 && Serial.readStringUntil('\n').indexOf("Move") != -1)
    {
      Serial.println("Command = STOP");
      forceStop = true;
    }
    if (forceStop) break;
    if (joyStickMode) break;

    // =======================¤  Turn to closest path to target position and auto change speed
    if (forward) {
      motorRight = mFwd;
      motorLeft = mFwd;
    } else {
      motorRight = mBwd;
      motorLeft = mBwd;
    }

    // =============================================¤        control speed
    mSpeed = 255 - 1.6 * (iL - (moveDist - 132));
    mSpeed = mSpeed > 255 ? 255 : mSpeed;

    digitalWrite(mPinL, motorLeft);
    digitalWrite(mPinR, motorRight);
    analogWrite(mPwmL, mSpeed);
    analogWrite(mPwmR, mSpeed);

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("iL/moveDist=");
    lcd.print(iL);
    lcd.print("/");
    lcd.print(moveDist);

    Serial.print("Lid=");
    Serial.println(currentDist);
  }
  analogWrite(mPwmL, 0);
  analogWrite(mPwmR, 0);
  delay(500);
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    MOVING USING LIDAR
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void moveLid() {
  for (int i = 0; i < 5; i++)
  {
    currentDist = dist_ave(50);
    carPos();
    turn();

    while (targetDist != currentDist)
    {
      // Emergency stop
      if (Serial.available() > 0 && Serial.readStringUntil('\n').indexOf("Dist") != -1)
      {
        Serial.println("Command = STOP");
        forceStop = true;
      }
      if (forceStop) break;
      if (joyStickMode) break;

      // =======================¤  Turn to closest path to target position and auto change speed
      if (targetDist < currentDist) {
        motorRight = mFwd;
        motorLeft = mFwd;
      } else {
        motorRight = mBwd;
        motorLeft = mBwd;
      }

      // =============================================¤        control speed
      mSpeed = 255 - 21 * (10 - abs(currentDist - targetDist));
      mSpeed = mSpeed > 255 ? 255 : mSpeed;

      digitalWrite(mPinL, motorLeft);
      digitalWrite(mPinR, motorRight);
      analogWrite(mPwmL, mSpeed);
      analogWrite(mPwmR, mSpeed);

      currentDist = dist_ave(50);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Current pos: ");
      lcd.print(toDegrees(raw));
      lcd.print(char(223));
      lcd.setCursor(0, 2);
      lcd.print("Current dist: ");
      lcd.print(currentDist);
      lcd.print(" cm");
      lcd.setCursor(0, 3);
      lcd.print("Target dist: ");
      lcd.print(targetDist);
      lcd.print(" cm");

      Serial.print("Lid=");
      Serial.println(currentDist);
    }
    analogWrite(mPwmL, 0);
    analogWrite(mPwmR, 0);
  }
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    LIDAR DISTANCE CALCULATION
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
int dist_ave(int n_meas)
{
  if (n_meas <= 0) return -1;
  else
  {
    static unsigned int n = 0;
    long sum = 0;

    sum = myLidarLite.distance(false); // First measurement always with calibration
    n = 1;

    for (int i = 1; i < n_meas; i++)
    {
      if (n >= 100) // Make a calibrated measurement after avery 100 measurements
      {
        sum += myLidarLite.distance(false);
        n = 1;
      }
      else
      {
        sum += myLidarLite.distance();
        n++;
      }
    }
    sum = sum / n_meas;
    //Serial.println(sum);
    return (int) (sum - 15);
  }
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    TURNING USING COMPASS
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void turn() {
  for (int i = 0; i < 5; i++) {
    carPos();
    while (targetPos !=  raw)
    {
      // Emergency stop
      if (Serial.available() > 0 && Serial.readStringUntil('\n').indexOf("r") != -1)
      {
        Serial.println("Command = STOP");
        forceStop = true;
      }
      if (forceStop) break;
      if (joyStickMode) break;

      int cDist = 0;
      int ccDist = 0;

      // ==============¤   Check Position
      if (targetPos > raw) {
        cDist = targetPos - raw;
        ccDist = 256 - (targetPos - raw);
      } else {
        cDist = 256 - (raw - targetPos);
        ccDist = raw - targetPos;
      }

      // =======================¤  Turn to closest path to target position and auto change speed
      if (cDist < ccDist || turnR) {
        motorRight = mBwd;
        motorLeft = mFwd;
        mSpeed = 255 - 1.65 * (128 - cDist);
      } else {
        motorRight = mFwd;
        motorLeft = mBwd;
        mSpeed = 255 - 1.65 * (128 - ccDist);
      }

      // =============================================¤        control speed
      mSpeed = mSpeed > 255 ? 255 : mSpeed;

      digitalWrite(mPinL, motorLeft);
      digitalWrite(mPinR, motorRight);
      analogWrite(mPwmL, mSpeed);
      analogWrite(mPwmR, mSpeed);

      carPos();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Current pos: ");
      lcd.print(toDegrees(raw));
      lcd.print(char(223));
      lcd.setCursor(0, 1);
      lcd.print("Target pos: ");
      lcd.print(toDegrees(targetPos));
      lcd.print(char(223));
      Serial.print("Com=");
      Serial.println(toDegrees(raw));
    }
    analogWrite(mPwmL, 0);
    analogWrite(mPwmR, 0);
  }
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    CAR POSITION FUNCTIONS
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void carPos()
{
  Wire.beginTransmission(addr); // start talking
  Wire.write(0x01);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, 0x01, true);
  if (Wire.available() >= 0x01)
  {
    raw = byte(Wire.read());
  }
}

// Convert compass value to 360 degrees
int toDegrees(byte val) {
  return 360.0 / 256 * val;
}

// Convert from 360 degrees to byte
byte toByte(int val) {
  return 256.0 / 360 * (val + 1);
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    JOYSTICK ON/OFF FUNCTION
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void joyStick()
{
  joyStickMode = !joyStickMode;
  lcd.clear();
  lcd.print("Pressed");
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    PWM COUNTING FUNCTION, 13.8 PULSES PER CM
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void countENCBL()
{
  iL++;
}
void countENCBR()
{
  iR++;
}
