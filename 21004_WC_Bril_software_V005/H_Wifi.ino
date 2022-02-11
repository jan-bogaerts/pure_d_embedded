/**
 * @brief check if wifi ssid & pwd of user is configured
 * 
 * @return true 
 * @return false 
 */
bool hasWifiSettings() {
    Serial.println(home_ssid);//remove after testing
    Serial.println(home_pwd);//remove after testing
    return *home_ssid != 0 && *home_pwd != 0;
}

void printResponseHeader(WiFiClient* client, bool resultOk) {
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    if (resultOk) 
        client->println("HTTP/1.1 200 OK");
    else
        client->println("HTTP/1.1 400 Bad Request");
    client->println("Content-type:application/json");
    client->println("Connection: close");
    client->println();
}

bool processEnroll(String* header, WiFiClient* client) {
    int ssidIdx = header->indexOf("ssid=");
    Serial.print("idx of 'ssid=' = ");
    Serial.println(ssidIdx);

    int pwdIdx = header->indexOf("pwd=");
    Serial.print("idx of 'pwd=' = ");
    Serial.println(pwdIdx);

    int endIdx = header->indexOf(" HTTP/");
    Serial.print("terminal idx = ");
    Serial.println(endIdx);

    if (ssidIdx >= 0 && pwdIdx > 0 && endIdx > 0) {
        String ssid = header->substring(ssidIdx + 5, pwdIdx-1);
        Serial.println(ssid);
        ssid.toCharArray(home_ssid, 128);
        String pwd = header->substring(pwdIdx + 4, endIdx);
        Serial.println(pwd);
        pwd.toCharArray(home_pwd, 128);
        EEPROMSaveWifiInfo();
        printResponseHeader(client, true);
        client->println("{\"result\":true}");
        return true;
    }
    else if (ssidIdx == 0 && pwdIdx == 0) {
        printResponseHeader(client, false);
        client->println("{\"error\":\"missing ssid and missing pwd\"}");
    }
    else if (ssidIdx == 0) {
        printResponseHeader(client, false);
        client->println("{\"error\":\"missing ssid\"}");
    }
    else  {
        printResponseHeader(client, false);
        client->println("{\"error\":\"missing pwd\"}");
    }
    return false;
}

bool processClaim(String* header, WiFiClient* client) {
    int claimIdx = header->indexOf("code=");
    Serial.print("idx of 'code=' = ");
    Serial.println(claimIdx);
    
    int endIdx = header->indexOf(" HTTP/");
    Serial.print("terminal idx = ");
    Serial.println(endIdx);
    
    if (claimIdx >= 0 && endIdx) {
        String code = header->substring(claimIdx + 5, endIdx);
        Serial.println(code);
        String chipIdStr = String(chipId);
        Serial.println(chipIdStr);
        if (code == chipIdStr) {
            printResponseHeader(client, true);
            client->println("{\"result\":true}");
            return true;
        }
        else {
            printResponseHeader(client, false);
            client->println("{\"error\":\"invalid claim code\"}");
        }
    }
    else {
        printResponseHeader(client, false);
        client->println("{\"error\":\"missing claim code\"}");
    }
    return false;
}

bool processVerify(String* header, WiFiClient* client) {
    
    int pwdIdx = header->indexOf("pwd=");
    Serial.print("idx of 'pwd=' = ");
    Serial.println(pwdIdx);
    
    int endIdx = header->indexOf(" HTTP/");
    Serial.print("terminal idx = ");
    Serial.println(endIdx);

    if (pwdIdx >= 0 && endIdx >= 0) {
        String code = header->substring(pwdIdx + 4, endIdx);
        Serial.println(code);
        String pwdStr = String(system_pwd);
        if (code == pwdStr) {
            printResponseHeader(client, true);
            client->println("{\"result\":true}");
            return true;
        }
        else {
            printResponseHeader(client, false);
            client->println("{\"error\":\"invalid password\"}");
        }
    }
    else {
        printResponseHeader(client, false);
        client->println("{\"error\":\"missing password\"}");
    }
    return false;
}

void runServer()
{
    Serial.println("starting server");
    server.begin();                                                                     // Start de wifi comunicatie
    WifiMode=1;
    attachInterrupt(32, Interup, RISING);
    Printing();
    digitalWrite(LedWitIO,1);
    String header; 
    bool claimed = false;
    bool enrolled = hasWifiSettings();
    bool verified = false;
    while(WifiMode==1)
    {
        WiFiClient client = server.available();                                         // Listen for incoming clients
        if (client) {                                                                   // If a new client connects,
            Serial.println("New Client");                                               // print a message out in the serial port
            String currentLine = "";                                                    // make a String to hold incoming data from the client
            while (client.connected()) {                                                // loop while the client's connected
                if (client.available()) {                                               // if there's bytes to read from the client,
                    char c = client.read();                                             // read a byte, then
                    Serial.write(c);                                                    // print it out the serial monitor
                    header += c;
                    if (c == '\n') {                                                    // if the byte is a newline character
                        if (currentLine.length() == 0) {
                            if (header.indexOf("GET /enroll?") >= 0) 
                                enrolled = processEnroll(&header, &client);
                            else if (header.indexOf("GET /claim?") >= 0) 
                                claimed = processClaim(&header, &client);
                            else if (header.indexOf("GET /verify?") >= 0) 
                                verified = processVerify(&header, &client);                        
                            client.println();                                           // The HTTP response ends with another blank line
                            if ((claimed && enrolled) || verified)                     // if fully configured, we can exit the server loop and go back to normal processing.
                                WifiMode = 0;
                            break;                                                      // Break out of the while loop
                        } 
                        else {                                                          // if you got a newline, then clear currentLine
                            currentLine = "";
                        }
                    } 
                    else if (c != '\r') {                                               // if you got anything else but a carriage return character,
                        currentLine += c;                                               // add it to the end of the currentLine
                    }
                }
            }
            header = "";                                                                // Clear the header variable
            client.stop();                                                              // Close the connection
            Serial.println("Client disconnected.");
            Serial.println("");
        }
        else {
            checkUdpIncomming();                                                        // when adding an existing device, the mobile sends out an udp discovery packet, which we need to be able to handle
        }
    }
    delay(500);                                                                         // need to give the other side a little time before we stop completly and drop the wifi network (if in ap mode), which would give the other side errors
    Serial.println("server ended");
    digitalWrite(LedWitIO,0);
    WifiMode=0;
    Systeem=0;
}

/*
if there are credentials, start the wifi in sta mode
called when the device starts up.
*/
void tryStartClientWifi() {
    WiFi.persistent(false);                                     // prevent the wifi lib from writing to flash at every reboot, we don't need that and it kills flash mem
    WiFi.mode(WIFI_STA);
    if (hasWifiSettings()) {
        Serial.println("Connecting to WiFi in sta mode");
        WiFi.begin(home_ssid, home_pwd);
        checkWifiConnected(true);
    }
    else {
        isTryingToConnect = false;
        Serial.println("no wifi credentials, not connecting");
    }
}

bool checkWifiConnected(bool logNotConnected) {
    if (WiFi.status() == WL_CONNECTED) {
        isTryingToConnect = false;
        Serial.print("Connected to wifi, ip address: ");
        Serial.println(WiFi.localIP());
        WifiStatus = 2;
        startUdp();
        return true;
    }
    else {
        //Serial.println("Connection failed");
        WifiStatus = 0;
        isTryingToConnect = true;
        if (logNotConnected)
            Serial.println("not yet connected to wifi, retrying...");
        return false;
    }
}

/*
initializes the wifi in ap mode
*/
void startApWifi() {
    WiFi.persistent(false);                                     // prevent the wifi lib from writing to flash at every reboot, we don't need that and it kills flash mem
    Serial.print("starting wifi in ap mode");
    Serial.println("");
    Serial.print("on ssid ");
    Serial.println(ssid);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);             // make certain every device has a unique ssid name
    WifiStatus = 1;
    IPAddress IP = WiFi.softAPIP();                         // IP ADRES ESP32 = 192.168.4.1
}



/*
 
                    else if (header.indexOf("GET /17/on") >= 0) {           // turns the GPIOs on and off
                        Serial.println("GPIO 17 on");
                        output17State = "on";
                        digitalWrite(output17, HIGH);
                    } else if (header.indexOf("GET /17/off") >= 0) {
                        Serial.println("GPIO 17 off");
                        output17State = "off";
                        digitalWrite(output17, LOW);
                    } else if (header.indexOf("GET /18/on") >= 0) {
                        Serial.println("GPIO 18 on");
                        output18State = "on";
                        digitalWrite(output18, HIGH);
                    } else if (header.indexOf("GET /18/off") >= 0) {
                        Serial.println("GPIO 18 off");
                        output18State = "off";
                        digitalWrite(output18, LOW);
                    }




                    // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons 
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #555555;}</style></head>");
                        
                        // Web Page Heading
                        client.println("<body><h1>DENCK Web Server</h1>");
                        
                        // Display current state, and ON/OFF buttons for GPIO 17  
                        client.println("<p>GPIO 17 - State " + output17State + "</p>");
                        // If the output17State is off, it displays the ON button       
                        if (output17State=="off") {
                            client.println("<p><a href=\"/17/on\"><button class=\"button\">ON</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/17/off\"><button class=\"button button2\">OFF</button></a></p>");
                        } 
                        
                        // Display current state, and ON/OFF buttons for GPIO 18  
                        client.println("<p>GPIO 18 - State " + output18State + "</p>");
                        // If the output18State is off, it displays the ON button       
                        if (output18State=="off") {
                            client.println("<p><a href=\"/18/on\"><button class=\"button\">ON</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/18/off\"><button class=\"button button2\">OFF</button></a></p>");
                        }
                        client.println("</body></html>");

 */
