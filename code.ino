#include <Servo.h>
#include <PinChangeInterrupt.h>
#include <Wire.h>

int sensor_pin = 14;
int pump_pin = 16;

int trig_pin = 13;
int leftU_pin = 17;
int rightU_pin = 12;

Servo myservo2;
Servo myservo1;

volatile int time = 0;
volatile long encoderPos = 0;
volatile long encoderPos2 = 0;
unsigned long moveStartTime;



long getEncoderPos() {
    noInterrupts();      
    long pos = encoderPos; 
    interrupts();      
    return pos;
}
long getEncoderPos2() {
    noInterrupts();          
    long pos = encoderPos2;  
    interrupts();         
    return pos;
}

void setup()
{
  
  Serial.begin(9600);
  
  pinMode(pump_pin, OUTPUT);  // watering stuff
  digitalWrite(pump_pin, HIGH);
  delay(2000);
  pinMode(sensor_pin, INPUT);
  
  pinMode(2, OUTPUT); // motor pins
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);

  pinMode(leftU_pin, INPUT); // ultrasonic sensor
  pinMode(rightU_pin, INPUT);
  pinMode(trig_pin, OUTPUT);

  pinMode(8, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(8), readEncoder, RISING);
  pinMode(7, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(7), readEncoder2, RISING);

  myservo2.attach(10);
  myservo1.attach(9);
  myservo2.write(25);
  myservo1.write(90);
  moveStartTime = millis();
}


String buffer = "";
String unbuffered = "";

int plantNum;
int rowNum;
float distance;

int rowsDone = 0;
int plantsDone = 0;
int totalDone = 0;

String number;
int Cindex;
int readings[50];
 
  long pos1 = getEncoderPos();
  long pos2 = getEncoderPos2();
  float pulsesG = (pos2 + pos1)/ 2.0;
  int gonD = round(1.02101761 * pulsesG);
  int totalPlants = rowNum * plantNum;
void loop()
{
while (Serial.available()) {
  Serial.println("helo");
  char c = Serial.read();
  if (c == '\n' || c == '\r' ){
    continue;
  }

if (c == ';') {
      Cindex = buffer.indexOf(":");
      unbuffered = buffer.substring(0, Cindex);
      Serial.println(buffer);
      setValue(buffer);
      buffer = "";
    } 
    else {
      buffer += c;
    }
  }
  if(rowNum && distance && plantNum && rowsDone < rowNum){

  if (rowsDone <= rowNum){
   pos1 = getEncoderPos();
   pos2 = getEncoderPos2();
   pulsesG = (pos2 + pos1)/ 2.0;
   gonD = round(1.02101761 * pulsesG);
   totalPlants = rowNum * plantNum;


    Serial.println(gonD);
    if(plantsDone < plantNum){
      if (gonD < distance){
        Move(1, 100);
      }
      else{
      Stop();
      waterMovement();

    }
    }
  
    else{
      rowsDone++;
      if (rowsDone == rowNum){
        Stop();
        Serial.println("Donezies!");
        for(int i = 0; i <= plantNum; i++){
           Serial.println(readings[i]);
        }
        return;
      }
      else{
        plantsDone = 0;
        Serial.print("plantss");
        Serial.println(plantNum);
        Serial.print("rowsss");
        Serial.println(rowsDone);
            Rotate_Right(100);
      delay(600);
      Move(1, 100);
      delay(3000);
      Rotate_Right(100);
      delay(600);
      noInterrupts();
    encoderPos = 0;
    encoderPos2 = 0;
    interrupts();
    gonD = 0;
      pos1 = 0;
      pos2 = 0;
      }
  }
}}
}
int ind = 0;
int tries = 0;
void waterMovement(){
    myservo1.write(45);
    delay(5000);
servNo(100);
  int sensor_data = analogRead(sensor_pin);
   Serial.print("moisture:");
  Serial.println(sensor_data);
  delay(3000);
  readings[ind] = sensor_data;
  while(sensor_data > 440){
  sensor_data = analogRead(sensor_pin); // to refresh sensor value to escape loop
  Serial.print("moisture:");
  Serial.println(sensor_data);
  digitalWrite(pump_pin,LOW);
  delay(2000);
  digitalWrite(pump_pin,HIGH);
  tries++;
  if(tries > 9){
    Serial.println("Not working");
      delay(2000); 
    myservo2.write(25); 
    myservo1.write(90);
    plantsDone++;
    totalDone = plantsDone;
    noInterrupts();
encoderPos = 0;
encoderPos2 = 0;
interrupts();
ind++;
      gonD = 0;
      pos1 = 0;
      pos2 = 0;
      return;
  }
  }
    delay(2000); 
    myservo2.write(25); 
    myservo1.write(90);
    plantsDone++;
    totalDone = plantsDone;
    noInterrupts();
encoderPos = 0;
encoderPos2 = 0;
interrupts();
      gonD = 0;
      pos1 = 0;
      pos2 = 0;
}

void Move(int dir, int speed){
  if(dir == 1){
    digitalWrite(2,HIGH); //Move forward
    analogWrite(5,speed); 
    digitalWrite(4,LOW);
    analogWrite(6,speed + 7);
  }
  
  else if(dir == -1){// Move backward
    digitalWrite(2,LOW);
    analogWrite(5,speed);
    digitalWrite(4,HIGH);
    analogWrite(6,speed + 7);
  }

  else {Serial.println(
    "Error. Direction not specified."
    );
  }
}
void clearSerialBuffer() {
while (Serial.available()) {
  Serial.read(); 
}
}
void setValue(String info)
{
  Cindex = info.indexOf(":");
  String val = info.substring(0, Cindex);
  if(val == "Plants"){
  number = info.substring(Cindex + 1);
  plantNum = number.toInt();
  }
  if(val == "Distance"){
  number = info.substring(Cindex + 1);
  distance = number.toFloat();
  }
  if(val == "Rows"){
  number = info.substring(Cindex + 1);
  rowNum = number.toInt();
}
}

void Rotate_Left(int speed)
{
  digitalWrite(2,LOW);
  analogWrite(5,speed);
  digitalWrite(4,LOW);
  analogWrite(6,speed);
}

void Stop()
{
  digitalWrite(2,LOW);
  analogWrite(5,0);
  digitalWrite(4,LOW);
  analogWrite(6,0);
}

void Rotate_Right(int speed){
  digitalWrite(2,HIGH);
  analogWrite(5,speed);
  digitalWrite(4,HIGH);
  analogWrite(6,speed);
}

void readEncoder() {
  encoderPos++;
}
void readEncoder2() {
  encoderPos2++;
}

void servNo(int end){
    int start = 25;
    for(int i = start; i <= end; i++){
    myservo2.write(i);
    delay(60);}


}



