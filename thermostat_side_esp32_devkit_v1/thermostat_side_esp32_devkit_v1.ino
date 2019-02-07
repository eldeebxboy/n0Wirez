//#include <WiFi.h>
//#include <HTTPClient.h>
#define LED 2

#define INPUT0 19 // D19
#define INPUT1 21 // D21
#define INPUT2 22 // D22
#define INPUT3 23 // D23

const char *ssid = "YOUR_SSID";
const char *password = "password";
void setup()
{
    //pinMode(INPUT_PIN, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(INPUT0, INPUT);
    pinMode(INPUT1, INPUT);
    pinMode(INPUT2, INPUT);
    pinMode(INPUT3, INPUT);
/*
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
*/
}

void loop()
{
    int input0 = digitalRead(INPUT0);
    int input1 = digitalRead(INPUT1);
    int input2 = digitalRead(INPUT2);
    int input3 = digitalRead(INPUT3);
    if (input0 |input1 |input2 | input3) {
         digitalWrite(LED, HIGH);
    } else {
         digitalWrite(LED, LOW);
    }
/*
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
*/
}
