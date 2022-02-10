// De 3 lijnen sampeles op zo een hoog mogelijke freq
// L3 = gemeenschapelijk
// L1 = pos bat >= 75%              Led1=on    Led2=on   Led3=on   Led4=on   
// L1 = neg 50% <= bat <= 75%       Led1=on    Led2=on   Led3=on   Led4=off
// L2 = Pos 25% <= bat <= 50%       Led1=on    Led2=on   Led3=off  Led4=off
// L2 = neg  3% <= bat <= 25%       Led1=on    Led2=off  Led3=off  Led4=off
// L2 bink = 0% <= bat <= 3%        Led1=blink Led2=off  Led3=off  Led4=off    (blink time 0.55sec on - 0.55sec off)

void CheckBat()
{
    BatLevel = 0;
    LedT1=0;
    LedT2=0;
    LedT3=0;
    LedT4=0;  
    BatL1=0;
    BatL2=0;
    BatL3=0;
    Timer_8_Old=millis();
    Timer_8_New=millis();
    Timer_8_Div=Timer_8_New-Timer_8_Old;
    while(Timer_8_Div<50)                     // 50 is aantal millis seconden dat hij moet testen wie hoog of laag is
    {
        // currently not checking udp packets in this loop since it's onl 50 milli sec and this seems a bit of a time sensitive routine
        BatL1 = (10000+analogRead(BatL1IO));
        BatL2 = (10000+analogRead(BatL2IO));
        BatL3 = (10000+analogRead(BatL3IO));
        if (BatL1>(BatL3+DeltaBat)) LedT1=LedT1+1;    // D1 aan <25%
        if (BatL1<(BatL3-DeltaBat)) LedT2=LedT2+1;    // D2 aan <50%
        if (BatL2>(BatL3+DeltaBat)) LedT3=LedT3+1;    // D3 aan <75%
        if (BatL2<(BatL3-DeltaBat)) LedT4=LedT4+1;    // D4 aan >=75%
        Timer_8_New=millis();
        Timer_8_Div=(Timer_8_New-Timer_8_Old);  
    }

    if (LedT4>100) BatLevel = 4;
    else if (LedT3>100) BatLevel = 3;
    else if (LedT2>100) BatLevel = 2;
    else if (LedT1>100) BatLevel = 1;
    else BatLevel = 0;

    if (BatLevel == 0)
    { 
        //Error=2;      
        ErrorAfhandeling();
    } 
}
