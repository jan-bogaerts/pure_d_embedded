void VentilatorTesten()
{
//  Printing();
  digitalWrite(VentRelaisIO,HIGH);      // wordt pas na dac zetting terug uit gezet
  VentSpeed=100;                        // dit is maar ter info voor printing
  VentilatorBeginSec=now();
  Printing();
  Timer_3_Old_Div=0;
  Timer_3_Old=millis();
  Timer_3_New=millis();
  Timer_3_Div=Timer_3_New-Timer_3_Old;
  while(Timer_3_Div<1000)                   // motor 1 sec via de relais op volle kracht opstarten
  {
    if (Systeem==7)
    {
      digitalWrite(VentRelaisIO,LOW);  
      dacWrite(VentSpeedIO,0);
      VentilatorStop();
      Timer_3_Div=1001;
    }
    Timer_3_New=millis();
    Timer_3_Div=(Timer_3_New-Timer_3_Old); 
    if (Timer_3_Old_Div!=Timer_3_Div)
    {
     VentilatorStartStroom= VentilatorStartStroom+(analogRead(VentADCIO));
     Timer_3_Old_Div=Timer_3_Div;
    }
  }
  VentilatorStartStroom=VentilatorStartStroom/1000; 
  if ((VentilatorStartStroom>900)or(VentilatorStartStroom<50))  // indien de motor teveel stroom of te weinig stroom trekt, error 3 en printing  
  {
    Error=3;
    digitalWrite(VentRelaisIO,LOW);     // Relais uitzetten want er is te hoge stroom
    VentilatorStop();
    Printing();
  }
  digitalWrite(VentRelaisIO,LOW);         // Relais uitzetten want de stroom is gemeten
  VentilatorStop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VentilatorStart(int Speed)
{
//  if ((Speed==VentSpeedTraag) and (VentSpeed==VentSpeedNormaal))    // Als we overgaan van normaal naar traag
//  {
//    Timer_7_New=now();
//    VentilatorLaagTotaalSec=VentilatorLaagTotaalSec+( Timer_7_New- Timer_7_Old);
//    VentilatorLaagTusTotaalSec=VentilatorLaagTusTotaalSec+(Timer_7_New-Timer_7_Old);     
//  }
  VentSpeed=Speed;
  if (Speed==VentSpeedNormaal)
  {
    Timer_4_Old=now(); 
//Serial.println(Timer_4_Old);   
  }
  if (Speed==VentSpeedTraag)
  {
    Timer_7_Old=now();    
  }
  VentSpeed255=map(Speed,0,100,VentSpeedMin,VentSpeedMax);
  digitalWrite(VentRelaisIO,LOW);    
  dacWrite(VentSpeedIO,VentSpeed255);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VentilatorStop()
{
  if (VentSpeed==VentSpeedNormaal)
  {
    Timer_4_New=now();
    VentilatorHoogTotaalSec=VentilatorHoogTotaalSec+(Timer_4_New-Timer_4_Old);
    VentilatorHoogTusTotaalSec=VentilatorHoogTusTotaalSec+(Timer_4_New-Timer_4_Old);  
  }
  if (VentSpeed==VentSpeedTraag)
  {
    Timer_7_New=now();
    VentilatorLaagTotaalSec=VentilatorLaagTotaalSec+( Timer_7_New- Timer_7_Old);
    VentilatorLaagTusTotaalSec=VentilatorLaagTusTotaalSec+(Timer_7_New-Timer_7_Old);        
  }
  digitalWrite(VentRelaisIO,LOW);  
  dacWrite(VentSpeedIO,0);
}
