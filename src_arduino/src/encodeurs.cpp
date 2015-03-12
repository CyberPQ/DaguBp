#include "encodeurs.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "../lib/digitalWriteFast.h"

//Pinout des codeurs en quadrature
#define LEFT_A   2  // it externe IT_0
#define LEFT_B   5
#define RIGHT_A  3  // it externe IT_1
#define RIGHT_B  4

#define cm2count  ((double) 17.6839 / 2)
#define count2cm  ((double) 1/cm2count)

//variables
static volatile int encoderLeftCount = 0;
static volatile int encoderRightCount = 0;

void encodeurs_reset()
{
  encoderLeftCount = 0;
  encoderRightCount = 0;
}

//==============================================================================================
//
//      Avance -->                   ______        ______
//                      A     ------I      I------I      I--
//      Recule <--               ______        ______
//                      B ------I      I------I      I------
//
// Donc, juste après un changement d'état sur la pin A a lieu (dans le handler d'IT sur 
// changement de pin A), si A=B, c'est qu'on avance, mais si A=/B, c'est qu'on recule.
//==============================================================================================
void encoderInt_left() 
{
  if (digitalReadFast(LEFT_A) == digitalReadFast(LEFT_B))
    encoderLeftCount++;
  else
    encoderLeftCount--;
}

void encoderInt_right() 
{
  if (digitalReadFast(RIGHT_A) == digitalReadFast(RIGHT_B))
    encoderRightCount++;
  else
    encoderRightCount--;
}

void encodeurs_setup()
{
      pinMode(LEFT_A,INPUT);
      pinMode(LEFT_B,INPUT);
      pinMode(RIGHT_A,INPUT);
      pinMode(RIGHT_B,INPUT);
      
      // turn on pullup resistor
      digitalWrite(LEFT_A, HIGH);
      digitalWrite(LEFT_B, HIGH);
      digitalWrite(RIGHT_A, HIGH);
      digitalWrite(RIGHT_B, HIGH);
                  
      attachInterrupt(0, encoderInt_left,  CHANGE);
      attachInterrupt(1, encoderInt_right, CHANGE);
}

//-------------------------------------------------
// Moyenne entre l'encodeur roite et gauche = 
//   distance parcourue par le centre du robot
int encodeurs_get_distance()
{
  return ( (encoderLeftCount+encoderRightCount) / 2 );
}

double encodeurs_get_distance_cm()
{
  return ( (double)(encodeurs_get_distance()) * count2cm );
}

//-------------------------------------------------
// Angle du robot (différence entre gauche et droite)
// L'angle est positif en tournant vers la droite, comme
// avec un compas géographique (=boussole)
int encodeurs_get_angle()
{
  return (encoderLeftCount-encoderRightCount);
}

int encodeurs_get_left()
{
  return encoderLeftCount;
}

int encodeurs_get_right()
{
  return encoderRightCount;
}

