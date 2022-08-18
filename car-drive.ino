#include <Servo.h>
#include <IRremote.h>   //bibliotek for IR mottaker

Servo myservo;

void stopRedLight();    //Funksjon som skal utføres for interrupt! Stopper bil ved rødt lys

int RECV_PIN = 4;      //mottaker koblet til å pinne 3
IRrecv irrecv(RECV_PIN);
decode_results results;
const byte InterruptPin = 3 ; //Interrupt signal må komme inn på pinne to. Splitter data-in signal fra IRmottaker som også går ned i pinnt 11.


unsigned char flag = 3;   //Variabel for sensorer på bil, forteller hvilken av sensorene som leser svart strek
unsigned char angle = 90;  //variabel vinkel/bil; sette motor til denne vinkel for å svinge slik at bilen holder seg på streken


#define ENA_PIN   5 // ENA til pin5  
#define ENB_PIN   6// ENB til pin6
#define MOTOR_V_1 8 // MOTOR_L_1 til pin8
#define MOTOR_V_2 9 // MOTOR_L_1 til pin9
#define MOTOR_H_1 10 // MOTOR_R_1 til pin10
#define MOTOR_H_2 11 // MOTOR_R_1 til pin11

#define FORWARD 0  // Definerer at fremover = 0. Bilen kjører fremover
#define BACK    1 // Definerer at bakover= 0. Bilen kjører bakover

#define VINKEL_HOYRE_MAKS  115 // definerer ulike rotasjonsvinkler for servoen
#define VINKEL_HOYRE_HALV 107
#define VINKEL_VENSTRE_MAKS   65
#define VINKEL_VENSTRE_HALV  73
#define VINKEL_MIDT    90
// Setter servoen i riktig posisjon
#define LYSSENSOR_V_1_PIN  A0 // Linetracking sensor 1 på analog pinne A0
#define LYSSENSOR_V_2_PIN  A1 // Linetracking sensor 2 på analog pinne A1
#define LYSSENSOR_MID_PIN  A2 // Linetracking sensor 3 på analog pinne A2
#define LYSSENSOR_H_1_PIN A3 // Linetracking sensor 4 på analog pinne A3
#define LYSSENSOR_H_2_PIN A5 // Linetracking sensor 5 på analog pinne A5
#define SPEED 175 //Rotasjonshastighet til motoren

void setup()
{
  Serial.begin(9600);
  /* setter pinnene under som OUTPUT */
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(MOTOR_V_1, OUTPUT);
  pinMode(MOTOR_V_2, OUTPUT);
  pinMode(MOTOR_H_1, OUTPUT);
  pinMode(MOTOR_H_2, OUTPUT);
  
  myservo.attach(2);//servo kobles til pinne 3  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  myservo.write(90);//setter utgangsposisjon til servo rett frem (90 grader)

  irrecv.enableIRIn(); // Start the reciever
  attachInterrupt(digitalPinToInterrupt(InterruptPin), stopRedLight, RISING) ;

}


//    V1    V2        MID      H2        H1
void loop()
{
  Serial.println("Starter voidloop");
  if (analogRead(LYSSENSOR_V_1_PIN) > 758) //Dersom Linetracking 1 sensor leser verdi mer enn 758
    flag = 1;
  else if (analogRead(LYSSENSOR_V_2_PIN) > 758) //Dersom Linetracking 2 sensor leser verdi mer enn 758
    flag = 2;
  else if (analogRead(LYSSENSOR_MID_PIN) > 758) //Dersom Linetracking 3 sensor leser verdi mer enn 758
    flag = 3;
  else if (analogRead(LYSSENSOR_H_2_PIN) > 758) //Dersom Linetracking 4 sensor leser verdi mer enn 758
    flag = 4;
  else if (analogRead(LYSSENSOR_H_1_PIN) > 758) //Dersom Linetracking 5 sensor leser verdi mer enn 758
    flag = 5;
  else
    flag = 6;

  switch (flag) //Finner riktig vinkel til motor for å kjøre tilbake til strek;
  {
    case 1: angle = VINKEL_VENSTRE_MAKS;   break;
    case 2: angle = VINKEL_VENSTRE_HALV;  break;
    case 3: angle = VINKEL_MIDT;     break;
    case 4: angle = VINKEL_HOYRE_HALV; break;
    case 5: angle = VINKEL_HOYRE_MAKS;  break;
    case 6: angle = angle;            break;
    default: break;
  }
  myservo.write(angle);//Vrir motoren til riktig vinkel (angle)
  CAR_move(FORWARD, SPEED, SPEED); //Servo kjører med hastighet 180
}

void CAR_move(unsigned char direction, unsigned char speed_left, unsigned char speed_right)
{
  // funksjonen trener RETNING, HASTIGHET (på begge motorer), denne hastigheten enabler også via ENA_PIN/ENB_PIN)

  switch (direction)
  {
    //bil kjører med hastighet 180
    case 0: digitalWrite(MOTOR_V_1, HIGH); //motorer beveger seg med klokken
      digitalWrite(MOTOR_V_2, LOW);
      digitalWrite(MOTOR_H_1, HIGH);
      digitalWrite(MOTOR_H_2, LOW); break;
    //car move back with speed 180
    case 1: digitalWrite(MOTOR_V_1, LOW); //motorer beveger seg mot klokken
      digitalWrite(MOTOR_V_2, HIGH);
      digitalWrite(MOTOR_H_1, LOW);
      digitalWrite(MOTOR_V_2, HIGH); break;
    default: break;
  }
  analogWrite(ENA_PIN, speed_left); //enable signal. Dersom speed_left er høy, tillat rotasjon på venstre motor.
  analogWrite(ENB_PIN, speed_right); //enable signal. Dersom speed_right er høy, tillat rotasjon på høyre motor.
}

void stopRedLight()   //Setter enable-pin til motorer LOW, slik at motorene stopper.
{
  analogWrite(ENA_PIN, LOW);
  analogWrite(ENB_PIN, LOW);

//  static bool slutt = false;
//  if (slutt) return;
//  else slutt = true;
 
//  myservo.write(90);

  if (irrecv.decode(&results))   // for å dekryptere og printe ut IR-signal i HEXA tall
  {
    Serial.println(results.value, HEX);     //skriver ut resultatverdi fra IR-mottaker i hexa-tall
    irrecv.resume();  // resume, (returner) og motta neste verdi
  }

}
