#include <WiFi.h>
#include <HTTPClient.h>
#define INPUT_PIN 34
#define LED 2
const char *ssid = "YOUR_SSID";
const char *password = "password";
void setup()
{
    Serial.begin(115200);
    delay(1000);		//Delay needed before calling the WiFi.begin
    pinMode(INPUT_PIN, INPUT);
    pinMode(LED, OUTPUT);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {	//Check for the connection
	delay(1000);
	Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
}

void loop()
{
    int val = digitalRead(INPUT_PIN);
    Serial.println(val);
    if (val) {
	digitalWrite(LED, HIGH);
    } else {
	digitalWrite(LED, LOW);
    }
    if (WiFi.status() == WL_CONNECTED) {	//Check WiFi connection status
	HTTPClient http;
	char buffer[20];
	sprintf(buffer, "{\"state\":%d}", val);
	http.begin("https://us-central1-datascience-181217.cloudfunctions.net/saveRelayState");	//Specify destination for HTTP request
	http.addHeader("Content-Type", "text/plain");	//Specify content-type header
	int httpResponseCode = http.POST(buffer);	//Send the actual POST request
	if (httpResponseCode > 0) {
	    String response = http.getString();	//Get the response to the request
	    Serial.println(httpResponseCode);	//Print return code
	    Serial.println(response);	//Print request answer
	} else {
	    Serial.print("Error on sending POST: ");
	    Serial.println(httpResponseCode);
	}
	http.end();		//Free resources
    } else {
	Serial.println("Error in WiFi connection");
    }
    delay(1000);		//Send a request every 10 seconds
}
