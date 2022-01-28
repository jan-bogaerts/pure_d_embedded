void CheckAfstand()                       // AfstandSensor: 0=Geen detectie   1=Staat iemand  2=Zit iemand  3=Deksel dicht
{
  x=0;
  y=0;
  OldAfstandSensor=4;
  digitalWrite(AfstandI2COnOff,HIGH);
  lox.begin();
//  delay(50);  /// was 500
  ///////////////////// Afstands meting 20 keer of 10 keer onstabiel
  while ((x<20) and (y<10))
  {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    Afstand=measure.RangeMilliMeter;
    if (Afstand>600)
      {
        AfstandSensor=0;
        y=y+1;
      }
    if (Afstand<601)
      {AfstandSensor=1;}
    if (Afstand<300)
      {AfstandSensor=2;}
    if (Afstand<100)
      {AfstandSensor=3;}
    if (Afstand<10)
      {AfstandSensor=0;}

    if ((Afstand<100) and ((Afstand>(OldAfstand+5)) or (Afstand<(OldAfstand-5))))  // Het uitfilteren met lens kortbij te meten
    {
      x=0;
      OldAfstand=Afstand;
      y=y+1; 
    }
    if (OldAfstandSensor==AfstandSensor)
    {
      x=x+1;
    }
    if (OldAfstandSensor!=AfstandSensor)
    {
      x=0;
      OldAfstandSensor=AfstandSensor;
    }
  }
  if (y>8)
  {
    AfstandSensor=0; 
    OldAfstandSensor=0;
  }
  /////////////////////// Kijken of er iemand op de wc gaat zitten voor sound te geven
  if ((RTCAfstandSensor!=2) and (AfstandSensor==2))
  {
    SoundGespeeld=1;
    Sound();
  } 
  /////////////////////// Kijken of de vorige stand de deksel dicht was en dan +1
  if ((RTCAfstandSensor==3) and (AfstandSensor!=3))
  {
    RTCDekselOpenDichtTeller = RTCDekselOpenDichtTeller+1;
  } 
  /////////////////////// Kijken of de vorige stand iemand had neer gezeten en dan +1
  if ((RTCAfstandSensor==2) and (AfstandSensor!=2))
  {
    RTCNeergezeten = RTCNeergezeten+1;
  } 
  /////////////////////// Tijd starten als er iemand op zit
  if ((AfstandSensor==2) and (RTCAfstandSensor!=2))
  {
    if (Systeem==0)
    {
      Systeem=3;
    }
    RTCZittijdold=second();
    if ((FilterIsVerzadigd==1)and(FilterVerzadegingsFoutTeller<3))
      {
        Minor=6;
        MinorLedFlikkeren();
        FilterVerzadegingsFoutTeller=FilterVerzadegingsFoutTeller+1;
      }
  }
/////////////////////// Tijd stoppen als er niemand op zit en gemiddelde neerzit tijd berekenen
  if ((AfstandSensor!=2) and (RTCAfstandSensor==2))
  {
    RTCZittijdnew=second();
    RTCZittijd=RTCZittijdnew-RTCZittijdold;
    RTCTotaalZittijd=RTCTotaalZittijd+RTCZittijd;
    RTCGemZittijd=RTCTotaalZittijd/RTCNeergezeten;
  }
  
  RTCAfstandSensor = AfstandSensor; 
  digitalWrite(AfstandI2COnOff,LOW); 
  Printing();
}  

  
