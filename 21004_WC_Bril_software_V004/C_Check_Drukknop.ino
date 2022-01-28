void CheckDrukknop()
{
  Drukknop=0;
  Timer_8_Old=millis();
  Timer_8_New=Timer_8_Old; 
  while(digitalRead(DrukknopIO)==1)
  {
     Timer_8_New=millis(); 
  }
  Timer_8_Div=Timer_8_New-Timer_8_Old;
  if (((Timer_8_Div>0) and (Timer_8_Div<2000)) or (UitslaapCode==2))
  {
    while(digitalRead(DrukknopIO)==0)
    {
      Drukknop=1; 
    }
  }
  if (Timer_8_Div>2000)
  {
    while(digitalRead(DrukknopIO)==0)
    {
      Drukknop=2; 
    }    
  }
}
