#include "encodeurs.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "../lib/digitalWriteFast.h"

//Pinout des codeurs en quadrature
#define LEFT_A   2 // it externe IT_0
#define LEFT_B   5
#define RIGHT_A  3  // it externe
#define RIGHT_B  4

#define cm2count  ((double) 17.6839)
#define count2cm  ((double) 1/17.6839)

//variables
static volatile int encoderLeftCount = 0;
static volatile int encoderRightCount = 0;

void encodeurs_reset()
{
  encoderLeftCount = 0;
  encoderRightCount = 0;
}

void encoderInt_left() 
{
  if (digitalReadFast(LEFT_B) == HIGH)
    encoderLeftCount++;
  else
    encoderLeftCount--;
}

void encoderInt_right() 
{
  if (digitalReadFast(RIGHT_B) == HIGH)
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
      
      attachInterrupt(0, encoderInt_left,  RISING);
      attachInterrupt(1, encoderInt_right, RISING);
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
// L'angle est positif en tournat vers la droite, comme
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

