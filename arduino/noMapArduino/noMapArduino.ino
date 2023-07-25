#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int32.h>

//Ros Node Handle
ros::NodeHandle  nh;

//PWM Output Pin
const int forwardPin = 4;
const int reversePin = 5;
const int leftPin = 6;
const int rightPin = 7;

//Emergency Button Pin and Value
const int forceStopPin = 2;
const int removeStopPin = 3;
volatile bool forcedStop = false;

//Speed and pwm limit
float maxVel = 1;
int minVal = 200;

//Variables for storing linear and angular speed as pwm values
int pwm_linear = minVal;
int pwm_angular = minVal;

//Variables for storing forward/backward(x) and left/right(y) voltage
int xVoltage = minVal;
int yVoltage = minVal;

/*
std_msgs::String interr;
char s1[10]="interrupt";
char s2[3]="go";
*/

//Boolean values to decide the 4-way direction
bool forward;
bool left;

//Check if the Push Button Switch is pressed
void checkForceStop(){
  forcedStop = true;
}
void removeForceStop(){
  forcedStop = false;
}

void callback_linear(const std_msgs::Int32& lin_msg){
  pwm_linear = lin_msg.data;

  //Check the direction
  if(pwm_linear >= 0)
    forward = true;
  else
    forward = false;
  
  xVoltage = abs(pwm_linear);
}
void callback_angular(const std_msgs::Int32& ang_msg){
  pwm_angular = ang_msg.data;

  //Check the direction
  if(pwm_angular >= 0)
    left = true;
  else
    left = false;

  yVoltage = abs(pwm_angular);
}

//Ros Subscriber
ros::Subscriber<std_msgs::Int32> sub_l("arduino_linear", callback_linear);
ros::Subscriber<std_msgs::Int32> sub_a("arduino_angular", callback_angular);

void setup(){
  //Setup of the Pins
  pinMode(forwardPin,OUTPUT);
  pinMode(reversePin,OUTPUT);
  pinMode(leftPin,OUTPUT);
  pinMode(rightPin,OUTPUT);

  pinMode(forceStopPin, INPUT_PULLUP);
  pinMode(removeStopPin, INPUT_PULLUP);
  
  forcedStop = true; // Initial state it is needed to release the chair
  
  attachInterrupt(digitalPinToInterrupt(forceStopPin), checkForceStop, FALLING);
  attachInterrupt(digitalPinToInterrupt(removeStopPin), removeForceStop, FALLING);
  
  //Setting default values
  forward = true;
  left = true;

  xVoltage = minVal;
  yVoltage = minVal;

  // Start the communication between ROS and Arduino
  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.subscribe(sub_l);
  nh.subscribe(sub_a);

  // Initialize the pwm pins
  analogWrite(forwardPin, 255);
  analogWrite(leftPin, 255);
  analogWrite(reversePin, 255);
  analogWrite(rightPin, 255);
}

void loop(){
  //check if the variable for the emergency stop is enabled
  if(forcedStop){
    analogWrite(forwardPin, minVal);
    analogWrite(leftPin, minVal);
    analogWrite(reversePin, minVal);
    analogWrite(rightPin, minVal);
  }
  else{
    //Linear Velocity
    
    if(forward){
      //set backward velocity to 0
      analogWrite(reversePin, minVal);
      //set forward velocity to the mapped value
      analogWrite(forwardPin, xVoltage);
    }
    else {
      //set forward velocity to 0
      analogWrite(forwardPin, minVal);
      //set backward velocity to the mapped value
      analogWrite(reversePin, xVoltage);                      
    }
    
    if(left){
      //set right velocity to 0
      analogWrite(rightPin, minVal);
      //set left velocity to the mapped value
      analogWrite(leftPin, yVoltage);                      
    }
    else{
      //set left velocity to 0
      analogWrite(leftPin, minVal);
      //set backward velocity to the mapped value
      analogWrite(rightPin, yVoltage);
    }
    
  }
  //keep the set voltages for 10 milliseconds
  delay(10); 

  // Keep the communication between ROS and Arduino
  nh.spinOnce();
  nh.spinOnce();
  
}
