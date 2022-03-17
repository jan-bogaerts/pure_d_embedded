
  /*********************************************************************************************************************/
/*   WC BRIL 21004                                                                                                   */
/*   wifinaam: "PURE-D-{chipId}";                                                                                              */
/*   password: "123456789";                                                                                          */
/*   IP ADRES: 192.168.4.1                                                                                           */
/* > Versie : 001                                                                                                    */
/*   Date   : 25/09/2020                                                                                             */
/*   Aanpassingen:                                                                                                   */
/* > Versie : 002                                                                                                    */
/*   Date   : 27/09/2020                                                                                             */
/*   Aanpassingen:                                                                                                   */
/*    A_Check_Afstand: regel 28, afstands sensor met filter meet niet stabiel op korte afstand                       */
/* > Versie : 003                                                                                                    */
/*   Date   : 01/10/2020                                                                                             */
/*   Aanpassingen:                                                                                                   */
/*    F_Ventilator: regel 23, nu gaan we ook kijken of de ventilator niet draaid en geven dan ook een error.         */
/* > Versie : 004                                                                                                    */
/*   Date   : 18/10/2020                                                                                             */
/*   Aanpassingen:                                                                                                   */
/*    Main: Interupt bijgezet om de ventielator uit te zetten als een vrouw alleen maar wilt neer zitten             */
/*          Daarom systeem 7 en 8 bijgemaakt                                                                         */
/*    Afstandsensor Lox.(begin()) bij gezet om vlugger te kunnen de afstand te meten                                 */
/*    Geluid als de batterij wordt vervangen                                                                         */
/*    Rode led geeft nu de erorr's aan                                                                               */
/*    Print ventilator lage senlheid en ook hoge snelheid                                                            */
/*    Aantal filters vervangingen ook bij gevoegd                                                                    */
/*    Interup sneller laten reageren aks de unit in werking is gegaan                                                */
/*    Uit wifi door de drukknop                                                                                      */
/* >  Version: 005
      Date: 28/01/2022
      Author: Jan Bogaerts
      Changes: 
        - change a bunch of global variabes to define (no more mem)
        - add wifi ssid, wifi-pwd & sys-pwd to eeprom memory
        - add full support for wifi enroll (expand wifi server)
        - send udp packet upon wakeup & when certain values change
        - monitor incomming udp messages and perform actuations accordingly
        - code refactoring of for loops
        - general code refactoring to remove spagetti a bit
*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*   WIFI GEGEVENS                                                                                                   */
/*   ssid = PURE-D-{chipId}                                                                                                   */
/*   password = 123456789                                                                                            */
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*   Aansluiting ESP32                                                                                               */
/*    PEN        OMSCHRIJVING           AANSLUITING                                                                  */
/*      1 ...... 3V3  ....  Sys     .....  NC                                                                        */
/*      2 ...... EN   ....  Sys     .....  NC                                                                        */
/*      3 ...... IO36 ....  ADC0    .....  BatL1                                                                     */
/*      4 ...... IO39 ....  ADC3    .....  BatL2                                                                     */
/*      5 ...... IO34 ....  ADC6    .....  BatL3                                                                     */
/*      6 ...... IO35 ....  ADC7    .....  Sensor ADC                                                                */
/*      7 ...... I032 ....  Input   .....  Push Button                                                               */
/*      8 ...... IO33 ....  ADC5    .....  ADC vent stroom                                                           */
/*      9 ...... IO25 ....  I/O     .....  NC   (Spare)                                                              */
/*     10 ...... IO26 ....  DACout  .....  Vent Speed                                                                */
/*     11 ...... IO27 ....  Output  .....  Audio On/Off                                                              */
/*     12 ...... IO14 ....  Serial  .....  ESP-TX naar audio                                                         */
/*     13 ...... IO12 ....  Serial  .....  ESP-RX van audio                                                          */
/*     14 ...... GND  ....  Sys     .....  GND                                                                       */
/*     15 ...... IO13 ....  ADC14   .....  Vent On/Off relais                                                        */
/*     16 ...... SD2  ....  Sys     .....  NC                                                                        */
/*     17 ...... SD3  ....  Sys     .....  NC                                                                        */
/*     18 ...... CMD  ....  Sys     .....  NC                                                                        */
/*     19 ...... +5V  ....  Sys     .....  +5V IN                                                                    */
/*     20 ...... CLK  ....  Sys     .....  NC                                                                        */
/*     21 ...... SD0  ....  Sys     .....  NC                                                                        */
/*     22 ...... SD1  ....  Sys     .....  NC                                                                        */
/*     23 ...... IO15 ....  I/O     .....  NC   (spare)                                                              */
/*     24 ...... IO2  ....  I/O     .....  NC   (spare)                                                              */
/*     25 ...... IO0  ....  I/O     .....  NC                                                                        */
/*     26 ...... IO4  ....  Output  .....  Sensor On/Off                                                             */
/*     27 ...... IO16 ....  I/O     .....  NC   (spare)   HEARTBEAD                                                  */
/*     28 ...... IO17 ....  Output  .....  Led Red                                                                   */
/*     29 ...... IO5  ....  I/O     .....  NC                                                                        */
/*     30 ...... IO18 ....  Output  .....  Led White                                                                 */
/*     31 ...... IO19 ....  Input   .....  Filer                                                                     */
/*     32 ...... GND .....  Sys     .....  GND                                                                       */
/*     33 ...... IO21 ....  Serial  .....  I2C data Afstand meting                                                   */
/*     34 ...... RX  .....  Serial  .....  Voorbehouden RX                                                           */
/*     35 ...... TX  .....  Serial  .....  Coorbehouden TX                                                           */
/*     36 ...... IO22 ....  Serial  .....  I2C clock Afstand meting                                                  */
/*     37 ...... IO23 ....  Output  .....  SchutDown Afstand meting                                                  */
/*     38 ...... GND  ....  Sys     .....  GND                                                                       */
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/* Bedrading 26p                                                                                                     */
/* PEN    VAN                     NAAR                                                                               */
/*  1     GND ................... GND                                                                                */
/*  2     GND ................... GND                                                                                */
/*  3     +5V ................... +5V                                                                                */
/*  4     +5V ................... +5V                                                                                */
/*  5     bat <100% .............                                                                                    */
/*  6     bat <75%  .............                                                                                    */
/*  7     bat <50%  .............                                                                                    */
/*  8     bet <25%  .............                                                                                    */
/*  9     Filter ................                                                                                    */
/* 10     Geur sensor aan/uit ...                                                                                    */
/* 11     Geur sensor error   ...                                                                                    */
/* 12     Geur ADC ..............                                                                                    */
/* 13     I2C data ..............                                                                                    */
/* 14     I2C clock .............                                                                                    */
/* 15     I2C SchutDown .........                                                                                    */
/* 16     Drukknop ..............                                                                                    */
/* 17     Led Wit ...............                                                                                    */
/* 18     Led Rood ..............                                                                                    */
/* 19     3V3 ................... 3V3                                                                                */
/* 20     spare .................                                                                                    */
/* 21     spare .................                                                                                    */
/* 22     spare .................                                                                                    */
/* 23     +5V ................... +5V                                                                                */
/* 24     +5V ................... +5V                                                                                */
/* 25     GND ................... GND                                                                                */
/* 26     GND ................... GND                                                                                */
/*********************************************************************************************************************/

/*****************************************************************************************************************************************/
/* ERROR'S                                                                                                                               */
/* 0 -  Geen Error                                                                                                                       */
/* 1 -  Filter Error                 - Mag niet starten                                                                                  */
/* 2 -  Batterij te laag             - Moet stoppen en zolang hij kan de RLed 2x laten pinken                                            */
/* 3 -  Ventilator verkeerde stroom  - Mag niet starten, telkens proberen als er iemand plaatst neemt en de Rled 3x pinken               */
/* 4 -  Afstand sensor werkt niet    - Kan niet starten OPGELET en blijft om de 30 sec de Rled 4 x flikkeren en sound om de 10min        */
/* 5 -  Geur sensor werkt niet       - Mag starten maar ventielator zal niet meer traag gaan draaien en Rled 5x flikkeren                */
/* 6 -  Filter verzadigd             - Mag starten zal Rled 6x laten flikkeren 
*/
/*****************************************************************************************************************************************/

/*********************************************************************************************************************/
/* SYSTEEM stappen                                                                                                   */
/* 0 -  Geen detectie en gaat x tijd slapen                                                                          */
/* 1 -  Aangezet met drukknop (kort) voor test te draaien                                                            */
/* 2 -  Aangezet met drukknop (lang) voor naar wifi te gaan                                                          */
/* 3 -  Er zit iemand op de wc en gaan niet meer in slaap                                                            */
/* 4 -  Geur meten, tijd starten en afstand meten                                                                    */
/* 5 -  Stank gemeten                                                                                                */
/* 6 -  Al 4 min geen stank gematen                                                                                  */
/* 7 -  Indien men op de drukknop drukt als het systeem reeds aan het werk is                                        */
/* 8 -  Kijken of men opstaat om terug naar systeem 0 kunnen gaan                                                    */ 
/* 10-  Opgestaan en niks gemeten                                                                                    */
/* 11-  Opgegtaan en deksel dicht gedaan                                                                             */
/* 13-  Geursensor defect maar toch blijven werken                                                                   */
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/* GEHEUGEN indeling (per 4 byte's) 0=0-3 1=4-7 2=8-11                                                               */
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
/*********************************************************************************************************************/

#define TIME_TO_SLEEP  SleepTime                          // SleepTime is de tijd dat de ESP32 in slaap gaat 
#include "SoftwareSerial.h"                               // Libary voor een bijkomende serieele poort voor de audio
#include <DFRobotDFPlayerMini.h>                          // Libary voor de audio module
#include <TimeLib.h>                                      // Libary voor de timers aan te spreken
#include <EEPROM.h>                                       // Libary voor gegevens naar en van EEPROM vast te krijgen
#include "Adafruit_VL53L0X.h"                             // Libary voor de afstands sensor
#include <jsonlib.h>                                      // for parsing commands that come from the mobile
#include <WiFi.h>                                         // Libary voor de wifi
#include <WiFiUdp.h>                                      // udp communication
#define EEPROM_SIZE 459                                   // Voorbehouden EEPROM plaatsen (max of 3 strings is long)
Adafruit_VL53L0X lox = Adafruit_VL53L0X();                // Type afstands sensor


/* INSTELBARE PARAMETERS */  
#define SOFWAREVERSION 5                                  // Dit is de sofware versie; fix -> change to define so that it no longer uses any memory
int SleepTime = 5;                                        // Gaat om de 5 sec wakker worden en afstand meten
int UpdateMemoryBootCounter=8640;                         // om de 8640x5sec = 43200sec = 720min = 12 uur naar EEPROM schrijven
int TijdTestDraaien=20;                                   // Tijd dat de ventilator test draaid als men kort op de druknop drukt
int VentSpeedTest=30;                                     // Snelheid dat de ventilator moet draaien bij een test in% (30%-100%)
RTC_DATA_ATTR int VentSpeedNormaal = 50;                  // De waarde om de vent normaal te laten draaien in% (50)
RTC_DATA_ATTR int VentSpeedTraag = 30;                    // De waarde om de vent traag te laten draaien in% (30)
int VentTijdNaarTraag=240;                                // De tijd dat de ventilator over gaat van normaal snelheid naar trage snelheid in sec (240=4min)
RTC_DATA_ATTR int VentNaloopTijdOpstaan=30;               // De tijd dat de ventilator nog na draaid als men opstaat in sec
int VentNaloopTijdDekeslDicht=10;                         // De tijd dat de ventilator nog na draaid als de deksel dicht gaat in sec
RTC_DATA_ATTR int Volume=30;                              // Het volume van de audio (0=min 30=max)
RTC_DATA_ATTR int GeurMeetTijd=4;                         // Tijd dat we tussen twee geur metingen gaan doen in sec. Niet onder de 4 sec gaan
RTC_DATA_ATTR int FilterVervangenOpMin=8100;              // Tijd dat de filter kan mee gaan. omgerekend naar sec = 486000
// ----- SYSTEEM -----
unsigned long chipId = 0;                                 // chipid wat in de ESP32 steekt, gaan we uitlezen.


// ----- BOOTEN -----
RTC_DATA_ATTR int bootCount = 0;                          // In RAM zetten we de aantal keren dat de ESP uit slaap komt
RTC_DATA_ATTR int UitslaapCode=0;                         // code=0 1ste keer uit boot   code=2 met drukknop   code=4 met timer   code=10 Geen reboot

// ----- BATERIJ -----
int  LedT1=0;                                             // Hulp variabele voor de baterijen ledjes uit te lezen
int  LedT2=0;                                             // Hulp variabele voor de baterijen ledjes uit te lezen
int  LedT3=0;                                             // Hulp variabele voor de baterijen ledjes uit te lezen
int  LedT4=0;                                             // Hulp variabele voor de baterijen ledjes uit te lezen
int BatL1IO = 36;                                         // De IO lijn voor lijn1 van de ledjes te dedecteren
int BatL2IO = 39;                                         // De IO lijn voor lijn1 van de ledjes te dedecteren
int BatL3IO = 34;                                         // De IO lijn voor lijn1 van de ledjes te dedecteren
unsigned long BatL1=0;                                    // Hulp variabele voor de baterijen ledjes uit te lezen
unsigned long BatL2=0;                                    // Hulp variabele voor de baterijen ledjes uit te lezen
unsigned long BatL3=0;                                    // Hulp variabele voor de baterijen ledjes uit te lezen
//RTC_DATA_ATTR int Led1=0;                                 // Variabele om de waarde van de ledjes te vergelijken
//RTC_DATA_ATTR int Led2=0;                                 // Variabele om de waarde van de ledjes te vergelijken
//RTC_DATA_ATTR int Led3=0;                                 // Variabele om de waarde van de ledjes te vergelijken
//RTC_DATA_ATTR int Led4=0;                                 // Variabele om de waarde van de ledjes te vergelijken
RTC_DATA_ATTR int BatLevel = 0;                           // battery level 0-4 (0, 25%, 50%, 75%, 100%)
#define DeltaBat 1000                                     // Het waarde verschil tussen een lijn die hoog is of laag is
RTC_DATA_ATTR int BatterijVervangingen=0;                 // Het aantal keren dat de unit opnieuw onder spanning komt. Is te reseteen door eerst de drukknop in te houden en dan batterij insteken.
int HeartBeadIO=16;                                       // De uitgang naar de batterij om de batterypack aan te houden

// ----- AFSTAND -----
int AfstandI2COnOff = 23;                                 // pen voor I2C enable
int Afstand=0;                                            // Variabele die de afstand terug geeft van de afstand sensor
int OldAfstand=0;                                         // Hulp variabele om kunnen te kijken wat de vorige afstand was
int AfstandSensor=0;                                      // Toestand v/d afstand sensor 0=geen detectie(>600)or(<10)  1= Staat iemand ervoor(<600)   2= Zit iemand erop(<300)  3= Deksel dicht(<100)
int OldAfstandSensor=0;                                   // Toestand v/d vorige afstand sensor 0=geen detectie(>600)or(<10)  1= Staat iemand ervoor(<600)   2= Zit iemand erop(<300)  3= Deksel dicht(<100)
int ErrorOldAfstandSensor=0;                              // Toestand v/d vorige afstand sensor in de Error afhandelingen
RTC_DATA_ATTR int RTCAfstandSensor = 0;                   // Toestand v/d afstand sensor in RAM brengen

// ----- TIJD -----
unsigned long Timer_1_Old;                                // Timer gegevens gebruikt in de Main routine voor de drukknop tijd te meten
unsigned long Timer_1_New;                                // Timer gegevens gebruikt in de Main routine voor de drukknop tijd te meten
unsigned long Timer_1_Div;                                // Timer gegevens gebruikt in de Main routine voor de drukknop tijd te meten
unsigned long Timer_2_Old;                                // Timer gegevens gebruikt in de E_Test_Draaien routine
unsigned long Timer_2_New;                                // Timer gegevens gebruikt in de E_Test_Draaien routine
unsigned long Timer_2_Div;                                // Timer gegevens gebruikt in de E_Test_Draaien routine
unsigned long Timer_3_Old;                                // Timer gegevens gebruikt in de F_Ventilator routine
unsigned long Timer_3_New;                                // Timer gegevens gebruikt in de F_Ventilator routine
unsigned long Timer_3_Div;                                // Timer gegevens gebruikt in de F_Ventilator routine
unsigned long Timer_3_Old_Div;                            // Timer gegevens gebruikt in de F_Ventilator routine
unsigned long Timer_4_Old;                                // Timer gegevens gebruikt voor de lage snelheid van de ventilator
unsigned long Timer_4_New;                                // Timer gegevens gebruikt voor de lage snelheid van de ventilator
unsigned long Timer_5_Old;                                // Timer gegevens gebruikt in de Main routine voor de ventilator naloop tijd bij het opstaan en ook deksel dicht
unsigned long Timer_5_New;                                // Timer gegevens gebruikt in de Main routine voor de ventilator naloop tijd bij het opstaan en ook deksel dicht
unsigned long Timer_6_Old;                                // Timer gegevens gebruikt in de Main routine voor de tijd om over te schakelen naar trage snelheid
unsigned long Timer_6_New;                                // Timer gegevens gebruikt in de Main routine voor de tijd om over te schakelen naar trage snelheid
unsigned long Timer_7_Old;                                // Timer gegevens gebruikt voor de hoge snelheid van de ventilator
unsigned long Timer_7_New;                                // Timer gegevens gebruikt voor de hoge snelheid van de ventilator
unsigned long Timer_8_Old;                                // Timer gegevens gebruikt in de B_Check_Bat routine
unsigned long Timer_8_New;                                // Timer gegevens gebruikt in de B_Check_Bat routine
unsigned long Timer_8_Div;                                // Timer gegevens gebruikt in de B_Check_Bat routine

// ----- DEKSEL -----
RTC_DATA_ATTR int RTCDekselOpenDichtTeller = 0;           // Deksel open/dicht teller in RAM
long EepromDekselOpenDicht;                               // Deksel open/dicht in EEPROM

// Neergezeten
RTC_DATA_ATTR int RTCNeergezeten = 0;                     // Aantal keren het systeem actief geweest is door neer te zitten in RAM
long EepromNeergezeten;                                   // Aantal keren het systeem actief geweest is door neer te zitten in EEPROM
int EvenOpgestaan=0;                                      // Hulp variabele voor te kijken of er iemand even heeft opgestaan

// ----- ZIT TIJD -----
RTC_DATA_ATTR long RTCZittijdold = 0;                     // Hulp variabele om de zit tijd te meten in ram
RTC_DATA_ATTR long RTCZittijdnew = 0;                     // Hulp variabele om de zit tijd te meten in ram
RTC_DATA_ATTR long RTCZittijd = 0;                        // Geef de zit tijd van de laatste activatie
RTC_DATA_ATTR long RTCTotaalZittijd = 0;                  // Geef de zit tijd totaal
RTC_DATA_ATTR long RTCGemZittijd = 0;                     // Geef de zit tijd gemiddelde

// ----- DRUKKNOP -----
int DrukknopIO=32;                                        // Drukknop ingang ligt op IO32
int Drukknop = 0;                                         // Drukknop Uit Sleep 0=niet gedrukt  1=kort gedrukt 2=lang gedrukt
//long DrukknopTeller=0;

// ----- LEDJES -----
int LedWitIO=17;                                          // Witte led uitgang ligt op IO17
int LedRoodIO=18;                                         // Rode led uitgang ligt op IO18

// ----- VENTILATOR -----
#define VentSpeedMin 30                                   // De absolute waarde die we naar de DAC mogen sturen voordat de ventilator stil valt
#define VentSpeedMax 220                                  // De absolute waarde die we naar de DAC mogen sturen, hoger doet niks meer uit
#define VentSpeedIO  26                                   // IO26 DAC uitgang om de vent te sturen
#define VentRelaisIO 13                                   // IO13 uitgang van de relais om de vent op max te sturen
#define VentADCIO    33                                   // IO33 ingang ADC om de startstroom te meten
int VentSpeed = 0;                                        // 0-100% 0=VentspeedMin 100%=VentspeedMax
bool VentOn = false;
// int Speed=0;                                              // Was vroeger ventspeed
int VentSpeed255 = 0;                                     // De waarde gemap om door te geven 
unsigned long VentilatorStartStroom;                      // Waarde Startstroom in de F_Ventilator routine
unsigned long VentilatorBeginSec = 0;                     // Hulp variabele voor de timer om de ventilator activatie te meten
unsigned long VentilatorEindSec = 0;                      // Hulp variabele voor de timer om de ventilator activatie te meten
RTC_DATA_ATTR unsigned long VentilatorTotaalSec = 0;      // Tijd dat de ventilator reeds gedraaid heeft in RAM
RTC_DATA_ATTR unsigned long VentilatorLaagTotaalSec = 0;  // Tijd dat de ventilator reeds gedraaid heeft in RAM
RTC_DATA_ATTR unsigned long VentilatorHoogTotaalSec = 0;  // Tijd dat de ventilator reeds gedraaid heeft in RAM
RTC_DATA_ATTR unsigned long VentilatorLaagTusTotaalSec = 0;// Tijd dat de ventielator reeds gedraaid heeft in lage stand na filter te vervangen
RTC_DATA_ATTR unsigned long VentilatorHoogTusTotaalSec = 0;// Tijd dat de ventielator reeds gedraaid heeft in hoge stand na filter te vervangen

// ----- GEUR SENSOR -----
int GeurSensorAanUit=0;                                   // Hulp variabele om te weten of de geur sensor aan of uit is
int GeurSensorIO=4;                                       // Geur sensor aan of uit zetten via IO4
int GeurSensorADC=35;                                     // De ADC ingang van de geur sensor ligt op 35
int GeurGemiddeldeWaarde=0;                               // Hulp variabele die de gemiddelde geur waarde geeft
RTC_DATA_ATTR int GeurGemiddeldeTriggerWaarde=0;          // De gemiddelde Trigger waarde wordt uitgeprint
int GeurWaarde=0;                                         // De geur waarde die juist gemeten wordt
int GeurWaardeVoor=0;                                     // De geur waarde die de meeting ervoor was
int Gekalibreerd=0;                                       // Geeft aan of de sensor warm genoeg is
int GeurToestand=0;                                       // 0= sensor uit 1= aan het opwarmen 2= aan het meten 3= iets gemeten 4= sensor werkt niet
int GeurErrorTeller=0;                                    // Teller voor de sound kunnen te tellen met een geur error

// ----- FILTER -----
#define FilterIO 19                                       // IO19 is de ingang om te zien of de filter aanwezig is
RTC_DATA_ATTR unsigned long FilterVezadegingTijdTeller=0; // telt is seconden maar printing in min
RTC_DATA_ATTR int FilterIsVerzadigd=0;                    // 0= niet verzadigd 1= verzadigd
RTC_DATA_ATTR int FilterVerzadegingsFoutTeller=0;         // Teller voor aantal keren de Rled te laten flikkeren als er iemand opzit
RTC_DATA_ATTR unsigned long FilterVervangenOpSec=0;       // Ingaven is in minuten maar hier zetten we dit om naar seconden (8100min = 486000sec)
RTC_DATA_ATTR unsigned long FilterVervangingen=0;         // Aantal filter vervangingen en wordt ook naar EEPROM geschreven
RTC_DATA_ATTR unsigned long FilterVervangingenOld=0;      // indien we een filter fout zien gaat deze variabele op 1 en gaan we de teller telkens +1 doen als er iemand opzit
RTC_DATA_ATTR unsigned long FilterVervangingenTeller=0;   // indien we een filter fout hebben gehad en de teller 5 is gaan we pas zeggen dat de filter vervangen +1 is
// ----- SYSTEEM ----- 
RTC_DATA_ATTR int Systeem=0;                              // 0=meting om de xsec  1=aangezet met drukknop  2=aangezet met afstand 3=Ventilator testen 4=4min voorbij 5=geur gemeten 5=nadraaien 
RTC_DATA_ATTR int WifiMode=0;                             // 1= in wifi mode
RTC_DATA_ATTR int TestMode=0;                             // 1= in 20seconden test mode
int InterupKnopGedrukt=0;                                 // Als er in een interupt op de drukknop is gedrukt is deze een 1
// ----- HULP -----
int x=0;                                                  // Voor hulp 
int y=0;                                                  // Voor hulp
//unsigned long SecMin=60;
// ----- ERRORS ----- 
RTC_DATA_ATTR int Error = 0;                              // 0=Geen error  1=Filter error   2=Batterij te laag  3=Stroom ventilator 4=Geen afstand sensor 5=Geen geur sensor
RTC_DATA_ATTR int PrevError = 0;                          // keep track of the previous error, so we can see when the error value has changed (and send a message to mobiles)
RTC_DATA_ATTR int Minor = 0;                              // 0=Geen Minor  6=Filter verzadigd

// ----- AUDIO -----
int AudioOnOff=27;                                        // IO27 is de uitgang om de audio aan/uit te zetten
SoftwareSerial SerialMP3(12, 14);                         // RX, TX voor de audio
DFRobotDFPlayerMini Player;                               // We gaan de audio module aanspreken met "Player"
RTC_DATA_ATTR int SoundGespeeld=0;                        // 0= mag geen audio afspelen 1= moet audio afspelen

// ----- WIFI -----
const char* ssid     = "PURE-D";                          // name of the ap mode  wifi network
const char* password = "10071984";                        // Paswoord of the ap mode wifi network
String output17State = "off";                             // Wifi hulp
String output18State = "off";                             // Wifi hulp
const int output17 = 17;                                  // Wifi hulp
const int output18 = 18;                                  // Wifi hulp
WiFiServer server(80);                                    // Wifi hulp
WiFiUDP udp;                                              // for processing udp packets
#define UDP_PORT 45031                                    // the port to use for udp messages  
uint8_t udpBuffer[300];                                   // buffer for incomming packets from the mobile devices
unsigned short WifiStatus = 0;                            // 0=not connected, 1=ap mode, 2=connected to wifi
bool isTryingToConnect = false;                           // true when not yet connected, but still trying to connect (has credentials, but WiFi.status() doesn't yet return connected). This allows us to connect async
unsigned long startedTryingConnectionAt = 0;              // time when we tried to start the wifi, so we don't try for ever

RTC_DATA_ATTR char home_ssid[128];                        // wifi network to connect to after initial ap mode
RTC_DATA_ATTR char home_pwd[128];                         // pwd of wifi network to connect to after initial ap mode
RTC_DATA_ATTR char system_pwd[128] = "PURE-D";            // pwd assigned by the user to the device, for verifying additional new connections
#define EEPROM_HOME_SSID_POS 76                           // position of home-ssid in eeprom
RTC_NOINIT_ATTR int REBOOT_AFTER_ENROLL = 0;              // set to 1 when the system just did a restart after configuring wifi.

// ----- SLEEP -----
 esp_sleep_wakeup_cause_t wakeup_reason;                  // Hoe de ESP32 uit slaap komt 0= eerste keer 2= drukknop 4=na sleep time(5sec)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setupPins() {
    pinMode(FilterIO, INPUT_PULLUP);                        // IO19 pen-31
    pinMode(DrukknopIO, INPUT);                             // IO32 pen-7
    pinMode(GeurSensorIO, OUTPUT);                          // IO4  pen-26
    digitalWrite(GeurSensorIO,LOW);                         // Zet de Geursensor uit
    pinMode(AfstandI2COnOff, OUTPUT);                       // IO23 pen-37
    pinMode(LedRoodIO, OUTPUT);                             // IO17 pen-28
    digitalWrite(LedRoodIO,LOW);                            // Zet de rode led uit
    pinMode(LedWitIO, OUTPUT);                              // IO18 pen-30
    digitalWrite(LedWitIO,LOW);                             // Zet de witte led uit
    pinMode(VentRelaisIO, OUTPUT);                          // IO33 pen-8
    digitalWrite(VentRelaisIO,LOW);                         // Zet de ventilator relais uit
    pinMode(AudioOnOff, OUTPUT);                            // IO27 pen-11
    digitalWrite(AudioOnOff,LOW);                           // Zet de audio module uit
    digitalWrite(AfstandI2COnOff,HIGH);                     // Zet de afstand sensor aan om onmiddelijk te kunnen testen of deze gaat
    pinMode(HeartBeadIO, OUTPUT);                           // IO16 pen-27 heartbead lijn naar de batterij
    digitalWrite(HeartBeadIO, LOW);                         // Geef een laag naar de batterij
    delay(100);                                             // Wacht 0.1sec
    digitalWrite(HeartBeadIO, HIGH);                        // Geef een hoog naar de batterij
}

void setup(){
    SleepTime=SleepTime-2;                                  // aanpassing want loop duurd 2 sec dus SleepTime - 3sec
    FilterVervangenOpSec=(FilterVervangenOpMin*60); 
    setupPins();    

    EEPROM.begin(EEPROM_SIZE);                              // Reserveer de x plaatsen in de EEPROM

    EEPROMReadWifiInfo();
    //for debug, now
    //EEPROMLeegmaken();
    //till here
    if (REBOOT_AFTER_ENROLL == 1) {                         // rebooted after enrollement/server config, try to connect to wifi asap so device knows we are connected to network
        tryStartClientWifi();
    }

    dacWrite(VentSpeedIO,VentSpeed);                        // VentSpeed = 0 bij opstarten
    for(int i=0; i<17; i=i+8)                               // Chip ID uit de ESP32 halen
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;

    Serial.begin(115200);                                   // Start de seriele verbinding naar de monitor op 115200 baut
    SerialMP3.begin(9600);                                  // Start de seriele verbindeng naar de audio module op 9600 baut
    Player.begin(SerialMP3);                                // Geef de audio seriele verbinding de naar Player

    if (!lox.begin())                                       // Start de I2C voor de afstands meting en indien deze niet lukt geef dan error=4
    {
        Error=4;
        Serial.println(F("Failed to boot VL53L0X"));
        ErrorAfhandeling();
    }

    
    bootCount = bootCount+1;                                // boot teller + 1 (in RTC RAM)
    if (bootCount==UpdateMemoryBootCounter)                 // Indien de bootCount de ingestelde boootcounter heeft gehaald moet alles naar eeprom geschreven worden
    {
        bootCount=0;
        EEPROMWriteList(); 
    }
}


void checkGoToSleep() {
    if ((Systeem==0) or (Systeem==12)) {
        //  Printing();
        if (Systeem==12) {
            Systeem=0;
        }
        sendUdpToSleep();                                   // let the mobiles know we are going to sleep, also shuts down udp
        detachInterrupt(32);
        delay(10);
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_32,1);
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000000);
        digitalWrite(AfstandI2COnOff,LOW);              // zet de afstands meting uit voor stroom te besparen
        esp_deep_sleep_start();      
    }
}

void checkSmellSensorDefect() {
    if (Systeem==13) {                                       // Geursensor defect maar toch blijven draaien tot ze opstaan
        digitalWrite(GeurSensorIO,0);                       // Geursensor uit zetten 
        GeurToestand=4;
        VentilatorStop(); 
        VentilatorStart(VentSpeedNormaal);
        CheckAfstand();
        broadcastUdpState();                                // ventilator state has changed, let any
        Printing(); 
        while (AfstandSensor==2) {
            digitalWrite(AfstandI2COnOff,HIGH);
            delayCheckUdp(500);        
            lox.begin();
            CheckAfstand();
            delayCheckUdp(4500); 
        }
        Systeem=10;
        Printing();
    }
}

void checkLitClosed() {
    if (Systeem==11) {                                       // Opgestaan en deksel is dicht (VentNaloopTijdDekeslDicht na draaien)
        Timer_5_Old = (millis()+(VentNaloopTijdDekeslDicht*1000));
        while (Timer_5_New<Timer_5_Old) {
            checkUdpIncomming();                              // always process any possible udp packets while in the loop so that the device remains responsive to mobile commands
            Timer_5_New=millis();
        }
        VentilatorStop();
        Systeem=0;
    } 
}

void checkUserGotUp() {
    if (Systeem==10) {                                       // Opgestaan en VentNaloopTijdOpstaan starten en kijken of deksel dicht gaat
        digitalWrite(GeurSensorIO,0);                         // Geursensor uit zetten
        GeurToestand=0;
        Printing();
        Timer_5_New=millis();     
        Timer_5_Old = (millis()+(VentNaloopTijdOpstaan*1000));
        while ((Timer_5_New<Timer_5_Old) and (AfstandSensor!=3)) {
            Timer_5_New=millis();
            digitalWrite(AfstandI2COnOff,HIGH);
            delayCheckUdp(500);        
            lox.begin();
            CheckAfstand();
        }
        if (AfstandSensor==3) {                               // Kijken of de deksel dicht gaat
            Systeem=11; 
            Printing(); 
        }
        else {                                                // Deksel niet dicht, gewoon opgestaan en VentNaloopTijdOpstaan is voorbij
            VentilatorStop();
            Systeem=0;    
        }
    }
}

void checkCycleInteruptedWhileSitting() {
    if (Systeem==8) {
        while (AfstandSensor==2) {
            digitalWrite(AfstandI2COnOff,HIGH);
            delayCheckUdp(4500);        
            lox.begin();
            CheckAfstand();
            delayCheckUdp(500);
            Printing(); 
        } 
        Systeem=0;
        Printing(); 
    }
}

void checkInteruptBtnPressed() {
    if (Systeem==7)                                           // Komen hier via de interup als de drukknop gedrukt is
    {
        VentilatorStop();
        digitalWrite(GeurSensorIO,0);                           // Geursensor uit zetten
        GeurToestand=0;  
        if (Error==5)                                           // Door de ondebreking kunnen we een error 5 krijgen dat de geur sensor verkeerd gaat meten
        {
            Error=0;
        }
        Printing();
        InterupKnopGedrukt=0;
        Systeem=8;
    } 
}

void checkUserOnSeatForWhileWithNoSmell() {
    if (Systeem==6) {                                        // Zit er al 4 min op en geen stank
        VentilatorStop();
        VentilatorStart(VentSpeedTraag);
        digitalWrite(AfstandI2COnOff,HIGH);
        delayCheckUdp(500);        
        lox.begin();
        CheckAfstand();
        Geur();
        EvenOpgestaan=0;
        while ((GeurToestand!=3) and (EvenOpgestaan<2 ) and (Systeem!=7)) {     // Als we geen geur hebben en niet opgestaan en geen interup blijven we hier
            digitalWrite(AfstandI2COnOff,HIGH);
            delayCheckUdp(500);        
            lox.begin();
            CheckAfstand();
            Geur();
            Printing();
        
            if (AfstandSensor!=2) {
                EvenOpgestaan=EvenOpgestaan+1;
                if (EvenOpgestaan>1) {                                          // Mag 2 metingen anders zijn dan zitten op de wc 
                    Systeem=10;                                                 // Opgestaan en nalooptijd starten (30sec)
                }
            }
            else EvenOpgestaan=0;
        } 
        if (GeurToestand==3) Systeem = 5;                                       // als er stank gemeten is op volle karcht ga naar systeem 5  
    }
}

void removeSmells() {
    if (Systeem==5) {                                       // Stank en blijven draaien tot er niemand meer op zit
        digitalWrite(GeurSensorIO,0);                       // Geursensor uit zetten 
        GeurToestand=0;                                     // Voor het uitprinten
        VentilatorStop;                                     // Eerst stoppen voor de berekening
        VentilatorStart(VentSpeedNormaal);                  // Zet ventilator terug op hoge snelheid
        digitalWrite(AfstandI2COnOff,HIGH);                 // Afstands sensor terug aan zetten en gaan hier blijven kijken of ze terug opstaan voor verder te gaan
        delayCheckUdp(500);
        lox.begin();
        CheckAfstand();
        Printing(); 
        while (AfstandSensor==2) {                           // Als er nog iemand opzit blijven draaien
            digitalWrite(AfstandI2COnOff,HIGH);
            delayCheckUdp(500);        
            lox.begin();
            CheckAfstand(); 
        }
        Systeem=10;                                         // Als ze niet meer zitten gan naar systeem 10
        Printing();
    }
}

void checkUsage() {
    if (Systeem==4)                                         // Ventilator draaid en is ok, tijd starten voor geur meting en afstands meting laten door lopen
    {
        Timer_6_Old=millis()+(VentTijdNaarTraag*1000);
        EvenOpgestaan=0;                                      // pas triggeren als ze wat langer zijn opgestaan
        
        while ((GeurToestand!=3) and (EvenOpgestaan<2) and (Timer_6_New<Timer_6_Old) and (InterupKnopGedrukt==0)) // evenopgestaan was 4
        {
            digitalWrite(AfstandI2COnOff,HIGH);                 // hier de afstands sensor al aan zetten maar eerst geur aan zetten zodat we wat tijd sparen
            Geur();                                             // hier heef de afstand sensor de tijd
            CheckAfstand();                                     // nu pas afstand gaan meten
            Printing(); 
            Timer_6_New=millis();
            if (AfstandSensor != 2)                               // Indien ze even opgestaan zijn
            {
                EvenOpgestaan = EvenOpgestaan + 1;
                if (EvenOpgestaan>1)                              // Mag 2 metingen anders zijn dan zitten op de wc 
                    Systeem=10;                                     // Opgestaan en nalooptijd starten (30sec)
            }
            else
                EvenOpgestaan=0;
        }
        if (GeurToestand==3)                                  // als er stank gemeten is, ga naar systeem 5 
            Systeem=5; 
        if (Timer_6_New>Timer_6_Old)                          // Hier komen we als ze er lang op zitten en gaan we naar systeem 6
            Systeem=6; 
        if (GeurToestand==4)                                  // Hier komen we als er geurtoestand=4 (error op de geur sensor) en moeten toch verder werken
            Systeem=13;  
    }
} 

void testVent() {
    if (Systeem==3)                                         // Er zit iemand op wc en stroom ventilator testen en geursensor aan zetten
    {
        digitalWrite(GeurSensorIO,1);                         // Geursensor aan zetten
        GeurToestand=1;                                       // Geursensor aan het opwarmen voor het uitprinten
        VentilatorTesten();                                   // Test de ventilator
        if (FilterVervangingenOld==1)                         // Als de filter is terug geplaats de aaltal vervangingen +1
        {
            FilterVervangingenOld=0;
            FilterVervangingen=FilterVervangingen+1;
        }
        if ((Error==0) or (Error==5) or (Error==6))           // Als er geen error is of de geur sensor werkt niet of filter is verzadigd, toch gewoon verder gaan
        {
            VentilatorStart(VentSpeedNormaal);
            if (Systeem!=7)                                     // Als we juist in interupt zijn geweest mag hij niet terug systeem 4 worden
                Systeem=4;                                        // Verder gaan
            Printing(); 
        }
        else                                                  // Er is een error 
        {
            VentilatorStop();                                   // Ventilator stoppen
            digitalWrite(GeurSensorIO,0);                       // Geursensor uit zetten  
            GeurToestand=0;                                     // Voor het uitprinten
            ErrorAfhandeling();
        }
    }
}

/**
 * Met Drukknop uit slaap komen
 */
void checkAndPrepWakeupFromBtn() {
    if (UitslaapCode==2)                                        // kijken of de drukknop is gedruk
    {
        Timer_1_Old=(millis());
        while(digitalRead(DrukknopIO)==1)                       // tellen hoe lang de drukknop is ingedrukt
        {
            checkUdpIncomming();                                // always process any possible udp packets while in the loop so that the device remains responsive to mobile commands
            delay(1); 
        }
        Timer_1_New=(millis());
        Timer_1_Div=Timer_1_New-Timer_1_Old;

        if (Timer_1_Div< 500)                                   // kort gedrukt voor naar test te gaan
        {
            Drukknop=1;
            Systeem=1;
            Printing();
            TestDraaien();
        }
        if (Timer_1_Div > 500)                                  // lang gedrukt voor naar wifi te gaan
        {
            Drukknop=2;
            Systeem=2;
            Printing();
            if (hasWifiSettings()) {
                if (WifiStatus == 0) {                          // not yet fully started, so wait till it is.
                    if (!isTryingToConnect)                     // the wifi is not yet started, so do now
                        tryStartClientWifi();
                    WifiMode=1;                                 // this allows us to check if the user pressed the button again before we were able to start the wifi completely
                    while(!checkWifiConnected(true) && WifiMode==1)
                        delay(100);
                    if (WifiStatus == 0)                        // failed to connect to local wifi, start in ap mode instead
                        startApWifi();
                }
            }
            else 
                startApWifi();                                  // we need to start the wifi if it wasn't previously started yet (if not yet configured, the wifi doesn't normally start, but now we are in server mode, so ap-wifi mode is allowed)
            runServer();
            if (WifiStatus == 2)                                // make certain that ap mode is off, otherwise he mobile might remain connected to the esp, which is bad
                WiFi.softAPdisconnect (true);
            //tryStartClientWifi();                               // server mode is done, start back up in sta mode and try to connect. this makes certain we are no longer a server
            delayCheckUdp(400);                                // before going back to sleep, give this device and the mobile device a little time to connect to the router and communicate with each other
            REBOOT_AFTER_ENROLL = 1;
            ESP.restart();
        }
    }  
    attachInterrupt(32, Interup, RISING);                       // Zet de drukknop nu in interup mode met opgaande flank
}

/**
 * to do when woken up
 */
void checkWakeFromSleep() {
    if ((UitslaapCode==0) and (digitalRead(DrukknopIO)==1))   // Eerste keer uit slaap en de knop is gedrukt dus alle geheugens wissen
        EEPROMLeegmaken();
    if ((UitslaapCode==0) and (digitalRead(DrukknopIO)==0)) { // Eerste keer uit slaap en de knop is niet gedrukt dus batterij is vervangen
        EEPROMReadList();  
        BatterijVervangingen=BatterijVervangingen + 1;        // en hier de waarde in EEPROM uithalen en terug zetten
        EEPROMWritelong(3,BatterijVervangingen);
        EEPROM.commit(); 
        digitalWrite(AudioOnOff,HIGH);                        // Geluid aan zetten
        delayCheckUdp(2000); 
        Player.volume(Volume);                                // Zet volume
        delayCheckUdp(100);
        Player.play(7);                                       //Geef geluid dat de batterij is vervangen
        delayCheckUdp(2000); 
        digitalWrite(AudioOnOff,LOW);                         // Zet audio uit
    }
    // Gewoon na X tijd uit slaap komen?
    if (Systeem==0) {  
        ////// LED flikkeren om aan te tonen dat systeem in sleepcycle zit en werkt //////
        //digitalWrite(LedRoodIO,1);      
        //delay(10);                    
        //digitalWrite(LedRoodIO,0);
        //digitalWrite(LedWitIO,1);
        //delay(10);
        //digitalWrite(LedWitIO,0);
        Drukknop=0;
        digitalWrite(AfstandI2COnOff,HIGH);                   // Hier al de afstands meeting aan zetten om tijd te sparen    
        CheckFilter();                                        // Filter checken
        CheckBat();                                           // Batterij checken
        CheckAfstand();                                       // Afstand checken
        if (Systeem==3) {
            tryStartClientWifi();                             // only start the wifi when someone is on the toilet, this saves battery
        }   
        else if (REBOOT_AFTER_ENROLL == 1) {                  // give the mobile and device some time to communicate
            delayCheckUdp(15000);
        }
        ErrorAfhandeling();                                   // errors afhandelen
    }
    REBOOT_AFTER_ENROLL = 0;
}

void loop()
{
    esp_sleep_wakeup_cause_t wakeup_reason;                 // Haal de wake_up code op
    wakeup_reason = esp_sleep_get_wakeup_cause();           
    UitslaapCode=wakeup_reason;                             // Zet de Wake_up code in UitslaapCode
    checkAndPrepWakeupFromBtn();
    checkWakeFromSleep(); 
    testVent();
    checkUsage();
    removeSmells();
    checkUserOnSeatForWhileWithNoSmell();
    checkInteruptBtnPressed();
    checkCycleInteruptedWhileSitting();
    checkUserGotUp();
    checkLitClosed();
    checkSmellSensorDefect();
    checkGoToSleep();
}

void Interup()
{
  InterupKnopGedrukt=1;
  Drukknop=0;
  WifiMode=0;
  Systeem=7;
  Serial.println("INTERUPT");
  Printing();
}  
