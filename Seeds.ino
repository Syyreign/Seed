#include <Servo.h>

// A struct that contains data on the different farm types
struct FarmType{
  float pay;
  int color[3];
};

// A struct for each of the farms, that includes an index to a specific farm type
struct Farm{
    int farmIndex;
    int pins[2];
};

int buttonPins[] = {2,3,4,5};
bool isPinHeld[] = {false, false, false, false};

// The different farm types
const FarmType empty = {0.0, {0, 0, 0}};
const FarmType carrot = {0.5, {255, 0, 0}};
const FarmType potato = {3.5, {255, 255, 0}};
const FarmType test1 = {0.75, {0, 255, 0}};
const FarmType farmTypes[] = {empty, carrot, potato, test1};

Farm farms[] = {{0, {8,9}},{0, {10, 11}}, {0, {12, 13}}};

Servo myservo;
Servo heartservo;

// Controls the heartbeat servo
const int heartbeatPin = 6;
//Controls the dial servo
const int servoPin = 7;
//controls the intensity of the heartbeat servo
int beatScalar = 8;

//The current accumulated currency
float moneyCounter = 0; 

int oldTime = 0;
int currentTime = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  // Attach the servo pins
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

    //Checks if each button has been pressed
    if (digitalRead(buttonPins[i]) == HIGH) {

      // only trigger for the first loop if the button is being held down. Stops farms from cycling on each loop update.
      if(!isPinHeld[i]){
        Serial.print("Pin is high");
        Serial.println(i);
        if(buttonPins[i] == 5){
          spendMoney();          
        }
        else{        
          updateFarm(i);
        }

        // If the button is being held down then update this and prevent it from being triggered again
        isPinHeld[i] = true;
      }
    }

    // If no button is being held down
    else{
      isPinHeld[i] = false;
    }
    // Update the lights based on the farms current farmType color
    updateLights();
  }

  // Update the money being accumulated from the farmTypes
  updateMoney();
}

// If the spend button is pressed remove 45 from moneyCounter
void spendMoney(){
  moneyCounter -= 45;
  moneyCounter = max(moneyCounter, 0); 

  updateServo(); 
}

// Add 1 to the farm index to change which farm is being used
void updateFarm(int farmToChange){
  Serial.println("Updating farm");
  farms[farmToChange].farmIndex += 1;
  farms[farmToChange].farmIndex = farms[farmToChange].farmIndex % 4;

}

// Update the lights based on each farms currently selected farmType
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
  // Sets the time since update money was last fully run
  currentTime += millis() - oldTime;
  oldTime = millis();

  // Update the currency every 1000ms otherwise return
  if(currentTime < 1000){
    return;
  }

  currentTime = 0;

  // Used to increase the weight of the heartbeat servo
  float addCounter = 0;
  for(int i=0; i<3; i++){
    addCounter += farmTypes[farms[i].farmIndex].pay;
    Serial.print("Farm: ");
    Serial.print(farms[i].farmIndex);
    Serial.print(" ");
  }

  // Update the moneyCounter with the new currency
  moneyCounter += addCounter;
  moneyCounter = min(moneyCounter, 180);

  Serial.print("Add Counter : ");
  Serial.print(addCounter);

  Serial.print(" Money: ");
  Serial.println(moneyCounter);

  // Update both the heartbeat and the dial servo
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

  // Swing the heartbeat servo stronger based on the weight
  heartservo.write(90 + (weight * beatScalar));
  beatScalar *= -1;
  
  //delay(10);
}