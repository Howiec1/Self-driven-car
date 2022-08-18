
int bryter = 9 ;   //trykkknapp koblet til pinne D9
int hovedRod = 3;   //rødt lys er koblet til pinne D3
int hovedGul = 4;   // gult lys til pinne D4
int hovedGronn = 5;  //grønt lys til pinne D5

int IRledPin =  10;    // IR LED koblet til digital pinne 13

void setup() {

  Serial.begin (9600);  //Seriell kommunikasjon for bryter og sensor
  
  // initialiserer IR LED Pin og 3,4 og 5 som OUTPUT, og bryter som INPUT
  pinMode(IRledPin, OUTPUT);      
  pinMode (bryter, INPUT);
  pinMode(hovedRod, OUTPUT);
  pinMode(hovedGul, OUTPUT);
  pinMode(hovedGronn, OUTPUT);
}

void loop()
{
  // Dersom ingenting skjer, lys kunn grønt lys
  VaerGronnHovedgate();
  
  int starttid=0;

  if (bryterTrykket()==HIGH)   // utfør dette dersom bryteren/knappen trykkes ned
  {
 
    BliRodHovedgate();
    delay(1);    //bare for ordens skyld.
    
    starttid = millis();  //Starttid. Tidtaker, slik at vi kunn er her inne i 5 sekunder. 5 = sluttidRoodtLys - Starttid
    
    VaerRodHovedgate (starttid);
 
    BliGronnHovedgate();   // når tiden er ute (bestemmes i funksjon VaerRodHovedgate) skal vi gå over til å bli grønt lys igjen
   
  }

}

void VaerGronnHovedgate ()  //Når lyskrysset skal lyse grønt
{
  digitalWrite(IRledPin, LOW);  //IRLed Avslått. Setter IRledPin lav for å forsikre oss om at den aldri sender ut signal når den ikke skal
  digitalWrite(hovedRod, LOW);   // Rød LED Avslått
  digitalWrite(hovedGul, LOW);    //Gul LED  Avslått
  digitalWrite(hovedGronn, HIGH);  //Kun grønn LED som skal lyse
}


void VaerRodHovedgate (int Starttid)  // Når lyskrysset skal lyse rødt lys, settes alt lavt først (bortsett fra rød LED)
{
  digitalWrite(IRledPin, LOW);   /* til og med denne, som rett etterpå skal sende ut frekvens 38kHz til IRreciever.
                                  Setter IRledPin lav for å forsikre oss om at den aldri sender ut signal når den ikke skal*/
  digitalWrite(hovedRod, HIGH);
  digitalWrite(hovedGul, LOW);
  digitalWrite(hovedGronn, LOW);
  int sluttidRoodtLys =0;
  
  while (1)   //while-loop som sørger for at IRLed sender riktig frekvens og puls til mottaker
  {
  //int Sluttid=millis();
    sendSignaturPuls(); 
    sluttidRoodtLys = millis();  // henter ut en millis()-verdi for tidtaking
    
    if (sluttidRoodtLys - Starttid >= 5000) 
    {
      Serial.println("Naa har jeg breaket");     //Si ifra rett før den breaker
      Serial.println(sluttidRoodtLys-Starttid);  // 
      break;
    }
  } 
}



void BliRodHovedgate()   // Overgangsfunksjon fra grønt lys til rødt lys
{
  digitalWrite(IRledPin, LOW);  //Setter IRledPin lav for å forsikre oss om at den aldri sender ut signal når den ikke skal
  digitalWrite(hovedRod, LOW);
  digitalWrite(hovedGronn, LOW);
  digitalWrite(hovedGul, HIGH);
  delay(1000);
  digitalWrite(hovedGul, LOW);
  digitalWrite(hovedRod, HIGH);
}

void BliGronnHovedgate () // Overgangsfunksjon fra rødt lys til grønt lys
{
  digitalWrite(IRledPin, LOW);  //Setter IRledPin lav for å forsikre oss om at den aldri sender ut signal når den ikke skal
  digitalWrite(hovedGul, HIGH);
  delay(1000);
  digitalWrite(hovedRod, LOW);
  digitalWrite(hovedGul, LOW);
  digitalWrite(hovedGronn, HIGH);
}

void sendSignaturPuls()   // heksadesimalt = 22AE7A2A
{
  // Dette er koden for vårt spesielle heksa-tall
 
  pulseIR(2080);  //Sender IRsignal i 2080 millisekunder med 38kHz som produseres av pulseIR()-funksjonen
  delay(27);   //Venter i 27 millisekudner
  pulseIR(440);
  delayMicroseconds(1500);
  pulseIR(460);
  delayMicroseconds(3440);
  pulseIR(480);
 
}

void pulseIR(long microsecs)    // funksjon som sender frekvens lik 38KHz, som vi har modellert selv
{
 
  while (microsecs > 0) 
  {
    // 38 kHz er omtrent 13 microsekunder høy og  13 microsekunder lav 
   digitalWrite(IRledPin, HIGH);  // dette tar omtrent 3 mikrosekunder
   delayMicroseconds(10);         // venter i 10 mikrosekunder
   digitalWrite(IRledPin, LOW);   // dette tar også omtrent 3 mikrosekunder
   delayMicroseconds(10);         // vent ca 10 mikrosek
 
   // så dette blir tilsammen 26 mikrosekunder
   
   microsecs -= 26;   //når microsecs blir 0 (26-26) hopper den ut av while-loopen over
  }

}


int bryterTrykket()     // lager en funksjon som returnerer "sant" når bryteren er trykket inn
{
  int bryterStatus = digitalRead(9);  //bryterstatus blir lik 0(ikke trykket) eller 1 (trykket ned). (digitalread av input-pinne 9)
  return bryterStatus == HIGH;   //Send tilbake "sant" (lik: 1) når bryteren er trykket ned
}
