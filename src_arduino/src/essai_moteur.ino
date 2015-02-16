#include "PID_v1.h"
#include "encodeurs.h"
#include "moteurs.h"

//Autres pins
#define LED_BLEUE 13 // LED connected to digital pin 13

//variables
unsigned long timeOld;

double Kp = 81; // 10 rad/sec, 75 phase margin
double Ki = 10;
double Kd = 5;

double consigne_position_cm = 0;
double consigne_rotation_deg = 0;
double mesure_position_cm = 0;
double mesure_rotation_deg = 0;
double cmd_distance_PWM = 0;
double cmd_rotation_PWM = 0;
int sampleTime = 20;

PID distancePID(&mesure_position_cm, &cmd_distance_PWM, &consigne_position_cm,Kp,Ki,Kd, REVERSE);
PID rotationPID(&mesure_rotation_deg, &cmd_rotation_PWM, &consigne_rotation_deg,4,1,0, DIRECT);

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
  
  distancePID.SetMode(AUTOMATIC);
  distancePID.SetSampleTime(sampleTime);
  distancePID.SetOutputLimits(-255,255);

  rotationPID.SetMode(AUTOMATIC);
  rotationPID.SetSampleTime(sampleTime);
  rotationPID.SetOutputLimits(-100,100);
  
  Serial.println("ready...");
  
  timeOld = millis();
  digitalWrite(LED_BLEUE, 1); //pour signaler la fin de l'init
}

void loop(){

  unsigned long timeNow = millis();

  //Gestion de la reception d'ordres sur le port série
  GestionRxOrdre();

  //Pilotage des moteurs
  mesure_position_cm  = encodeurs_get_distance_cm();
  mesure_rotation_deg = encodeurs_get_angle();          // TODO à remplacer par un compas ou giro-compas (IMU)
  distancePID.Compute();
  rotationPID.Compute();
  moteurs_update_pwm(cmd_distance_PWM+cmd_rotation_PWM, cmd_distance_PWM-cmd_rotation_PWM);

  if (timeNow - timeOld > 1000){
    //Gestion de l'émission du statut de la carte
    GestionTxStatus();
    timeOld = timeNow;
  }
 
}


//Gestion de la reception d'ordres sur le port série
void GestionRxOrdre()
{
  if (Serial.available() > 0)
  {
    int incomingByte = Serial.read();

    switch(incomingByte)
    {
      case 'h' :
      case 'H' :
        Serial.println("PILOTAGE : ");
        Serial.println("           ");
        Serial.println("     P       Mode auto     : A");
        Serial.println("     ^       Mode repos    : Z");
        Serial.println("     |       Mode autotune : T (todo)");
        Serial.println(" W <-+-> X ");
        Serial.println("     |     ");
        Serial.println("     v     ");
        Serial.println("     M     ");
        Serial.println("           ");
        break;
        
      case 'p' :
      case 'P' :
        consigne_position_cm+=5; 
        break;
        
      case 'm' : 
      case 'M' : 
        consigne_position_cm-=5; 
        break;
        
      case 'w' : 
      case 'W' : 
        consigne_rotation_deg -= 20; 
        break;
        
      case 'x' : 
      case 'X' : 
        consigne_rotation_deg += 20;
        break;
        
      case 'z' : 
      case 'Z' : 
        distancePID.SetMode(MANUAL); 
        rotationPID.SetMode(MANUAL); 
        cmd_distance_PWM = 0; 
        cmd_rotation_PWM = 0;
        break;
        
      case 'a' : 
      case 'A' : 
        consigne_position_cm  = mesure_position_cm; 
        consigne_rotation_deg = mesure_rotation_deg;
        distancePID.SetMode(AUTOMATIC); 
        rotationPID.SetMode(AUTOMATIC);
        break;
        
      default  : 
        Serial.println("?");
    }
  }
}

//Gestion de l'émission du statut de la carte
void GestionTxStatus()
{
    digitalWrite(LED_BLEUE, 1);
    
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
    Serial.print("/");
    Serial.print(consigne_rotation_deg);
    Serial.print("     lue:");
    Serial.print(mesure_position_cm);
    Serial.print("/");
    Serial.print(mesure_rotation_deg);

    Serial.print("       vg:");
    Serial.print(cmd_distance_PWM+cmd_rotation_PWM);
    Serial.print("  vd:");
    Serial.print(cmd_distance_PWM-cmd_rotation_PWM);
    
    Serial.print("       (cmd = ");
    Serial.print(cmd_distance_PWM);
    Serial.print(" +/- ");
    Serial.print(cmd_rotation_PWM);
    Serial.println(" )");

    digitalWrite(LED_BLEUE, 0);
}

