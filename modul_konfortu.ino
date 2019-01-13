// Created by Yahoo and Koniu
// -----   Światła -------

#define ZMIERZCH 0.7
 

const int PIN_DRL = 11;
const int PIN_HOME_ARRIVAL = 6;
const int PIN_Swiatla_MIJANIA = A1;
const int PIN_kluczyk = A2;
const int PIN_kierownica = A3;
const int PIN_light_L = 3;
const int PIN_fotorezystor = A4;
const int PIN_mglowe = A6;
const int PIN_light_R = 9;
const int PIN_mijania = 12;
const float left_threshold = 4.0;
const float right_threshold = 1.35;
bool left_on = false;
bool right_on = false;
int analogInput = A0;
float vout = 0.0;
float vin = 0.0;
float R1 = 100000.0; // R1(100K)
float R2 = 10000.0; //  R2 (10K)
int value = 0;
int a[5] = {};
bool wlaczone = false;
bool swiatla_mijania_zapalone = false;
float swiatla_volt ;
float kluczyk_volt;
float kluczyk_prev = .0;
float kierownica_volt =.0;
float fotorezystor_volt = .0;
float mglowe_volt = .0; 
bool light_L = false, light_R = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_mglowe,INPUT);
  pinMode(analogInput,INPUT);
  pinMode(PIN_mijania,OUTPUT);
  pinMode(PIN_kierownica,INPUT);
  pinMode(PIN_light_L,OUTPUT);
  pinMode(PIN_light_R,OUTPUT);
  pinMode(PIN_kluczyk,INPUT);
  pinMode(PIN_DRL,OUTPUT);
  pinMode(PIN_HOME_ARRIVAL,OUTPUT);
  pinMode(PIN_Swiatla_MIJANIA,INPUT);
  pinMode(PIN_fotorezystor,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  value = analogRead(analogInput);
  vout = (value * 5) / 1024.0;
  vin = vout / (R2 / (R1 + R2));
  swiatla_volt = analogRead(PIN_Swiatla_MIJANIA);
  swiatla_volt = (swiatla_volt * 5) / 1024.0 ;
  swiatla_volt = swiatla_volt / (R2 / (R1 + R2));
  kluczyk_volt = analogRead(PIN_kluczyk);
  kluczyk_volt = (kluczyk_volt * 5) / 1024.0 ;
  kluczyk_volt = kluczyk_volt / (R2 / (R1 + R2));
  kierownica_volt = analogRead(PIN_kierownica);
  kierownica_volt *= 5.0/1024.0 ;
  fotorezystor_volt = analogRead(PIN_fotorezystor);
  fotorezystor_volt *= 5.0/1024.0;
  mglowe_volt = analogRead(PIN_mglowe); 
  mglowe_volt *= 5.0/1024.0;
  mglowe_volt = mglowe_volt / (R2 / (R1 + R2));
 
  //Serial.println(kierownica_volt);
  Serial.println(mglowe_volt);
  //Serial.println(swiatla_volt);
  //Serial.println(vin);
  if(vin > 13.4 && !wlaczone && abs(swiatla_volt) < 0.35)
  {
    //wlacza
    wlaczone = true;
    for(int i=0;i<=255;i++)
    {
      analogWrite(PIN_DRL,i);
      delay(15);  
    }  
  } else if( vin < 13 ){
    //wylacza
   if(swiatla_mijania_zapalone || swiatla_volt > 8 ){
    swiatla_mijania_zapalone = false;
      for(int i=255;i>=0;i--)
        {
            analogWrite(PIN_mijania,i);
            delay(5);
        }
   }
        if(wlaczone){
           wlaczone = false;
    for(int i=255;i>=0;i--)
    {
        analogWrite(PIN_DRL,i);
        delay(5);
    }
        }
  } else if(vin > 13.4 && wlaczone && swiatla_volt > 8)
  {
      wlaczone = false;
    for(int i=255;i>=0;i--)
    {
        analogWrite(PIN_DRL,i);
        delay(5);
    }
  }
  if(vin > 13.4 && swiatla_volt < 8 && fotorezystor_volt < ZMIERZCH && !swiatla_mijania_zapalone)
  {
    swiatla_mijania_zapalone = true;
     for(int i=0;i<=255;i++)
      {
        analogWrite(PIN_mijania,i);
        delay(15);  
      }
      if(wlaczone){  
        wlaczone = false;
        
        for(int i=255;i>=0;i--)
        {
            analogWrite(PIN_DRL,i);
            delay(5);
        }
      }
  } else if(vin > 13.4 && swiatla_volt < 8 && fotorezystor_volt > ZMIERZCH){
      if(swiatla_mijania_zapalone)
      {
          swiatla_mijania_zapalone = false;
            for(int i=255;i>=0;i--)
        {
            analogWrite(PIN_mijania,i);
            delay(5);
        }
      }
      if(!wlaczone)
      {
        wlaczone = true;
          for(int i=0;i<=255;i++)
      {
        analogWrite(PIN_DRL,i);
        delay(15);  
      }
      }    
  }

//  if(vin > 13.4 && mglowe_volt > 13.4)
//  {
//    if(wlaczone)
//    {
//        wlaczone = false;        
//        for(int i=255;i>=0;i--)
//        {
//            analogWrite(PIN_DRL,i);
//            delay(2);
//        }
//    }
//    if(!light_L)
//    {
//         for(int i=0;i<=255;i++)
//    {
//      //analogWrite(PIN_HOME_ARRIVAL,i);
//      light_L = true;
//      analogWrite(PIN_light_L,i);
//    }  
//  }
//  if(!light_R)
//    {
//         for(int i=0;i<=255;i++)
//    {
//      //analogWrite(PIN_HOME_ARRIVAL,i);
//      light_R = true;
//      analogWrite(PIN_light_R,i);
//    }  
//  }
//  }
  
  if(vin < 13 && !wlaczone && abs(swiatla_volt) < 0.35 && kluczyk_zadzialal(kluczyk_volt) && fotorezystor_volt < ZMIERZCH)
  {
       for(int i=0;i<=255;i++)
    {
      //analogWrite(PIN_HOME_ARRIVAL,i);
      light_L = light_R = true;
      analogWrite(PIN_light_L,i);
      analogWrite(PIN_light_R,i);
      delay(15);  
    }  
    delay(5000);
    for(int i=255;i>=0;i--)
    {
      light_L = light_R = false;
        //analogWrite(PIN_HOME_ARRIVAL,i);
        analogWrite(PIN_light_L,i);
        analogWrite(PIN_light_R,i);
        delay(5);
    }
  }
  if(vin > 13.4 && (swiatla_volt > 8 || digitalRead(PIN_mijania) == HIGH) && !wlaczone){
  if(kierownica_volt < right_threshold && !light_L)
  {
    light_L = true;
    if(light_R)
      {
        light_R = false;
        for(int i=255;i>=0;i--)
        {
          analogWrite(PIN_light_R,i);
          delay(2);
        }  
      }
      for(int i=0;i<=255;i++)
    {
      analogWrite(PIN_light_L,i);
      delay(4);  
    }  
  }
  else if(kierownica_volt > left_threshold && !light_R)
  {
    light_R = true;
     if(light_L)
      {
        light_L = false;
        
        for(int i=255;i>=0;i--)
        {
          analogWrite(PIN_light_L,i);
          delay(2);
        }  
      }
      for(int i=0;i<=255;i++)
    {
      analogWrite(PIN_light_R,i);
      delay(4);  
    }  
  }
  else if(kierownica_volt >= right_threshold+0.5 && kierownica_volt <=left_threshold-0.5)
  {
      if(light_R)
      {
        light_R = false;
        for(int i=255;i>=0;i--)
        {
          analogWrite(PIN_light_R,i);
          delay(2);
        }  
      }
      if(light_L)
      {
        light_L = false;
        
        for(int i=255;i>=0;i--)
        {
          analogWrite(PIN_light_L,i);
          delay(2);
        }  
      }
  }
  }
}


bool kluczyk_zadzialal(float kluczyk)
{
  if(abs(kluczyk - kluczyk_prev) > 10){
    kluczyk_prev = .0;
    return true;
  }
  else{
    kluczyk_prev = kluczyk;
    return false;   
  }
}
