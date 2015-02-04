#include "moteurs.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

//Pinout des ponts en H de la carte
#define MOTOR_LEFT_DIR  7
#define MOTOR_LEFT_PWM  9
#define MOTOR_RIGHT_DIR 8
#define MOTOR_RIGHT_PWM 10

void moteurs_setup(void)
{
  pinMode(MOTOR_LEFT_DIR,OUTPUT);
  pinMode(MOTOR_LEFT_PWM,OUTPUT);
  pinMode(MOTOR_RIGHT_DIR,OUTPUT);
  pinMode(MOTOR_RIGHT_PWM,OUTPUT);

  digitalWrite(MOTOR_LEFT_DIR , 1);
  digitalWrite(MOTOR_RIGHT_DIR, 1);
}

void moteurs_update_pwm(double cmd_left, double cmd_right)
{
  //update directions
  digitalWrite(MOTOR_LEFT_DIR , (cmd_left>0) ? 0 : 1);
  digitalWrite(MOTOR_RIGHT_DIR, (cmd_right>0) ? 0 : 1);
  
  //update speeds
  analogWrite(MOTOR_LEFT_PWM , abs(cmd_left)); 
  analogWrite(MOTOR_RIGHT_PWM, abs(cmd_right)); 
}
