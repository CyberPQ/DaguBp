#include "../lib/digitalWriteFast.h"
#include "../lib/PID_v1.h"

//Pinout des ponts en H de la carte
#define MOTOR_LEFT_DIR  7
#define MOTOR_LEFT_PWM  9
#define MOTOR_RIGHT_DIR 8
#define MOTOR_RIGHT_PWM 10

//Pinout des codeurs en quadrature
#define LEFT_A   2 // it externe IT_0
#define LEFT_B   5
#define RIGHT_A  3  // it externe
#define RIGHT_B  4

//Autres pins
#define LED_BLEUE 13 // LED connected to digital pin 13

//variables
int val = 0;     // variable to store the read value
volatile int encoderLeftCount = 0;
volatile int encoderRightCount = 0;
volatile boolean changeFlag = false;
volatile int vitesse = 255;

float cmd_v_g=0;
float cmd_v_d=0;
#define CMD_STEP 0.05f
int consigne_v_g = 0;
int consigne_v_d = 0;

void setup(){
  Serial.begin(115200);
  pinMode(MOTOR_LEFT_DIR,OUTPUT);
  pinMode(MOTOR_LEFT_PWM,OUTPUT);
  pinMode(MOTOR_RIGHT_DIR,OUTPUT);
  pinMode(MOTOR_RIGHT_PWM,OUTPUT);

  digitalWrite(MOTOR_LEFT_DIR , 1); 
  digitalWrite(MOTOR_RIGHT_DIR, 1);
  
  pinMode(LEFT_A,INPUT);
  pinMode(LEFT_B,INPUT);
  pinMode(RIGHT_A,INPUT);
  pinMode(RIGHT_B,INPUT);
  
  pinMode(LED_BLEUE, OUTPUT);      // sets the digital pin 13 as output
  
   attachInterrupt(0, encoderInt_left,  RISING);
   attachInterrupt(1, encoderInt_right, RISING);
   
   Serial.print("Batterie : ");
   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
   float voltage =  analogRead(A7) * (5.0 / 1023.0);
   Serial.print(voltage);
   Serial.println(" volts.");
   
   Serial.println("ready...");
   digitalWrite(LED_BLEUE, 1); //pour signaler la fin de l'init
}

void loop(){
  
  //Gestion de la reception d'ordres sur le port série
  GestionRxOrdre();
  
  //Gestion de l'émission du statut de la carte
  GestionTxStatus();
  
  //Mise à jour des commande de moteurs, avec rampes
  updateMotor();
}

//Gestion de la reception d'ordres sur le port série
void GestionRxOrdre()
{
  if (Serial.available() > 0)
  {
    int incomingByte = Serial.read();
    
    switch(incomingByte)
    {
      case '0' : setMotor(0      , 0      ); encoderLeftCount=0; encoderRightCount=0;break;
      case '1' : setMotor(0      , 0      ); break;
      case '2' : setMotor(-vitesse, -vitesse); break;
      case '3' : setMotor(0      , 0      ); break;
      case '4' : setMotor(-vitesse, +vitesse); break;
      case '5' : setMotor(0      , 0      ); break;
      case '6' : setMotor(+vitesse, -vitesse); break;
      case '7' : setMotor(+vitesse>1, +vitesse); break;
      case '8' : setMotor(+vitesse, +vitesse); break;
      case '9' : setMotor(+vitesse, vitesse>1); break;
      case '+' : vitesse=constrain(vitesse+10, 0, 255); break;
      case '-' : vitesse=constrain(vitesse-10, 0, 255); break;
      default  : Serial.println("commande inconnue !");
    }
  }
}

void setMotor(int vitesse_gauche, int vitesse_droite)
{
  //enregistrement des consignes, qui seront appliquées
  //par 'updateMotor()', avec gestion des pentes 
  consigne_v_g = vitesse_gauche;
  consigne_v_d = vitesse_droite;
}

void updateMotor()
{
  //mise à jour des commandes avec rampes
  if(consigne_v_g>cmd_v_g) cmd_v_g = min(cmd_v_g+CMD_STEP, consigne_v_g);
  if(consigne_v_g<cmd_v_g) cmd_v_g = max(cmd_v_g-CMD_STEP, consigne_v_g);

  if(consigne_v_d>cmd_v_d) cmd_v_d = min(cmd_v_d+CMD_STEP, consigne_v_d);
  if(consigne_v_d<cmd_v_d) cmd_v_d = max(cmd_v_d-CMD_STEP, consigne_v_d);
  
  //positionnement des directions de rotation
  digitalWrite(MOTOR_LEFT_DIR , (cmd_v_g>0) ? 0 : 1); 
  digitalWrite(MOTOR_RIGHT_DIR, (cmd_v_d>0) ? 0 : 1);
  
  //reglage des PWM des vitesses
  analogWrite(MOTOR_LEFT_PWM , abs(cmd_v_g)); 
  analogWrite(MOTOR_RIGHT_PWM, abs(cmd_v_d)); 
}

//Gestion de l'émission du statut de la carte
void GestionTxStatus()
{
  if (changeFlag)
  {
    digitalWrite(LED_BLEUE, 1);
    changeFlag = false;
    Serial.print("L:");
    Serial.print(encoderLeftCount);
    Serial.print("  R:");
    Serial.print(encoderRightCount);
    Serial.print("  vg:");
    Serial.print(cmd_v_g);
    Serial.print("  vd:");
    Serial.println(cmd_v_d);
    digitalWrite(LED_BLEUE, 0);
  }
}

void encoderInt_left() 
{
  if (digitalReadFast(LEFT_B) == HIGH)
    encoderLeftCount++;
  else
    encoderLeftCount--;
  changeFlag = true;
}

void encoderInt_right() 
{
  if (digitalReadFast(RIGHT_B) == HIGH)
    encoderRightCount++;
  else
    encoderRightCount--;
  changeFlag = true;
}
