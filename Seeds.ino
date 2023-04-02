#include <Servo.h>

struct FarmType{
  float pay;
  int color[3];
};

struct Farm{
    int farmIndex;
    int pins[2];
};

int buttonPins[] = {2,3,4,5};
bool isPinHeld[] = {false, false, false, false};

const FarmType empty = {0.0, {0, 0, 0}};
const FarmType carrot = {0.5, {255, 0, 0}};
const FarmType potato = {3.5, {255, 255, 0}};
const FarmType test1 = {0.75, {0, 255, 0}};
const FarmType farmTypes[] = {empty, carrot, potato, test1};

Farm farms[] = {{0, {8,9}},{0, {10, 11}}, {0, {12, 13}}};

Servo myservo;
Servo heartservo;

const int heartbeatPin = 6;
const int servoPin = 7;
int beatScalar = 8;

float moneyCounter = 0; 

int oldTime = 0;
int currentTime = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  heartservo.attach(heartbeatPin); 
  myservo.attach(servoPin); 

  pinMode(13,   OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11,  OUTPUT);
  pinMode(10,  OUTPUT);
  pinMode(9,  OUTPUT);
  pinMode(8,  OUTPUT);
}

void loop() {
  for(int i=0; i<4; i++){
    if (digitalRead(buttonPins[i]) == HIGH) {
      if(!isPinHeld[i]){
        Serial.print("Pin is high");
        Serial.println(i);
        if(buttonPins[i] == 5){
          spendMoney();          
        }
        else{        
          updateFarm(i);
        }
        isPinHeld[i] = true;
      }
    }
    else{
      isPinHeld[i] = false;
    }
    //testServo();
    updateLights();
  }

  updateMoney();
}

void spendMoney(){
  moneyCounter -= 45;
  moneyCounter = max(moneyCounter, 0); 

  updateServo(); 
}

void updateFarm(int farmToChange){
  Serial.println("Updating farm");
  farms[farmToChange].farmIndex += 1;
  farms[farmToChange].farmIndex = farms[farmToChange].farmIndex % 4;

}

void updateLights(){
  for(int i=0; i<4; i++){
      Farm curr = farms[i];

      int currentGreen = farmTypes[curr.farmIndex].color[0];
      int currentRed = farmTypes[curr.farmIndex].color[1];

      analogWrite(farms[i].pins[0], currentGreen);
      analogWrite(farms[i].pins[1], currentRed);
  }
}

void updateMoney(){
  currentTime += millis() - oldTime;
  oldTime = millis();

  if(currentTime < 1000){
    return;
  }

  currentTime = 0;

  float addCounter = 0;
  for(int i=0; i<3; i++){
    addCounter += farmTypes[farms[i].farmIndex].pay;
    Serial.print("Farm: ");
    Serial.print(farms[i].farmIndex);
    Serial.print(" ");
  }
  moneyCounter += addCounter;
  moneyCounter = min(moneyCounter, 180);

  Serial.print("Add Counter : ");
  Serial.print(addCounter);

  Serial.print(" Money: ");
  Serial.println(moneyCounter);

  heartBeat(addCounter);
  updateServo();
}

void updateServo(){
  Serial.println("Update servo");
  myservo.write((int)moneyCounter);
  //delay(10);

}

void heartBeat(int weight){
  Serial.println("Update heartbeat");

  heartservo.write(90 + (weight * beatScalar));
  beatScalar *= -1;
  
  //delay(10);
}