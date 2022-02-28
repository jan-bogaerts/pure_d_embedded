/*****************************************************************************************************************************************/
/* ERROR'S                                                                                                                               */
/* 0 -  Geen Error                                                                                                                       */
/* 1 -  Filter Error                 - Mag niet starten                                                                                  */
/* 2 -  Batterij te laag             - Moet stoppen en zolang hij kan de RLed 2x laten pinken                                            */
/* 3 -  Ventilator verkeerde stroom  - Mag niet starten, telkens proberen als er iemand plaatst neemt en de Rled 3x pinken               */
/* 4 -  Afstand sensor werkt niet    - Kan niet starten OPGELET en blijft om de 30 sec de Rled 4 x flikkeren en sound om de 10min        */
/* 5 -  Geur sensor werkt niet       - Mag starten maar ventielator zal niet meer traag gaan draaien en Rled 5x flikkeren                */
/* 6 -  Filter verzadigd             - Mag starten zal Rled 6x laten flikkeren                                                           */
/*****************************************************************************************************************************************/

void processFilterError() {
    Printing();
    Serial.println("");
    Serial.println("*****************************");
    Serial.println("*    FILTER NIET AANWEZIG   *");
    Serial.println("*****************************");
    if (isTryingToConnect == false && WifiStatus == 0)          // we have a serious error that needs to be reported. this can happen at setup (device starts up), at which point wifi isn't started yet, so do it now
        tryStartClientWifi();
    digitalWrite(GeurSensorIO, 0);                               // Geursensor uit zetten  
    VentilatorStop();
    digitalWrite(AudioOnOff, LOW);                               // Geluid uit zetten
    digitalWrite(AfstandI2COnOff, HIGH);                         // Afstand sensor aan zetten
    delayCheckUdp(500);
    if (WifiStatus == 2 && Error != PrevError) {             // if we connected and have not send the status update, do this now
        broadcastUdpState();
        PrevError = Error;                                  // keep track changes so we can send a new status update when done
    }   
    ErrorOldAfstandSensor = 0;
    OldAfstandSensor = 4;
    while (digitalRead(FilterIO)==1)                            // In deze lus blijven tot filter is terug geplaats, om de 30 sec witte led en als men opstaat geluid.
    {
        checkUdpIncomming();
        if (WifiStatus == 2 && Error != PrevError) {             // if we connected and have not send the status update, do this now
            broadcastUdpState();
            PrevError = Error;                                  // keep track changes so we can send a new status update when done
        }   
        ErrorLedFlikkeren();
        ErrorAfstansmeting();  
        if ((ErrorOldAfstandSensor!=2) and (AfstandSensor==2))   // Alleen geluid spelen als er iemand gaat op zitten
        {
            SoundGespeeld=1;
            Sound();
        } 
        ErrorOldAfstandSensor=AfstandSensor;
        delayCheckUdp(5000);
    }
    Error=0;
    FilterIsVerzadigd=0;                                        // filter is eruit geweest dus zal wel vervangen geweest zijn
    FilterVezadegingTijdTeller=0;                               // Tijd teller van de filter op 0 zetten
    VentilatorLaagTusTotaalSec=0;
    VentilatorHoogTusTotaalSec=0;
    broadcastUdpState();                                        // the state has changed
}

void processBatteryLow() {
    Printing();
    Serial.println(" ");
    Serial.println("**********************************");
    Serial.println("*    BATTERIJ TE LAGE SPANNING   *");
    Serial.println("**********************************");
    digitalWrite(AfstandI2COnOff,HIGH);                 // Afstands meting aan zetten
    digitalWrite(GeurSensorIO,0);                       // Geursensor uit zetten  
    VentilatorStop();
    digitalWrite(AudioOnOff,LOW);                       // Geluid uit zetten
    ErrorOldAfstandSensor=0;
    SoundGespeeld=1;
    Sound();
    while (1) {                                         // In deze lus blijven tot de batterij is vervangen
        for (y=0;y<20;y++) {
            for (x=0;x<Error;x++) {
                digitalWrite(LedRoodIO,1);
                delayCheckUdp(200);
                digitalWrite(LedRoodIO,0);
                delayCheckUdp(400);
            }
            delayCheckUdp(20000);  
        }
        SoundGespeeld=1;
        Sound();
    }   
}

void processVentBadPower() {
    Printing();
    Serial.println(" ");
    Serial.println("*************************************");
    Serial.println("*    VENTILATOR VERKEERDE STROOM    *");
    Serial.println("*************************************");
    digitalWrite(AfstandI2COnOff,HIGH);                                                     // Afstands meting aan zetten
    digitalWrite(GeurSensorIO,0);                                                           // Geursensor uit zetten  
    VentilatorStop();
    digitalWrite(AudioOnOff,LOW);                                                           // Geluid uit zetten
    ErrorOldAfstandSensor=0;
    while (Error==3) {                                                                      // In deze lus blijven tot de motor de juiste stroom terug meet
        ErrorLedFlikkeren();
        ErrorAfstansmeting();
        if ((ErrorOldAfstandSensor!=2) and (AfstandSensor==2)) {                            // Alleen ventilator starten als er iemand gaat op zitten
            digitalWrite(VentRelaisIO,HIGH);                                                // wordt pas na dac zetting terug uit gezet
            VentSpeed=100;
            Timer_3_Old_Div=0;
            Timer_3_Old=millis();
            Timer_3_New=millis();
            Timer_3_Div=Timer_3_New-Timer_3_Old;
            while(Timer_3_Div<1000) {                                                       // motor 1 sec via de relais op volle kracht opstarten
                checkUdpIncomming();                                                        // always process any possible udp packets while in the loop so that the device remains responsive to mobile commands
                Timer_3_New=millis();
                Timer_3_Div=(Timer_3_New-Timer_3_Old); 
                if (Timer_3_Old_Div!=Timer_3_Div) {
                    VentilatorStartStroom= VentilatorStartStroom+(analogRead(VentADCIO));
                    Timer_3_Old_Div=Timer_3_Div;
                }
            }
            VentilatorStartStroom=VentilatorStartStroom/1000; 
            digitalWrite(VentRelaisIO,LOW);
            if ((VentilatorStartStroom>900)or(VentilatorStartStroom<50)) {                  // indien de motor teveel stroom of te weinig stroom trekt, error 3 en printing  
                SoundGespeeld=1;
                Sound();
            }
            else {                                                                          // indien de stroom terug goed is verder gaan en error=0
                digitalWrite(VentRelaisIO,LOW);
                Error=0;
                AfstandSensor=0;
                OldAfstandSensor=4;
                Systeem=12;                         // terug springen zonder systeem 3,4....
                digitalWrite(LedRoodIO,0);          // Rode led uitzetten
                digitalWrite(AfstandI2COnOff,LOW);       // Afstands meting uit zetten  
            }
        }
        ErrorOldAfstandSensor=AfstandSensor;
        delayCheckUdp(5000);      
    }
}

void processBrokenDistanceSensor() {
    Printing();
    Serial.println(" ");
    Serial.println("***********************************");
    Serial.println("*    AFSTAND SENSOR WERKT NIET    *");
    Serial.println("***********************************");
    digitalWrite(AfstandI2COnOff,LOW);        // Afstands meting uit zetten
    digitalWrite(GeurSensorIO,0);       // Geursensor uit zetten  
    VentilatorStop();
    SoundGespeeld=1;
    Sound();
    x=0;
    digitalWrite(AudioOnOff,LOW);       // Geluid uit zetten
    while (Error==4)  {                         // In deze lus blijven tot de sensor is goeds meet
        delayCheckUdp(30000);
        ErrorLedFlikkeren();
        TestAfstandSensor(); 
        if (x<2) {                         // 2x geluid spelen 
            SoundGespeeld=1;
            Sound();
        } 
        if (x>10)                         // 8x geen geluid spelen
            x=0;  
        x=x+1;
    }
}

void processBrokenSmellSensor() {
    Printing();
    Serial.println(" ");
    Serial.println("********************************");
    Serial.println("*    GEUR SENSOR WERKT NIET    *");
    Serial.println("********************************");
    Serial.println(" ");
    ErrorLedFlikkeren();
    if (GeurErrorTeller<2) {
        SoundGespeeld=1;
        Sound();
    }
    GeurErrorTeller=GeurErrorTeller+1;
    if (GeurErrorTeller>9) GeurErrorTeller=0;
}

void processFilterSaturated() {
    Printing();
    Serial.println(" ");
    Serial.println("*****************************");
    Serial.println("*    FILTER IS VERZADIGD    *");
    Serial.println("*****************************");
    Serial.println(" ");
    ErrorLedFlikkeren();
}

void ErrorAfhandeling()
{
    if (Error != PrevError) {             // when the erorr value has changed, let mobiles know.
        broadcastUdpState();
        PrevError = Error;                // keep track changes
    }
    if (Error==1)                         // Zit geen filter in. Mag niet werken en telkens men op de wc gaat zitten audio geven
        processFilterError();
    if (Error==2)                         // De batterij heeft een te lage spanning en gaat om de 30 sec de rode led 2 keer flikkeren en sound om de 10min (20x30sec)
        processBatteryLow();
    if (Error==3)                         // De ventilator heeft een te hoge of te lage start stroom en mag terug proberen als er iemand komt op te zitten met geluid
        processVentBadPower();
    if (Error==4)                         // De afstands sensor werkt niet en gaat om de 30 sec de witte led 4 flikkeren en sound om de 10min (20x30sec)
        processBrokenDistanceSensor();
    if (Error==5)                         // De geur sensor werkt niet en gaat blijven de rode led 5 flikkeren en sound 2x om de 10keer
        processBrokenSmellSensor();
    if (FilterIsVerzadigd==1)             // De Filter is oververzadigd led 6 flikkeren en sound 3x sound geven maar blijven werken
        processFilterSaturated();

    if (Error != PrevError) {             // while processing the error, it could be that the error value has changed, keep track of this.
        broadcastUdpState();
        PrevError = Error;                // keep track changes
    }   
}  

void ErrorLedFlikkeren()
{
    for (x=0;x<Error;x++) {
        digitalWrite(LedRoodIO,1);
        delayCheckUdp(200);
        digitalWrite(LedRoodIO,0);
        delayCheckUdp(400);
    }
}  

void MinorLedFlikkeren()
{
    for (x=0;x<Minor;x++) {
        digitalWrite(LedRoodIO,1);
        delayCheckUdp(200);
        digitalWrite(LedRoodIO,0);
        delayCheckUdp(400);
    }
}  

void ErrorAfstansmeting()
{
    x=0;
    y=0; 
    digitalWrite(AfstandI2COnOff,HIGH);
    lox.begin(); 
    while ((x<20) and (y<10))
    {
        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);
        Afstand=measure.RangeMilliMeter;
        if (Afstand>600) {
            AfstandSensor=0;
            y=y+1;
        }
        if (Afstand < 601) AfstandSensor=1;
        if (Afstand < 300) AfstandSensor=2;
        if (Afstand < 100) AfstandSensor=3;
        if (Afstand < 10)  AfstandSensor=0;
        if (OldAfstandSensor == AfstandSensor) x=x+1;
        if (OldAfstandSensor != AfstandSensor) {
            x=0;
            OldAfstandSensor=AfstandSensor;
        }
    }
    if (y>8) AfstandSensor=0; 
}

void TestAfstandSensor()
{
    digitalWrite(AfstandI2COnOff,HIGH);
    delayCheckUdp(500);
    if (!lox.begin())                       // Start de I2C voor de afstands meting en indien deze niet lukt geef dan error=4
    {
        Error=4;
    }
    else
    {
        Error=0;
    //    Serial.print("AfstandSensor  = OK");
    //    delay(2000);     
    }
}
