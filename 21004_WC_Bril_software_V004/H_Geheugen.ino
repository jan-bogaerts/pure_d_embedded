/*********************************************************************************************************************/
/* GEHEUGEN indeling (per 4 byte's) 0=0-3 1=4-7 2=8-11. De 2 laatste zijn strings, max 256 bytes lang                */
/* 0 -  Chip ID                                                                                                      */
/* 1 -  Software versie                                                                                              */
/* 2 -  Time out                                                                                                     */
/* 3 -  Baterij vervangingen                                                                                         */
/* 4 -  Boot Counter                                                                                                 */
/* 5 -  Update Boot's                                                                                                */
/* 6 -  Deksel open/dicht                                                                                            */
/* 7 -  Aantal keren gezeten                                                                                         */
/* 8 -  Totaal Tijd gezeten                                                                                          */
/* 9 -  Aantal sec Ventilator actief                                                                                 */
/* 10-  Gemiddelde waarde Geursensor iets gemeten                                                                    */
/* 11-  Ventilator aantal seconden in laag toerental totaal                                                          */
/* 12-  Ventilator aantal seconden in hoog toerental totaal                                                          */
/* 13-  Ventilator aantal seconden in laag toerental                                                                 */
/* 14-  Ventilator aantal seconden in hoog toerental                                                                 */
/* 15-  Filter aantal seconden deze reeds aan het filteren is in seconden                                            */
/* 16-  Filter vervangingen                                                                                          */
/* 17-  ssid, byte 1 geeft lengte
/* 
/*********************************************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void EEPROMLeegmaken()
{
  for (x=0;x<30;x++)
  {
     EEPROMWritelong(x,0); 
  }
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println("*********************"); 
    Serial.println("*   EEPROM GEWIST   *"); 
    Serial.println("*********************"); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
//    delay(2000);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void EEPROMWritelong(int address, long EepromData)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  address=address*4;
  byte four = (EepromData & 0xFF);
  byte three = ((EepromData >> 8) & 0xFF);
  byte two = ((EepromData >> 16) & 0xFF);
  byte one = ((EepromData >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
  EEPROM.commit();
}

long EEPROMReadlong(unsigned long address)
{
  address=address*4;
  //Read the 4 bytes from the eeprom memory.
  unsigned long four = EEPROM.read(address);
  unsigned long three = EEPROM.read(address + 1);
  unsigned long two = EEPROM.read(address + 2);
  unsigned long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

/**
 * writes the string to the eeprom and returns how many bytes were
 * used
 */
byte EEPROMWriteString(int address, String str) {
    byte length = (byte)str.length();
    EEPROM.write(address, length);
    for(int i=0; i < length; i++) {
        EEPROM.write(address+i+1, str.charAt(i));
    }
    return length + 1;
}

String EEPROMReadString(int address) {
    byte length = EEPROM.read(address);
    String result;
    for(int i=0; i < length; i++) {
        char letter = EEPROM.read(address+1+i);
        results += String(char);
    }
    return result;
}

void EEPROMWriteList()
{
  EEPROMWritelong(0,chipId);
  EEPROMWritelong(1,SoftwareVersie);
  EEPROMWritelong(2,SleepTime);
  EEPROMWritelong(3,BatterijVervangingen);
  EEPROMWritelong(4,bootCount);
  EEPROMWritelong(5,UpdateMemoryBootCounter);
  EEPROMWritelong(6,RTCDekselOpenDichtTeller);
  EEPROMWritelong(7,RTCNeergezeten);
  EEPROMWritelong(8,RTCTotaalZittijd);
  EEPROMWritelong(9,VentilatorTotaalSec);
  EEPROMWritelong(10,GeurGemiddeldeTriggerWaarde);
  EEPROMWritelong(11,VentilatorLaagTotaalSec);
  EEPROMWritelong(12,VentilatorHoogTotaalSec);
  EEPROMWritelong(13,VentilatorLaagTusTotaalSec);
  EEPROMWritelong(14,VentilatorHoogTusTotaalSec);
  EEPROMWritelong(15,FilterVezadegingTijdTeller);
  EEPROMWritelong(16,FilterVervangingen);     
  EEPROM.commit();
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println("************************************"); 
    Serial.println("*   ALLES NAAR EEPROM GESCHREVEN   *"); 
    Serial.println("************************************"); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" ");   
}

void EEPROMReadList()
{
//  chipId=EEPROMReadlong(0);
//  SoftwareVersie=EEPROMReadlong(1);
//  SleepTime=EEPROMReadlong(2);
  BatterijVervangingen=EEPROMReadlong(3);
  bootCount=EEPROMReadlong(4);
//  UpdateMemoryBootCounter=EEPROMReadlong(5);
  RTCDekselOpenDichtTeller=EEPROMReadlong(6);
  RTCNeergezeten=EEPROMReadlong(7);
  RTCTotaalZittijd=EEPROMReadlong(8);
  VentilatorTotaalSec=EEPROMReadlong(9);
  GeurGemiddeldeTriggerWaarde=EEPROMReadlong(10);
  VentilatorLaagTotaalSec=EEPROMReadlong(11);
  VentilatorHoogTotaalSec=EEPROMReadlong(12); 
  VentilatorLaagTusTotaalSec=EEPROMReadlong(13);
  VentilatorHoogTusTotaalSec=EEPROMReadlong(14);
  FilterVezadegingTijdTeller=EEPROMReadlong(15);
  FilterVervangingen=EEPROMReadlong(16);
  
  EEPROM.commit();
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println("********************************"); 
    Serial.println("*   ALLES UIT EEPROM GEHAALD   *"); 
    Serial.println("********************************"); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
    Serial.println(" "); 
}


void EEPROMSaveWifiInfo() {
    int nextPos = EEPROMWriteString(17, home_ssid);
    EEPROMWriteString(nextPos, home_pwd);
}

void EEPROMReadWifiInfo() {
    String ssid = EEPROMReadString(17);
    String pwd = EEPROMReadString(17 + 1 + ssid.length());
    ssid.toCharArray(home_ssid, 128);
    pwd.toCharArray(home_pwd, 128);
}
