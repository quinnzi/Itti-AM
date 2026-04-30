int sensor_pin = 0;
volatile int time = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(sensor_pin, INPUT);
  arm_degress = 135;//initialize the angle value of arm servo 2
  myservo2.attach(10);//arm Servo 2 is connected to D10
  time = 0
}
 
void loop()
{

  while (time == 0){
  myservo2.write(arm_degress);
  time++;
  }

   myservo2.write(0);
}

void Move(int dir, int speed){
  if(dir == 1){
  digitalWrite(2,HIGH); //Move forward
  analogWrite(5,speed); 
  digitalWrite(4,LOW);
  analogWrite(6,speed);
  }
  
  if(dir == -1){// Move backward
  digitalWrite(2,LOW);
  analogWrite(5,speed);
  digitalWrite(4,HIGH);
  analogWrite(6,speed);
  }

  else {Serial.println(
    'Error. Direction not specified.';
    )
  }
}

void Rotate_Left(int speed) //Define the left rotation function of the input speed
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
  digitalWrite(4,HIGH);
  analogWrite(6,0);
}

void Rotate_Right(int speed){
  digitalWrite(2,HIGH);
  analogWrite(5,speed);
  digitalWrite(4,HIGH);
  analogWrite(6,speed);
}