// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <Ticker.h>

#define LED 2 // blue led
#define RELAY0 0 // heater relay

#define STR_BUF_LEN 30 // number of characters in buffer

Ticker waiter;

// Replace with your network credentials
char tstatssid[STR_BUF_LEN];
char tstatssidpwd[STR_BUF_LEN];

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String ledState = "off";

bool menu = 1;

void now() {
    menu = 0;
    Serial.println("Timeout");
}

struct { // structure storing FLASH data 
    char isflashed[10] = ""; // see whether FLASH has been flashed before, must be "isflashed"
    char email[STR_BUF_LEN] = "";     // email for sending stats
    char masterpwd[STR_BUF_LEN] = ""; // master password
    char tstatssid[STR_BUF_LEN] = "";      // SSID of tstat network
    char tstatssidpwd[STR_BUF_LEN] = "";   // tstat network password
    char ssid[STR_BUF_LEN] = "";      // SSID of home network
    char ssidpwd[STR_BUF_LEN] = "";   // home network password
} data;
uint data_addr = 0;

void reset_flash() {
    Serial.println("Setting FLASH to default values:");
    strncpy(data.isflashed, "isflashed",STR_BUF_LEN);
    strncpy(data.email, "dummymail@dummy.dum",STR_BUF_LEN);
    strncpy(data.masterpwd, "mpwd",STR_BUF_LEN);
    strncpy(data.tstatssid, "TSTAT",STR_BUF_LEN);
    strncpy(data.tstatssidpwd, "tstatpwd",STR_BUF_LEN);
    strncpy(data.ssid, "COCOON_DEV",STR_BUF_LEN);
    strncpy(data.ssidpwd, "44417000",STR_BUF_LEN);
    EEPROM.put(data_addr,data);
    EEPROM.commit();
}


void print_eeprom() {
    EEPROM.get(data_addr,data);
    Serial.print("Email: ");
    Serial.println(String(data.email));  
    Serial.print("Master password: ");
    Serial.println(String(data.masterpwd));  
    Serial.print("Tstat SSID: ");
    Serial.println(String(data.tstatssid));  
    Serial.print("Tstat network password: ");
    Serial.println(String(data.tstatssidpwd));  
    Serial.print("Home network SSID: ");
    Serial.println(String(data.ssid));  
    Serial.print("Home network password: ");
    Serial.println(String(data.ssidpwd));  
}

void setup() {
    Serial.begin(115200);
    Serial.println("");
    Serial.println("Serial initialised");

    // FLASH INIT
    EEPROM.begin(512);
    EEPROM.get(data_addr,data);

    // Store default values if FLASH has not been programmed yet
    if ( strcmp(data.isflashed,"isflashed") != 0 ) {
        reset_flash();
    }

    // Enter masterpwd for setup menu
    Serial.printf("\n\rPassword?\n\r");
    waiter.attach(2, now); // two seconds to do that

    // Weak implementation of user menu
    char c;
    char buf[STR_BUF_LEN];
    String readString = "";
    int pwdmenu = 0, waspwd = 0;
    while (menu || pwdmenu) {
        // Catches characters on serial port until enter is pressed
        while (Serial.available() > 0) {
            c = Serial.read();
            if (readString.length() >= STR_BUF_LEN) {
                Serial.println("");
                Serial.println("Command too long! Won't be used.");
            }
            if (c != '\r' && c!='\n') {
                Serial.printf("%c",c);
                readString += c; 
            }
        }
        if (c == '\r' || c == '\n') { 
            c = '\0';
            // Master password test
            if(menu && readString == data.masterpwd) { // masterpwd test
                readString = "";
                pwdmenu = 1;
                waspwd = 1;
                menu = 0;
                waiter.detach();
                Serial.println("");
                Serial.println("PWD correct. You can skip entry by pressing ENTER.");
                Serial.println("Enter 'q' for exit.");
                Serial.println("Enter 'rd' for reset to default.");
                Serial.println("");
                Serial.println("EEPROM content:");
                print_eeprom();
                Serial.println("");
            }
            if (readString == "q") { // quits user menu
                menu = 0;
                pwdmenu = 0;
                continue;
            }
            if (readString == "rd") { // reset flash
                reset_flash();
                menu = 0;
                pwdmenu = 0;
                continue;
            }
            if (pwdmenu) { // goes through user menu
                pwdmenu++; 
            }
            Serial.println("");
            // User Menu
            switch(pwdmenu)
            {
                case 2:
                    Serial.println("Email: ");
                    if (readString == "") continue;
                    break;
                case 3:
                    Serial.println("Master password: ");
                    if (readString == "") continue;
                    readString.toCharArray(data.email, STR_BUF_LEN);
                    break;
                case 4:
                    Serial.println("tstat ssid: ");
                    if (readString == "") continue;
                    readString.toCharArray(data.masterpwd, STR_BUF_LEN);
                    break;
                case 5:
                    Serial.println("tstat ssid password: ");
                    if (readString == "") continue;
                    readString.toCharArray(data.tstatssid, STR_BUF_LEN);
                    break;
                case 6:
                    Serial.println("your wifi ssid: ");
                    if (readString == "") continue;
                    readString.toCharArray(data.tstatssidpwd, STR_BUF_LEN);
                    break;
                case 7:
                    Serial.println("your wifi password: ");
                    if (readString == "") continue;
                    readString.toCharArray(data.ssid, STR_BUF_LEN);
                    break;
                case 8:
                    pwdmenu = 0;
                    readString.toCharArray(data.ssidpwd, STR_BUF_LEN);
                    break;
            }
            readString = "";
        }
    }

    waiter.detach(); // detach timer function for pwd timeout

    if(waspwd) { // save user defined entries
        Serial.println("Data saved:");  
        print_eeprom();
    }

    // Initialize the output variables as outputs  
    pinMode(LED, OUTPUT);

    // Set outputs to LOW
    digitalWrite(LED, LOW);

    server.begin();

    // Start accesspoint with SSID and password
    WiFi.mode(WIFI_AP);
    Serial.println("");
    Serial.println("Starting access point with SSID: ");
    Serial.println(String(data.tstatssid));
    const char *ssid = data.tstatssid;
    const char *pwd = data.tstatssidpwd;
    while (!WiFi.softAP(ssid, pwd)) {    
        Serial.print(".");
        delay(500);
    }

    // Print local IP address and start web server
    Serial.println("Done.");
    Serial.println("IP address: 192.168.4.1");
    server.begin();
}

void loop(){
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
        Serial.println("New Client.");          // print a message out in the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                header += c;
                if (c == '\n') {                    // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // turns the GPIOs on and off
                        if (header.indexOf("GET /LED/on") >= 0) {
                            Serial.println("LED on");
                            ledState = "on";
                            digitalWrite(LED, LOW);
                        } else if (header.indexOf("GET /LED/off") >= 0) {
                            Serial.println("LED off");
                            ledState = "off";
                            digitalWrite(LED, HIGH);
                        }

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons 
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #77878A;}</style></head>");

                        // Web Page Heading
                        client.println("<body><h1>Tstat Web Server</h1>");

                        // Display current state, and ON/OFF buttons for GPIO 5  
                        client.println("<p>LED - State " + ledState + "</p>");
                        // If the ledState is off, it displays the ON button       
                        if (ledState=="off") {
                            client.println("<p><a href=\"/LED/on\"><button class=\"button\">ON</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/LED/off\"><button class=\"button button2\">OFF</button></a></p>");
                        } 

                        client.println("</body></html>");

                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                    } else { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}
