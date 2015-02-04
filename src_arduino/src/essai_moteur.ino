#include "PID_v1.h"
#include "encodeurs.h"
#include "moteurs.h"

//Autres pins
#define LED_BLEUE 13 // LED connected to digital pin 13

//variables
unsigned long timeOld;

volatile boolean changeFlag = false;

double Kp = 81; // 10 rad/sec, 75 phase margin
double Ki = 14;
double Kd = 3;

double consigne_position_cm = 0;
double consigne_rotation_deg = 0;
double mesure_position_cm = 0;
double mesure_rotation_deg = 0;
double cmd_distance_PWM = 0;
double cmd_rotation_PWM = 0;
int sampleTime = 20;
PID distancePID(&mesure_position_cm, &cmd_distance_PWM, &consigne_position_cm,Kp,Ki,Kd, REVERSE); 
PID rotationPID(&mesure_rotation_deg, &cmd_rotation_PWM, &consigne_rotation_deg,10,0,0, REVERSE); 

void setup(){
  Serial.begin(115200);

  encodeurs_setup();
  moteurs_setup();

  pinMode(LED_BLEUE, OUTPUT);      // sets the digital pin 13 as output

   Serial.print("Batterie : ");
   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 10V):
   float voltage =  analogRead(A7) * (10.0 / 1023.0);
   Serial.print(voltage);
   Serial.println(" volts.");

   //distancePID.SetMode(MANUAL);
   distancePID.SetMode(AUTOMATIC);
   distancePID.SetSampleTime(sampleTime);
   distancePID.SetOutputLimits(-255,255);

   Serial.println("ready...");

   timeOld = millis();
   digitalWrite(LED_BLEUE, 1); //pour signaler la fin de l'init
}

void loop(){

  unsigned long timeNow = millis();

  //Gestion de la reception d'ordres sur le port série
  GestionRxOrdre();

  //Pilotage des moteurs
  mesure_position_cm = encodeurs_get_distance_cm();
  distancePID.Compute();
  rotationPID.Compute();
  moteurs_update_pwm(cmd_distance_PWM+cmd_rotation_PWM, cmd_distance_PWM-cmd_rotation_PWM);

  if (timeNow - timeOld > 1000){
    changeFlag = true;	//provoque l'affichage des traces
    timeOld = timeNow;
    //consigne_position_cm+=5.0;
  }
 
  //Gestion de l'émission du statut de la carte
  GestionTxStatus();

}


//Gestion de la reception d'ordres sur le port série
void GestionRxOrdre()
{
  if (Serial.available() > 0)
  {
    int incomingByte = Serial.read();

    switch(incomingByte)
    {
      case 'p' : consigne_position_cm+=5; break;
      case 'm' : consigne_position_cm-=5; break;
      case 'z' : distancePID.SetMode(MANUAL); cmd_distance_PWM = 0; break;
      case 'a' : consigne_position_cm = mesure_position_cm; distancePID.SetMode(AUTOMATIC); break;
      default  : Serial.println("?");
    }
  }
}

//Gestion de l'émission du statut de la carte
void GestionTxStatus()
{
  if (changeFlag)
  {
    digitalWrite(LED_BLEUE, 1);
    changeFlag = false;
    Serial.print("D:");
    Serial.print(encodeurs_get_distance());
    Serial.print("  a:");
    Serial.print(encodeurs_get_angle());

    Serial.print("      L:");
    Serial.print(encodeurs_get_left());
    Serial.print("  R:");
    Serial.print(encodeurs_get_right());

    Serial.print("     SP:");
    Serial.print(consigne_position_cm);
    Serial.print("  lue:");
    Serial.print(mesure_position_cm);

    Serial.print("       vg:");
    Serial.print(cmd_distance_PWM);
    Serial.print("  vd:");
    Serial.println(cmd_distance_PWM);
    digitalWrite(LED_BLEUE, 0);
  }
}

