//parametry
const float engineVoltThresholdOn = 8; //volty wzbudzenie alternatora
const float engineVoltThresholdOff = 3;  //volty akumulator
const int dayLightsPower = 255; //moc swiatel dziennych 0-255
const int dayLightsDelay = 7; //opoznienie dziennych zal\wyl - w milisekundach 
const float beamLightsVoltThresholdOn = 10.0; //volty od swiatel mijania do wlaczania
const float beamLightsVoltThresholdOff = 6.0; //volty od mijania do wylaczania
const float wheelLeft = 1.35; //skret levy volty
const float wheelRight = 4.0; //skret prawy volty
const int leftRightSpeed = 2; //nie ruszac bo dobrze jest.
const int homeAssistDelay = 10; //w sekundach ile swieci home assist
const int drlDelay = 5; //w sekundach po ilu sie wlaczaja drl 
//stałe
const float R1 = 100000.0;
const float R2 = 10000.0;

// PINY
const int pinEngineIn = A0;
const int pinDayLightsOut = 3;
const int pinBuzzer = 4;
const int pinBeamLightsIn = A1;
const int pinAuxLightLeft = 5;
const int pinAuxLightRight = 9;
const int pinWheelIn = A2;
const int pinFogLights = A3;
const int pinKey = A4;

//zmienne
unsigned long timestamp;
bool timestampStart = false;
bool engineOn = false;
float engineVolt;
float lightsVolt;
float fogLightsVolt;
float wheelVolt;
float keyVolt;
float keyPrev = .0;
float ignitionPrev = .0;
bool dayLightsOn = false;
bool beamLightsOn = false;
bool fogLightsOn = false;
bool leftAuxOnProcess = false;
bool leftAuxOffProcess = false;
bool rightAuxOnProcess = false;
bool rightAuxOffProcess = false;
bool leftAuxOn = false;
bool rightAuxOn = false;

int leftValue = 0;
int rightValue = 0;

void setup() {
  
  Serial.begin(9600);
  pinMode(pinEngineIn,INPUT);
  pinMode(pinDayLightsOut,OUTPUT);
  pinMode(pinBeamLightsIn,INPUT);
  pinMode(pinBuzzer,OUTPUT);
  pinMode(pinAuxLightLeft,OUTPUT);
  pinMode(pinAuxLightRight,OUTPUT);
  pinMode(pinWheelIn,INPUT);
  pinMode(pinFogLights,INPUT);
  pinMode(pinKey,INPUT);
}

void loop() {
  
  engineVolt = readVolt(pinEngineIn);
  if(engineVolt < engineVoltThresholdOff)engineOn = false;
  lightsVolt = readVolt(pinBeamLightsIn);
  fogLightsVolt = readVolt(pinFogLights);
  wheelVolt = (analogRead(pinWheelIn)*5.0)/1024.0;
  if(lightsOn(engineVolt,lightsVolt))beamLightsOn = true;
  if(lightsOff(engineVolt,lightsVolt))beamLightsOn = false;
  if(fogLightsVolt > 10.0)fogLightsOn = true;
  // ------ DRL ------
  if(engineVolt > engineVoltThresholdOn && !dayLightsOn && !beamLightsOn)
  {
      dayLightsOn = true;
      if(!engineOn)delay(drlDelay*1000);
      engineOn = true;
      buzz(true);
      fadeIn(pinDayLightsOut,dayLightsPower,dayLightsDelay);
  }
  else if(engineVolt < engineVoltThresholdOff  && dayLightsOn && !beamLightsOn)
  {
    buzz(false);
      dayLightsOn = false;
      fadeOut(pinDayLightsOut,dayLightsPower,dayLightsDelay);  
  }
  else if( engineVolt > engineVoltThresholdOn && beamLightsOn && dayLightsOn)
  {      
    buzz(false);
      dayLightsOn = false;
      fadeOut(pinDayLightsOut,dayLightsPower,dayLightsDelay);  
  }
  else if(engineVolt > engineVoltThresholdOn && !beamLightsOn && !dayLightsOn)
  {      
    buzz(true);
      dayLightsOn = true;
      fadeIn(pinDayLightsOut,dayLightsPower,dayLightsDelay);
  }
  // ------ END DRL ------

  // ------ AUX LIGHTS ------
  if(beamLightsOn && !fogLightsOn)
  {      
      if(leftAuxOnProcess)leftOn(leftValue);
      if(leftAuxOffProcess)leftOff(leftValue);
      if(rightAuxOnProcess)rightOn(rightValue);
      if(rightAuxOffProcess)rightOff(rightValue);
      if(wheelVolt < wheelLeft && !leftAuxOn)
      {       
          leftAuxOnProcess = true;   
          leftOn(leftValue);
          //rightOff(rightValue);
      }
      if(wheelVolt > wheelRight && !rightAuxOn)
      {   
        rightAuxOnProcess = true;     
        rightOn(rightValue);  
        //leftOff(leftValue);
      }
      if(wheelVolt >= wheelLeft+0.5 && leftAuxOn) //>= 1.85
      {
        leftAuxOffProcess = true;
        leftOff(leftValue);  
      }
      if(wheelVolt <= wheelRight-0.5 && rightAuxOn)// <= 3.5
      {
        rightAuxOffProcess = true;
        rightOff(rightValue);  
      }    
    
  } else if(!beamLightsOn)
  {    
    leftOff(leftValue);
    rightOff(rightValue);  
  } else if(fogLightsVolt < 6.0)
  {    
      while(leftAuxOn || rightAuxOn){
        leftOff(leftValue);
        rightOff(rightValue);     
      }
      fogLightsOn = false;
  } else if(fogLightsVolt > 10.0)
    {
      while(!leftAuxOn || !rightAuxOn){
        leftOn(leftValue);
        rightOn(rightValue);
        fogLightsOn = true;
      }
    }  
  
  // ------ END AUX LIGHTS ------

  // ------ HOME ASSIST ------
  if(engineVolt < engineVoltThresholdOff)
  {
    keyVolt = readVolt(pinKey);
    if(keyLock(keyVolt))
    {
      for(int i=0;i<255;i++){
      leftOn(leftValue);
      rightOn(rightValue);
      delay(10);
      }
      delay(homeAssistDelay*1000);
      for(int i=255;i>=0;i--){
      leftOff(leftValue);
      rightOff(rightValue); 
      delay(10);
      }
    }    
  }
  // ------ END HOME ASSIST ------

  
//  Serial.println("Swiatla ");
//  Serial.println(lightsVolt);
//  Serial.println("Silnik ");
//  Serial.println(engineVolt);
Serial.println(wheelVolt);
//Serial.print("right: ");
//Serial.println(rightValue);
//Serial.print("left: ");
//Serial.println(leftValue);
}

//bool engineOn(float eV)
//{
////  if(eV < engineVoltThresholdOff && !timestampStart && engineOn)
////  {
////     timestamp = millis();
////     timestampStart = true; 
////     return false;
////  }
////  if(eV < engineVoltThresholdOff && timestampStart && millis()-timestamp >= 4000 && millis()-timestamp<=4200)
////  {
////    timestampStart = false;
////    engineOn = false;
////    return true;  
////  }
////  else{
////    
////    return false;
////  }
//    if(engineOn)return true;
//    if(abs(eV - ignitionPrev) > 7){
//        ignitionPrev = .0;
//        engineOn = true;
//        return true;
//    }
//    else{
//        ignitionPrev = eV;
//        return false;   
//    }
//    
//}

bool keyLock(float key)
{
  if(abs(key - keyPrev) > 10){
    keyPrev = .0;
    return true;
  }
  else{
    keyPrev = key;
    return false;   
  }
}



void leftOn(int& p)
{
   if(p >= 255)
   {
    p = 255;
      leftAuxOnProcess = false;
      leftAuxOn = true;
      analogWrite(pinAuxLightLeft,p);      
      return; 
   }
   analogWrite(pinAuxLightLeft,p);
   //delay(2);
   p+=leftRightSpeed;   
}

void leftOff(int& p)
{
    
  if(p <= 0)
   {
      p = 0;
      leftAuxOffProcess = false;
      leftAuxOn = false;
      analogWrite(pinAuxLightLeft,p);
      return; 
   }
   analogWrite(pinAuxLightLeft,p);
   //delay(2);
   p-=leftRightSpeed;      
}

void rightOn(int& p)
{
  if(p >= 255)
   {
    p = 255;
      rightAuxOnProcess = false;
      rightAuxOn = true;
      analogWrite(pinAuxLightRight,p);
      return; 
   }
   analogWrite(pinAuxLightRight,p);
   //delay(2);
   p+=leftRightSpeed;   
}

void rightOff(int& p)
{
   if(p <= 0)
   {
    p = 0;
      rightAuxOffProcess = false;
      rightAuxOn = false;
      analogWrite(pinAuxLightRight,p);
      return; 
   }
   analogWrite(pinAuxLightRight,p);
   //delay(2);
   p-=leftRightSpeed;   
}

bool lightsOff(float eV, float lV)
{
  return (lV < beamLightsVoltThresholdOff);    
}

bool lightsOn(float eV, float lV)
{
  return (lV > beamLightsVoltThresholdOn);   
}

void buzz(bool lOn)
{
  lOn?singOn():singOff();  
}

void singOn()
{
    for(int i=0;i<100;i++)
    {
      digitalWrite(pinBuzzer,200);
      delay(3);  
    }
    digitalWrite(pinBuzzer,LOW);
    delay(40);
    for(int i=0;i<100;i++)
    {
      digitalWrite(pinBuzzer,200);
      delay(3);  
    }
    digitalWrite(pinBuzzer,LOW);
}

void singOff()
{
    for(int i=0;i<100;i++)
    {
      digitalWrite(pinBuzzer,200);
      delay(4);  
    }
    digitalWrite(pinBuzzer,LOW);
}

float readVolt(int pin)
{
  float x = analogRead(pin);
  x = (x*5.0) / 1024.0;
  x = x/(R2/(R1 + R2));
  return x;  
}

void fadeOut(const int pin, int powerMax, int pinDelay)
{
    for(int i=powerMax;i>=0;i-=5)
    {
      analogWrite(pin,i);
      delay(pinDelay);  
    }  
}

void fadeIn(const int pin, int powerMax, int pinDelay)
{
    for(int i=0;i<powerMax;i+=5)
    {
        analogWrite(pin,i);
        delay(pinDelay);
    }
}
