// voorbeeld van printing en uitleg
/*
Chip ID = ......................... 2308976           <- ChipId ligt vast in de ESP32.
Software Versie = ................. 4                 <- Software Versie ligt vast in de soft.
Afstand Meting = .................. Geen detectie     <- Afstands meting  ofwel "Geen detectie"   "Iemand staat ervoor"   "Iemand zit neer"   "Deksel dicht".
Time Out = ........................ 5 Sec.            <- Time out (instelbaar).
Batterij = ........................ < 25%             <- Batterij waarde ofwel ">75%"   "<75%"   "<50%"   "<25%"   "Te laag" .
Aantal Batterij vervangen = ....... 46 keer.          <- Aantal keren dat de schakeling zonder spanning is geweest.
ReBoot door = ..................... tijd (5sec)       <- Ofwel rebooten door time-out ofwel door een spanning onderbreking.
Boot Counter = .................... 603               <- Aantal keren ge-reboot door de time-out.
Update Memory Afther Boots = ...... 8640              <- Na aantal reboots naar de EEPROM schrijven.
Deksel Open Dicht = ............... 0                 <- Aantal keren deksel open/dicht.
Aantal keren gezeten = ............ 1                 <- Aantal keren er iemand plaats heeft genomen op de wc.
Tijd laatste keer gezeten = ....... 6                 <- Tijd dat de laatste persoon neer gezeten heeft.
Totaal tijd neergezeten = ......... 6                 <- Totaal tijd dat er iemand op de wc gezeten heeft.
Gemiddelde tijd neer gezeten = .... 6                 <- Gemiddelde zit tijd.
Drukknop = ........................ niet gedrukt      <- Geef weer of er kort of lang is gedruk op de drukknop.
Systeem  = ........................ 0 Niet Actief     <- Geef weer wat de unit op dat moment aan het doen is.
Ventilator = ...................... 0 %               <- Geef weer hoe hard de ventilator draaid.
Geur sensor = ..................... Uit               <- Geur sensor aan of uit staat.
Gemiddelde Geur waarde = .......... 0                 <- Gemiddelde waarde wat de geur sensor heeft gemeten om te activeren.
Ventielator lage snelheid sec = ... 0                 <- Hoeveel sec de ventilator tot nu toe heeft gedraaid in lage snelheid
Ventielator hoge snelheid sec = ... 23                <- Hoeveel sec de ventilator tot nu toe heeft gedraaid in hoge snelheid
Ventielator actief sec = .......... 23                <- Hoeveel sec de ventilator tot nu toe heeft gedraaid.
Filter vervanging min = ........... 8100              <- Tijd dat de filter moet vervangen worden afhankelijk van de ventilator draai tijd
Filter reeds in weking min = ...... 0                 <- Tijd dat de filter al mee draaid
Error = ........................... 0                 <- Welke error er op dit moment aanwezig is.
*/

void Printing()
{
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");  
  Serial.print("Chip ID = ......................... ");
  Serial.println(chipId);
//====================================================================
  Serial.print("Software Versie = ................. ");
  Serial.println(SOFWAREVERSION);
//====================================================================
  Serial.print("Afstand Meting = .................. ");
  if (AfstandSensor==0)
  {
    Serial.println("Geen detectie");
  }
  if (AfstandSensor==1)
  {
    Serial.println("Iemand staat ervoor");
  }
  if (AfstandSensor==2)
  {
    Serial.println("Iemand zit neer");
  }
  if (AfstandSensor==3)
  {
    Serial.println("Deksel dicht");
  }
//====================================================================
  Serial.print("Time Out = ........................ ");
  Serial.print(SleepTime+2);
  Serial.println(" Sec.");  
//====================================================================
  Serial.print("Batterij = ........................ ");
  if (BatLevel == 4)
  {
    Serial.println(">= 75%");
  }
  if (BatLevel == 3)
  {
    Serial.println("< 75%");
  }
  if (BatLevel == 2)
  {
    Serial.println("< 50%");
  }
  if (BatLevel == 1)
  { 
    Serial.println("< 25%");
  }
  if (BatLevel == 0)
  { 
    Serial.println("Te Laag");
  }  
//==================================================================== 
  Serial.print("Aantal Batterij vervangen = ....... ");
  Serial.print(BatterijVervangingen);
  Serial.println(" keer."); 
//==================================================================== 
  Serial.print("ReBoot door = ..................... ");
  if (UitslaapCode==0)
  {
    Serial.println("eerste keer");    
  }
  if (UitslaapCode==2)
  {
    Serial.println("drukknop");    
  }
  if (UitslaapCode==4)
  {
    Serial.println("tijd (5sec)");    
  }    
  if (UitslaapCode==10)
  {
    Serial.println("geen reboot");    
  }    
//====================================================================  
  Serial.print("Boot Counter = .................... ");
  Serial.println(bootCount);
//====================================================================  
  Serial.print("Update Memory Afther Boots = ...... ");
  Serial.println(UpdateMemoryBootCounter);
//====================================================================
  Serial.print("Deksel Open Dicht = ............... ");
  Serial.println(RTCDekselOpenDichtTeller);
//====================================================================
  Serial.print("Aantal keren gezeten = ............ ");
  Serial.println(RTCNeergezeten);
//====================================================================
  Serial.print("Tijd laatste keer gezeten = ....... ");
  Serial.println(RTCZittijd);  
//====================================================================
  Serial.print("Totaal tijd neergezeten = ......... ");
  Serial.println(RTCTotaalZittijd); 
//====================================================================
  Serial.print("Gemiddelde tijd neer gezeten = .... ");
  Serial.println(RTCGemZittijd); 
//====================================================================
  Serial.print("Drukknop = ........................ ");
  if (Drukknop==0)
  {
    Serial.println("niet gedrukt");
   }
  if (Drukknop==1)
  {
    Serial.println("kort gedrukt");
  }
  if (Drukknop==2)
  {
    Serial.println("lang gedrukt");
  }
//====================================================================
  Serial.print("Systeem  = ........................ ");
  Serial.print(Systeem);
  
  if (Systeem==0)
  {
    Serial.println(" Niet Actief");
  }
  if (Systeem==1)
  {
    Serial.println(" Test Mode");
  }
  if (Systeem==2)
  {
    Serial.println(" Wifi Mode");
  }
  if (Systeem==3)
  {
    Serial.println(" Testen Ventilator");
  }
  if (Systeem==4)
  {
    Serial.println(" WC in gebruik");
   }
  if (Systeem==5)
  {
    Serial.println(" Geur gedetecteerd");
  }
  if (Systeem==6)
  {
    Serial.println(" Lange tijd geen geur");
  }
  if (Systeem==7)
  {
    Serial.println(" Cyclus onderbroken");
  }  
  if (Systeem==8)
  {
    Serial.println(" Cyclus onderbroken maar zit nog neer");
  }
  if (Systeem==10)
  {
    Serial.println(" Opgestaan nalooptijd");
  }
  if (Systeem==11)
  {
    Serial.println(" Deksel dicht nalooptijd");
  }  
  if (Systeem==12)
  {
    Serial.println(" Uit Error");
  }   
    if (Systeem==13)
  {
    Serial.println(" Geur sensor defect");
  }     
//====================================================================
  Serial.print("Ventilator = ...................... ");
  Serial.print(VentSpeed);
  Serial.println(" %");
//====================================================================
  Serial.print("Geur sensor = ..................... ");
  if (GeurToestand==0)
  {
    Serial.println("Uit"); 
  }
  if (GeurToestand==1)
  {
    Serial.print("Aan het opwarmen  (Waarde="); 
    Serial.print(GeurWaarde);
    Serial.println(")");
  }
  if (GeurToestand==2)
  {
    Serial.print("Aan het meten  (Waarde=");
    Serial.print(GeurWaarde);
    Serial.println(")");
  }
  if (GeurToestand==3)
  {
    Serial.print("Geur gedetecteerd  (Waarde=");
    Serial.print(GeurWaarde);
    Serial.println(")");    
  }
  if (GeurToestand==4)
  {
    Serial.println("Geur sensor werkt niet en toch blijven werken");
  }  
//====================================================================
  Serial.print("Gemiddelde Geur waarde = .......... "); 
  Serial.println(GeurGemiddeldeTriggerWaarde);     
//==================================================================== 
  Serial.print("Ventielator lage snelheid sec = ... ");
  Serial.println(VentilatorLaagTotaalSec); 
//==================================================================== 
  Serial.print("Ventielator hoge snelheid sec = ... ");
  Serial.println(VentilatorHoogTotaalSec); 
//==================================================================== 
  Serial.print("Ventielator actief sec = .......... ");
  VentilatorTotaalSec=(VentilatorHoogTotaalSec + VentilatorLaagTotaalSec);
  Serial.println(VentilatorTotaalSec);  
//==================================================================== 
  Serial.print("Filter vervanging min = ........... ");
  Serial.println(FilterVervangenOpMin);  
//==================================================================== 
  Serial.print("Filter reeds in werking min = ...... ");
  FilterVezadegingTijdTeller=(VentilatorLaagTusTotaalSec+VentilatorHoogTusTotaalSec);
  if (FilterVezadegingTijdTeller>FilterVervangenOpSec)
  {
    FilterIsVerzadigd=1;
    Error=6;
  }
  Serial.println(FilterVezadegingTijdTeller/60);
//==================================================================== 
  Serial.print("Filter aantal keren vervangen = ... ");
  Serial.println(FilterVervangingen);    
//====================================================================    
  Serial.print("Error = ........................... ");
  if (Error==0)
  {
    Serial.println("0");    
  }
  if (Error==1)
  {
    Serial.println("1 MAJOR (Geen filter aanwezig)");    
  }
  if (Error==2)
  {
    Serial.println("2 MAJOR (Batterij te laag)");    
  }
  if (Error==3)
  {
    Serial.println("3 MAJOR (Ventilator stroom verkeerd)");    
  }  
  if (Error==4)
  {
    Serial.println("4 MAJOR (Afstand sensor werkt niet)");    
  }    
  if (Error==5)
  {
    Serial.println("5 MINOR (Geur sensor werkt niet)");    
  }    
  if (Error==6)
  {
    Serial.println("6 MINOR (Filter moet vervangen worden)");    
  }      
//====================================================================  
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
