void Geur(){
    for (x=0;x<(GeurMeetTijd*100);x++)        // kijken of er niet op de drukknop is gedruk om te stoppen gedurende de wachtijd (X x de wachtijd in sec) voor de geur meeting
    {
        checkUdpIncomming();                  // always process any possible udp packets while in the loop so that the device remains responsive to mobile commands
        delay(5);
        if (Systeem==7) x=(GeurMeetTijd*100)+1;
    }
    GeurWaarde=analogRead(GeurSensorADC);
    GeurGemiddeldeWaarde=GeurGemiddeldeWaarde+GeurWaarde;
    GeurGemiddeldeWaarde=(GeurGemiddeldeWaarde/2);
    if ((GeurWaarde<900) and (Gekalibreerd==0) and (Systeem==4)) {
        Error=5;
        digitalWrite(GeurSensorIO,0);       // Geursensor uit zetten 
        GeurToestand=4;                     // voor het printen
        ErrorAfhandeling();
    }
    if (GeurGemiddeldeWaarde>900) {
        if (GeurWaarde < GeurGemiddeldeWaarde) {
            Gekalibreerd=1;
            GeurToestand=2;
            if (Error==5) Error=0;
        }
        if ((GeurWaarde > (GeurGemiddeldeWaarde+30)) and (Gekalibreerd==1)) {
            GeurToestand=3;
            if (GeurGemiddeldeTriggerWaarde==0)
                GeurGemiddeldeTriggerWaarde=GeurWaarde;
            else
                GeurGemiddeldeTriggerWaarde=((GeurGemiddeldeTriggerWaarde+GeurWaarde)/2);  
            GeurErrorTeller=0;
            if (Error==5) Error=0;
        }
        if (GeurGemiddeldeWaarde<900) Gekalibreerd=0;
    }
}