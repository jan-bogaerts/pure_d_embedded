
/*
Toiletbril v3 04/2021
Algemene omschrijving..................................................................................... 2
Materialen ...................................................................................................... 2
Onderdelen..................................................................................................... 2
App (Android/iOS) ........................................................................................... 3
ESP32............................................................................................................ 5
Aansturing ................................................................................................... 5
Logging ....................................................................................................... 9
Algemene omschrijving
  Toiletbril die slechte geuren neutraliseert met zitting en deksel, beiden met soft-close
  functie en bestand tegen schoonmaakmiddelen. Uiteraard ook volgens de normen voor
  hygiëne.
  Gemakkelijke montage door middel van universeel montagesysteem (quick-release, zie
  pressalit.com) en in de meeste verkocht vorm/formaat.
  Batterij en filters zijn gemakkelijk te verwisselen door de gebruiker. Hiervoor dienen
  voorzieningen gemaakt te worden in de mal/print. De batterij heeft een USB-C
  connector (female) en wordt d.m.v. een USB-C naar USB-C kabel (male to male)
  gekoppeld aan de ESP32 chip. De filter wordt voorzien van een foam en is
  verwisselbaar.
Materialen
  Toiletbril, deksel, afdekplaat deksel
  ➢ Duroplast (Urea 131,5)?
  ➢ Behandeling via polygiene (Polygiene is a fabric treatment which uses low
    concentrations of silver salt (silver chloride), which has antimicrobial properties)
Schroeven
  ➢ Inox (tegen corosie)
Onderdelen
Duroplast
  ➢ Bril
  ➢ Bril afdekplaat
  ➢ Deksel
  ➢ Deksel schroefplaat
  ➢ Deksel magnetische plaat
  ➢ Afsluitklep filter
Accessoires
  ➢ Bumpers deksel
  ➢ Bumpers zitting
  ➢ Softclose scharnierset
Voeding
  ➢ Powerbank USB-C (20.000mAh)
  ➢ USB-C to USB-C cable voor powerbank te koppelen met de PCB
Ventilator
  ➢ Type: blower FAN DC5V/1.8A 2800rpm
Filters
  ➢ Filter
  ➢ Filter-foam (EPDM 3mm)
  ➢ Geurtje (op termijn als accessoire, niet bij lancering)
PCB
  ➢ Op de PCB
    o ESP32
    o Luidspreker (op PCB)
    o Ball-sensor
  ➢ Gekoppeld met connector op PCB
    o Geursensor (TGS2600)
    o Membraamfolie knop met LGF backlight (wit en rode LED)
    o Micro-switch voor filter-detectie
    o Aanwezigheidssensor (VL53L0X)
App (Android/iOS)
Hieronder de mogelijkheden van de app uitgeschreven.
Algemeen
  • Meertalig
  • Firmware updates mogelijk
Connectie
  • Voor consumenten:
    o Connectie via bluetooth
    o Beheer via Smartphone App
  • Voor B2B:
    o Connectie via Wifi
    o Beheer via Webportal
  • De toiletbril dient eerst in AdHoc modus op te starten zodat via de App
    verbinding gemaakt kan worden met de toiletbril.
  • Nadat de Adhoc verbinding gelukt is, kan de gebruiker verbinden met het eigen
    thuisnetwerk (WIFI) door de gegevens in te geven hiervan (SSID kiezen →
    wachtwoord WIFI-netwerk ingeven).
  • De gebruiker zal na het maken van de verbinding (1ste keer) gevraagd worden om een wachtwoord op te geven (zelf te kiezen) zodat deze kan opgeslagen
    worden in de toiletbril. Dit om later, wanneer andere gebruikers, ook willen
    verbinden, dit wachtwoord dient ingegeven te worden. Een wachtwoord is nodig
    om te voorkomen dat andere gasten zomaar de instellingen van de toiletbril
    kunnen aanpassen.
  • Functie wachtwoord vergeten?
  • Meerdere toiletbrillen toevoegen moet mogelijk zijn, brillen moeten te benoemen zijn, bv. “toilet boven”, “toilet onder”.
  • De membraamfolieknop geeft de mogelijkheid om na x-seconden inhouden om
    het connectieproces opnieuw te starten.
Logging
  • Wanneer de App wordt geopend, dient op de achtergrond data opgehaald te worden van de toiletbril. Deze data dient dan gepusht te worden naar een 
    cloudomgeving zodat wij die data steeds kunnen raadplegen. Deze data is anoniem, de gebruiker van de App dient dus geen account aan te maken.
Welke data: zie ESP32 > Logging
Voeding
  • Status van de batterij weergeven
Ventilator
  • Voltage kunnen regelen tussen een min. en max. waarde (te bepalen door ons)
  • Nalooptijd kunnen instellen dat de ventilator nog blijft draaien (20, 30, 40
  seconden)
Filter
  • Aantal bedrijfsuren tonen (kleurbalk groen → geel → rood).
  • Tonen of de filter aanwezig is of niet d.m.v. pictogram.
Luidspreker
  • Volume kunnen regelen
  • Aan/uit kunnen zetten
Notifications weergeven (melding, storingen)
  • Batterij is leeg (onder de X%)
  • Wanneer de ventilator teveel stroom vraagt (zit vast, versleten, …)
  • Filter(s) niet aanwezig
  • Filter verzadigd (a.d.h.v. aantal werkuren)
Varia
  • Enkele diverse pagina’s zoals user manual, faq, about, info, updates,…
Service page
  • Afgeschermde pagina d.m.v. een wachtwoord waarbij enkele extra instellingen kunnen gewijzigd worden:
o Gevoeligheid geursensor
ESP32
Aansturing
Algemeen
• Serienummer per printplaat?
• ESP32 in sleep plaatsen en elke X seconden met de afstandssensor meten of
persoon gedetecteerd wordt of niet. Indien ja → systeem start op.
Voeding (Powerbank)
• Ventilator start op en draait op default waarde de eerste 4 min. Als na 4 min. de
geursensor niets “riekt” gaat hij op lagere capaciteit draaien.

Ventilator
• De ventilator gaat aan op default waarde:
o Wanneer de aanwezigheidssensor een waarde meet tussen 2 en 20cm +
nalooptijd.
• De ventilator gaat op halve kracht:
o 4 minuten nadat de aanwezigheidssensor heeft gemeten en de geursensor
geen opkomende flank detecteert.
• De ventilator gaat terug op volle kracht:
o Wanneer de geursensor een opwaartse flank detecteert.
• De ventilator gaat uit:
o Wanneer de aanwezigheidssensor niets meer meet + nalooptijd.
o Wanneer het deksel gesloten is.
o Wanneer zitting verticaal staat → meting aanwezigheidssensor < 2cm.
• De ventilator mag nooit starten:
o Als er geen filter aanwezig is.
o Als de batterijspanning te laag is (onder X%).
o Bij een storing.
• De ventilator dient ten alle tijden een constante spanning behouden, ook
wanneer de batterijspanning zou zakken.
Filters
• Meten of de filter aanwezig is.
Storingen / Meldingen
Storingen hebben voorrang op meldingen. Bij storing “filter niet aanwezig”,
“geursensor defect” of “ventilatie defect” mag de toiletbril niet meer werken.
• Batterij geplaatst of netstroom aanwezig:
o Melodie 1 afspelen
• Batterij onder X%:
o Melodie 2 afspelen
o Diagnoseled = rood pinkend 3x
• Batterijspanning te laag:
o Melodie 3 afspelen
o Diagnoseled = rood pinkend continue (snel)
• Filter verzadigt:
o Melodie 5 afspelen
o Diagnoseled = rood pinkend 4x
• Filter niet aanwezig of slecht geplaatst:
o Melodie 6 afspelen
o Diagnoseled = rood snel pinkend continue
o Toestel mag niet meer werken!
• Geursensor defect (te lage stroom):
o Melodie 8 afspelen
o Diagnoseled = rood pinkend 5x
o Toestel mag niet meer werken!
• Ventilator defect (te lage/te hoge stroom):
o Melodie 9 afspelen
o Diagnoseled = rood continue
o Toestel mag niet meer werken!
Aanwezigheidssensor
• De aanwezigheidssensor bepaalt wanneer het systeem actief moet worden:
o Meting tussen 2 en 20cm = persoon zit op toilet
o Meeting > 20cm = deksel staat open, maar er zit niemand op het toilet
o Meting < 2cm = deksel is toe of staat open maar de zitting staat ook open
Sensor deksel open/dicht
• Ball-sensor op de PCB.
Geursensor
• Moet voorverwarmd worden met een uitgang 5V DC.
• De waarde van de geurdetectie is een oplopende spanning tussen 1 en 5 volt
(analoge meting) en dient elke stijgende flank waar te nemen met een minimum
van 200mv ( → ventilatie terug op volle kracht) (stijgende flank)
Membraamfolieknop
• Induwen wanneer systeem actief = uitschakelen
• Induwen wanneer systeem niet actief = inschakelen
• 5 seconden inhouden = connectie opnieuw instellen (wifi)
• 20 seconden inhouden = developer modus (?) → meer mogelijkheden in de
smartphone app om bv serienummer uit te lezen of dergelijke. Moet nog
besproken worden met app ontwikkelaar.

Logging
Het loggen van diverse parameters om statistieken te kunnen bouwen. Deze data
wordt opgehaald wanneer er via de app connectie wordt gemaakt (push naar cloudomgeving).
Welke data loggen:
• Hoe vaak is de deksel open gegaan.
• Hoe vaak is de bril omhoog gegaan.
• Hoe vaak is de aanwezigheidssensor geactiveerd.
• Welke is de duurtijd van elke toiletbeurt.
• Totaal aantal bedrijfsuren ventilator op volle kracht.
• Totaal aantal bedrijfsuren ventilator op halve kracht.
• Bedrijfsuren batterij a.d.h start/stop percentage.
• Hoeveel oplaadcycli heeft de batterij (elke keer de batterij > 99% opgeladen is).
• Hoe vaak is de filter vervangen.
• Hoe vaak heeft de geursensor een startmelding gekregen en welke was dan de
begin en de eindwaarde (in volt).-
*/

// 1  Filter niet aanwezig                                    0001.mp3  Rled 1x   
// 2  Batterij onder X%                                       0002.mp3  Rled 2x   
// 3  Ventilator (verkeerde stroom of niet aangesloten)       0003.mp3  Rled 3x   
// 4  Afstandssensor werkt niet                               0004.mp3  Rled 4x   Als ik de afstandssensor uit trek, speelt hij geen sound. Toont wel 4x WIT LED maar met hele tijdsspanne tussen. Als ik de afstandssensor terug insteek, start hij ook niet meer tenzij ik de batterij ontkoppel.
// 5  Geursensor werkt niet                                   0005.mp3  Rled 5x   Geeft de error pas als iemand een 2de keer terug gaat zitten (dus niet de 1ste keer). Error gaat ook niet weg tenzij ik de batterij ontkoppel.
// 6  Filter verzadigd (na 8100 min bedrijfstijd ventilator)  0006.mp3  Rled 6x   Nog bij te voegen
// 7  Batterij geplaatst                                      0007.mp3  Wled 1x   Nog bij te voegen
