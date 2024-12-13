#include <Servo.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define gateTrigPin 2
#define gateEchoPin 3
#define spot1TrigPin 11
#define spot1EchoPin 12
#define spot2TrigPin 4   
#define spot2EchoPin 5   

#define servoPin 9

#define freeSpotThreshold 20

Servo gateServo;

hd44780_I2Cexp lcd;

unsigned long previousMillis = 0;
const long interval = 2000; 
bool showSpot1Status = true; 

void setup() {
  Serial.begin(9600); 

  pinMode(gateTrigPin, OUTPUT);
  pinMode(gateEchoPin, INPUT);
  pinMode(spot1TrigPin, OUTPUT);
  pinMode(spot1EchoPin, INPUT);
  pinMode(spot2TrigPin, OUTPUT);  
  pinMode(spot2EchoPin, INPUT);   

  gateServo.attach(servoPin);
  gateServo.write(0);  

  lcd.begin(16, 2);    
  lcd.print("Smart Parking");
  delay(2000);         
  lcd.clear();         
}

void loop() {
  int spot1Status = getParkingSpotStatus(spot1TrigPin, spot1EchoPin);
  int spot2Status = getParkingSpotStatus(spot2TrigPin, spot2EchoPin);
  int gateStatus = getEntranceStatus();

  Serial.print("Spot 1 Status: ");
  Serial.print(spot1Status);
  Serial.print(", Spot 2 Status: ");
  Serial.print(spot2Status);
  Serial.print(", Gate Status: ");
  Serial.println(gateStatus);

  lcd.setCursor(0, 0);
  if (gateStatus == 1) {
    lcd.print("Gate: Opening   ");
    gateServo.write(90);  
    delay(3000);          
  } else {
    lcd.print("Gate: Closed    ");
    gateServo.write(0);   
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    showSpot1Status = !showSpot1Status; 
  }

  lcd.setCursor(0, 1);
  if (showSpot1Status) {
    if (spot1Status == 1) {
      lcd.print("Spot 1: Occupied");
    } else {
      lcd.print("Spot 1: Free     ");
    }
  } else {
    if (spot2Status == 1) {
      lcd.print("Spot 2: Occupied");
    } else {
      lcd.print("Spot 2: Free     ");
    }
  }

}

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  

  long duration = pulseIn(echoPin, HIGH); 
  int distance = duration * 0.034 / 2;    
  return distance;
}

int getParkingSpotStatus(int trigPin, int echoPin) {
  int distance = getDistance(trigPin, echoPin);
  return (distance < freeSpotThreshold) ? 1 : 0;
}

int getEntranceStatus() {
  int distance = getDistance(gateTrigPin, gateEchoPin);
  return (distance < 10) ? 1 : 0;
}
