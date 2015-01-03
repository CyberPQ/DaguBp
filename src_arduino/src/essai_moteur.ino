#include "PID_v1.h"
#include "encodeurs.h"

//Pinout des ponts en H de la carte
#define MOTOR_LEFT_DIR  7
#define MOTOR_LEFT_PWM  9
#define MOTOR_RIGHT_DIR 8
#define MOTOR_RIGHT_PWM 10

//Autres pins
#define LED_BLEUE 13 // LED connected to digital pin 13

//variables
unsigned long timeOld;

volatile boolean changeFlag = false;

double Kp = 81; // 10 rad/sec, 75 phase margin
double Ki = 14;
double Kd = 3;

double consigne_position_cm = 0;
double mesure_position_cm = 0;
double cmd_vitesse_PWM = 0;
int sampleTime = 20;
PID motorPID(&mesure_position_cm, &cmd_vitesse_PWM, &consigne_position_cm,Kp,Ki,Kd, REVERSE); 

void setup(){
  Serial.begin(115200);

  encodeurs_setup();

  pinMode(MOTOR_LEFT_DIR,OUTPUT);
  pinMode(MOTOR_LEFT_PWM,OUTPUT);
  pinMode(MOTOR_RIGHT_DIR,OUTPUT);
  pinMode(MOTOR_RIGHT_PWM,OUTPUT);

  digitalWrite(MOTOR_LEFT_DIR , 1);
  digitalWrite(MOTOR_RIGHT_DIR, 1);

  pinMode(LED_BLEUE, OUTPUT);      // sets the digital pin 13 as output

   Serial.print("Batterie : ");
   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
   float voltage =  analogRead(A7) * (10.0 / 1023.0);
   Serial.print(voltage);
   Serial.println(" volts.");

   //motorPID.SetMode(MANUAL);
   motorPID.SetMode(AUTOMATIC);
   motorPID.SetSampleTime(sampleTime);
   motorPID.SetOutputLimits(-255,255);

   Serial.println("ready...");

   timeOld = millis();
   digitalWrite(LED_BLEUE, 1); //pour signaler la fin de l'init
}

void loop(){

  unsigned long timeNow = millis();

  //Gestion de la reception d'ordres sur le port série
  GestionRxOrdre();

  //PID  des moteurs
  mesure_position_cm = encodeurs_get_distance_cm();
  motorPID.Compute();

  //Mise à jour des commande de moteurs
  digitalWrite(MOTOR_LEFT_DIR , (cmd_vitesse_PWM>0) ? 0 : 1);
  digitalWrite(MOTOR_RIGHT_DIR, (cmd_vitesse_PWM>0) ? 0 : 1);
  analogWrite(MOTOR_LEFT_PWM , abs(cmd_vitesse_PWM)); 
  analogWrite(MOTOR_RIGHT_PWM, abs(cmd_vitesse_PWM)); 

  if (timeNow - timeOld > 10000){
    changeFlag = true;	//provoque l'affichage des traces
    timeOld = timeNow;
    //consigne_position_cm+=5.0;
  }
 
  //Gestion de l'émission du statut de la carte
#ifdef SERIAL_SHELL
  GestionTxStatus();
#endif

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
      case 'z' : motorPID.SetMode(MANUAL); cmd_vitesse_PWM = 0; break;
      case 'a' : consigne_position_cm = mesure_position_cm; motorPID.SetMode(AUTOMATIC); break;
      default  : Serial.println("?");
    }
  }
}

#ifdef SERIAL_SHELL

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

    Serial.print("     SP:");
    Serial.print(consigne_position_cm);
    Serial.print("  lue:");
    Serial.print(mesure_position_cm);

    Serial.print("       vg:");
    Serial.print(cmd_v_g);
    Serial.print("  vd:");
    Serial.println(cmd_v_d);
    digitalWrite(LED_BLEUE, 0);
  }
}
#endif

