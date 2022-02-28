
/*
builds a json object containing the current state of the device and sends it as an
udp packet to the address.
*/
void sendDevState(IPAddress address, bool includeHist) {
    String packet = "{\"claimCode\": \"";
    packet += chipId;
    packet += "\", \"bat\": ";
    packet += BatLevel;
    packet += ", \"vol\": ";
    packet += Volume;
    packet += ", \"error\":";
    packet += Error;
    packet += ", \"filter\":";
    packet += (FilterVezadegingTijdTeller * 100 / FilterVervangenOpSec);
    packet += ", \"ventSpeed\":";
    packet += VentSpeedNormaal;
    packet += ", \"ventDelay\":";
    packet += VentNaloopTijdOpstaan;
    packet += ", \"ventOn\": ";
    packet += VentOn ? "true" : "false";
    packet += ", \"firmwareVer\": ";
    packet += SOFWAREVERSION;
    packet += ", \"server\": ";
    packet += ( WifiMode == 1 ? "true" : "false");
    if (includeHist) {
        packet += ", \"bootCount\": ";
        packet += bootCount;

        packet += ", \"updateMemCnt\": ";
        packet += UpdateMemoryBootCounter;

        packet += ", \"litCnt\": ";
        packet += RTCDekselOpenDichtTeller;

        packet += ", \"sitCnt\": ";
        packet += RTCNeergezeten;

        packet += ", \"lastSitDur\": ";
        packet += RTCZittijd;

        packet += ", \"totalSitDur\": ";
        packet += RTCTotaalZittijd;

        packet += ", \"avgSitDur\": ";
        packet += RTCGemZittijd;

        packet += ", \"avgSmel\": ";
        packet += GeurGemiddeldeTriggerWaarde;

        packet += ", \"ventLowSpdDur\": ";
        packet += VentilatorLaagTotaalSec;

        packet += ", \"ventHighSpdDur\": ";
        packet += VentilatorHoogTotaalSec;

        packet += ", \"ventActiveDur\": ";
        packet += (VentilatorHoogTotaalSec + VentilatorLaagTotaalSec);
    }
    packet += "}";

    udp.beginPacket(address, UDP_PORT);
    Serial.println(address);
    Serial.println(packet);
    Serial.println(packet.length());
    udp.write((const uint8_t*)packet.c_str(), packet.length());
    udp.endPacket();
}

IPAddress broadcastAddress() {
    IPAddress subnet = WiFi.subnetMask();
    IPAddress ip = WiFi.localIP();
    IPAddress broadcast = ip | ( ~ subnet );
    return broadcast;
}

void broadcastUdpState() {
    if (WifiStatus == 2) {                                          // only try to send when connected
        IPAddress broadcastAdrs = broadcastAddress();
        Serial.println("broadcast udp state");
        sendDevState(broadcastAdrs, false);
    }
}

void broadcastUdpStateAndHistory() {
    if (WifiStatus == 2) {                                          // only try to send when connected
        IPAddress broadcastAdrs = broadcastAddress();
        Serial.println("send udp state and history");
        sendDevState(broadcastAdrs, true);
    }
}

void sendUdpToSleep() {
    if (WifiStatus == 2) {                                          // only try to send when connected
        IPAddress broadcastAdrs = broadcastAddress();
        Serial.println("send udp sleep");
        String packet = "{\"claimCode\": \"";
        packet += chipId;
        packet += "\", \"sleep\": true}";
        
        udp.beginPacket(broadcastAdrs, UDP_PORT);
        udp.write((const uint8_t*)packet.c_str(), packet.length());
        udp.endPacket();
        delay(350);                                                 // need to give the radio a little time to send the message
        udp.stop();
    }
}

void startUdp() {
    udp.begin(UDP_PORT);                            // initialize udp and transfer buffer
    Serial.print("udp buffer size: ");
    Serial.println(sizeof(udpBuffer));
    memset(udpBuffer, 0, sizeof(udpBuffer));        // clear the mem before we begin, so we only do when needed
    broadcastUdpState();
}


void processActuationCommand(IPAddress sender, String prop, String value) {
    Serial.println(prop);
    if (prop == "pwd") {
        value.toCharArray(system_pwd, 128);
        EEPROMSaveWifiInfo();                               // save now, otherwise it will never happen
        Serial.print("saved new pwd: ");
        Serial.println(value);
    }
    else if (prop == "volume") {
        Volume = value.toInt();
        if (Volume < 0)                                     // too small
            Volume = 0;
        else if (Volume > 30)                               // too big
            Volume = 30;
        Serial.print("set volume: ");
        Serial.println(Volume);
        sendDevState(sender, false);                        // let mobile know that value was set
        // don't save the data immediatly, this is done every 12 hours anyway, stick to this in order not to ware out the flash
    }
    else if (prop == "ventLevel") {
        VentSpeedNormaal = value.toInt();
        if (VentSpeedNormaal == 0)                     // bad value, so reset to keep the device working
            VentSpeedNormaal = 50; 
        else if (VentSpeedNormaal < 45)                // too small
            VentSpeedNormaal = 45;
        else if (VentSpeedNormaal > 55)                 // too big
            VentSpeedNormaal = 55;
        Serial.print("set vent speed: ");
        Serial.println(VentSpeedNormaal);
        if (VentOn) 
            VentilatorStart(VentSpeedNormaal);
        else 
            sendDevState(sender, false);                        // let mobile know that value was set
        // don't save the data immediatly, this is done every 12 hours anyway, stick to this in order not to ware out the flash
    }
    else if (prop == "ventAfterTime") {
        if (value == "20") 
            VentNaloopTijdOpstaan = 20;
        else if (value == "30") 
            VentNaloopTijdOpstaan = 30;
        else
            VentNaloopTijdOpstaan = 40;
        Serial.print("set ventAfterTime: ");
        Serial.println(VentNaloopTijdOpstaan);
        sendDevState(sender, false);                        // let mobile know that value was set
    }
    
}

void checkUdpIncomming() {
    if (WifiStatus == 0 && isTryingToConnect) {
        checkWifiConnected(false);
    }
    if (WifiStatus == 2) {
        int packetSize = udp.parsePacket();
        //receive response from server, it will be HELLO WORLD
        if(packetSize > 0){
            udp.read(udpBuffer, sizeof(udpBuffer));
            Serial.print("mobile to device: ");
            Serial.println((char *)udpBuffer);

            String json = String((char *)udpBuffer);
            String msg = jsonExtract(json, "msg");
            String sender = jsonExtract(json, "sender");
            if (msg == "pure-d" && sender == "mobile") {                          // we found a message from mobile to a device. always process this (it was either a broadcast or specifically sent to our ip)
                IPAddress senderIP = udp.remoteIP();
                Serial.print("ip of sender: ");
                Serial.println(senderIP);
                String action = jsonExtract(json, "action");
                if (action == "actuate") {
                    String prop = jsonExtract(json, "prop");
                    String value = jsonExtract(json, "value");
                    processActuationCommand(senderIP, prop, value);
                }
                else if (action == "history")
                    sendDevState(senderIP, true);
                else if (action == "ping")
                    sendDevState(senderIP, false);
            }
            memset(udpBuffer, 0, sizeof(udpBuffer));                                // make certain we clear the buffer for next use, so we don't reuse any previous data
        }
    }
}

void delayCheckUdp(uint32_t ms) {
    if (WifiStatus != 2 && !(WifiStatus == 0 && isTryingToConnect)) {               // not connected and not trying to conect, no need to try and check for udp messages, simpler, faster, saver to simply delay
        delay(ms);
    }
    else {
        unsigned long start = millis();
        unsigned long div = abs(millis() - start);
        while (div < ms) {
            checkUdpIncomming();
            div = abs(millis() - start);
        }
    }
}
