void TestDraaien()
{
  digitalWrite(LedRoodIO,1);
  VentilatorTesten();                       // Ventilator stroom testen
  VentilatorBeginSec=now();
  VentilatorStart(VentSpeedTest);
  Printing();
  Timer_2_Old=(TijdTestDraaien*1000)+millis();
  Timer_2_New=millis();
  if (Error==0)
  {
    while (Timer_2_New<Timer_2_Old)
    {
      Timer_2_New=millis();
      if(digitalRead(DrukknopIO)==1)   
      {
        digitalWrite(LedRoodIO,0);
        VentilatorStop();
        VentilatorEindSec=now();
        VentilatorTotaalSec=VentilatorTotaalSec+(VentilatorEindSec-VentilatorBeginSec);
        Systeem=0;
        Printing();
        Timer_2_Old=Timer_2_New+5000;               // voor uit de while lus te springen
      }
    }
  }
  digitalWrite(LedRoodIO,0);
  VentilatorStop();
  VentilatorEindSec=now();
  VentilatorTotaalSec=VentilatorTotaalSec+(VentilatorEindSec-VentilatorBeginSec);
  Systeem=0;
  Printing();
}
