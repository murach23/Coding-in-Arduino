// =====================================================================
/// \file   RadioButtonDemo.ino
///
/// \brief  Radio Button demonstration consisting of a main page with
///         one '404' (Not Found Page) and one '500' (Internal Server
///         Error Page). 
///         This demo can be used to remotely control the on-board LED.
// =====================================================================

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

#undef BUILTIN_LED 
#define BUILTIN_LED D4
char countTFC[80];
char countN[80];
char countTFS[80];
char countRGBS[80];
char countTEMp[80];
char countTILT[80];
char countTSW1[80];
char countWSW1[80];
char countTSW2[80];
char countWSW2[80];

byte cmd = 0x00;

// Give your access point a name and a password
const char* ssid = "Marcin";
const char* password = "password10";

// Create instance of the ESP8266WebServer with a port number of
// 80 for HTTP.
ESP8266WebServer server(80);

// This is the 'root'/'index page. This can *typically* be 
// accessed at http://192.168.4.1. This contains two radio
// buttons and a submit button

const char INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<title>ESP8266 Web Form Demo</title>"
"</head>"
"<body>"
"<h1> Lecimy tutaj</h1>"
"<FORM action=\"/\" method=\"post\">" "<P>" "LED<br>"
"<INPUT type=\"radio\" name=\"LED\" value=\"1\">On<BR>"
"<INPUT type=\"radio\" name=\"LED\" value=\"0\">Off<BR>"
"<INPUT type=\"submit\" value=\"Send\">" "</P>"
"</FORM>"

"<form action=\"/res\">"
"<input type=\"submit\" value=\"Reset\">"  //reset
"</form>"

"<form action=\"/ledoff\">"
"<input type=\"submit\" value=\"Off\">"  //off
"</form>"



"<form action=\"/ledon\">"
"<input type=\"submit\" value=\"Red\">"  // Hazard / Service vehicle
"</form>"

"<form action=\"/ledon1\">"
"<input type=\"submit\" value=\"Green\">" // Emergency
"</form>"

"<form action=\"/ledon2\">"
"<input type=\"submit\" value=\"Blue\">" // Doctor on call
"</form>"

"<form action=\"/ledon3\">"
"<input type=\"submit\" value=\"Amber\">" // Stationary police car
"</form>"

"<form action=\"/tl\">"
"<input type=\"submit\" value=\"equal\">" // Stationary police car
"</form>"

"<form action=\"/Set1\">"
"<input type=\"submit\" value=\"set1\">" // Stationary police car
"</form>"

"<form action=\"/Set2\">"
"<input type=\"submit\" value=\"set2\">" // Stationary police car
"</form>"

"<form action=\"/Pelican\">"
"<input type=\"submit\" value=\"pelican\">" // Stationary police car
"</form>"

"<form action=\"/Maintenance\">"
"<input type=\"submit\" value=\"maintenance\">" // Stationary police car
"</form>"

"</body>"
"</html>";


// This is our '404' Error - Page Not Found page
const char NOTFOUND_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<title>404 Not Found</title>"
"<body>"
"<h1>Oops! It's 404-time! Page not found!</h1>"
"</body>"
"</html>";


// This is our '500' Error - Internal Server Error
const char INTERNAL_SERVER_ERROR_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<title>500 Internal Server Error</title>"
"<body>"
"<h1>500 Internal Server Error</h1>"
"<p>LED mode unsupported<\p>"
"</body>"
"</html>";


//----------------------------------------------------------------------------
//                             PAGE HANDLING FUNCTIONS
//----------------------------------------------------------------------------

// ===============================================================
/// \fn     void handleRoot()
///
/// \brief  This function is called whenever a call to the index
///         page (i.e. http://192.168.4.1) is made.
///
///         If the LED name has a valid argument, the function
///         handleSubmit() is called to process the argument.
///         Otherwise, the response held in the INDEX_HTML will be 
///         sent in text/html format with HTTP error code 200 (OK)
// ===============================================================
void handleRoot()
{
  char html[2048];
  char temp[80];
  strcpy(html, INDEX_HTML);
  sprintf(temp, "<p>TFC=%s</p>", countTFC);
  strcat(html, temp);
  sprintf(temp, "<p>TFN=%s</p>", countN);
  strcat(html, temp);
  sprintf(temp, "<p>TFS=%s</p>", countTFS);
  strcat(html, temp);
  sprintf(temp, "<p>RGBS=%s</p>", countRGBS);
  strcat(html, temp);
  sprintf(temp, "<p>TEMp=%s</p>", countTEMp);
  strcat(html, temp);
  sprintf(temp, "<p>TILT=%s</p>", countTILT);
  strcat(html, temp);
  sprintf(temp, "<p>TSW1=%s</p>", countTSW1);
  strcat(html, temp);
  sprintf(temp, "<p>WSW1=%s</p>", countWSW1);
  strcat(html, temp);
  sprintf(temp, "<p>TSW2=%s</p>", countTSW2);
  strcat(html, temp);
  sprintf(temp, "<p>WSW2=%s</p>", countWSW2);
  strcat(html, temp);
  strcat(html, "</body>");
  strcat(html, "</html>");


    if (server.hasArg("LED")) 
    {
        handleSubmit();
    }
    else 
    {
        server.send(200, "text/html", INDEX_HTML);
    }
}

void change()
{
  
}

// ===============================================================
/// \fn     void handleSubmit()
///
/// \brief  This function is called to control the LED based on
///         the argument.
///
///         In the event of a valid argument, the LED is either
///         illuminated or extinguished. The response held in 
///         INDEX_HTML will be sent in text/html format with 
///         HTTP error code 200 (OK).
///         Otherwise, the internal server error response will be 
///         sent in text/html format with HTTP error code 500 (OK)
// ===============================================================

void handleSubmit()
{
    String LEDvalue;

    if (!server.hasArg("LED"))
    { 
        return;
    }
    
    // Obtain the argument
    LEDvalue = server.arg("LED");

    // Act on it
    if (LEDvalue == "1") 
    {
        writeLED(true);
        server.send(200, "text/html", INDEX_HTML);
    }
    else if (LEDvalue == "0") 
    {
        writeLED(false);
        server.send(200, "text/html", INDEX_HTML);
    }
    else 
    {
        server.send(500, "text/html", INTERNAL_SERVER_ERROR_HTML);  
    }
}


// ===============================================================
/// \fn     void handleLEDon()
///
/// \brief  This function is called whenever a call is made for
///         "ledon" (i.e. http://192.168.4.1/ledon)
///
///         The LED will be illuminated.
///         The response held in the string INDEX_HTML will be 
///         sent in text/html format with the error code 200 (OK)
// ===============================================================
void handleLEDon()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x68;
}
void handleLEDon1()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x69;
}
void handleLEDon2()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x6A;
}
void handleLEDon3()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x67;
}
void handletl()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x61;
}
void handleSet1()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x62;
}
void handleSet2()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x63;
}
void handlePelican()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x65;
}
void handleMaintenance()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x64;
}
void handleres()
{
    writeLED(true);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x6B;
}
// ===============================================================
/// \fn     void handleLEDoff()
///
/// \brief  This function is called whenever a call is made for
///         "ledoff" (i.e. http://192.168.4.1/ledoff)
///
///         The LED will be extinguished.
///         The response held in the string INDEX_HTML will be 
///         sent in text/html format with the error code 200 (OK)
// ===============================================================
void handleLEDoff()
{
    writeLED(false);
    server.send(200, "text/html", INDEX_HTML);
    cmd = 0x66;
}


// ===============================================================
/// \fn     void handleNotFound()
///
/// \brief  This function is called whenever a call is made for
///         an unsupported page
///
///         The response held in the string NOTFOUND_HTML will 
///         be sent in text/html format with the error code 404
///         (PAGE NOT FOUND)
// ===============================================================
//void handleLEDoff()
//{
//    writeLED(false);
//    server.send(200, "text/html", INDEX_HTML);
//    cmd = 0x41;
//}


// ===============================================================
/// \fn     void writeLED(bool LEDon)
///
/// \brief  Function to control the LED
///
/// \param  LEDon     LED status
// ===============================================================
void writeLED(bool LEDon)
{
    // Note inverted logic for NodeMCU 12E ESP8266
    if (LEDon)
    {
        digitalWrite(BUILTIN_LED, 0);
    }
    else
    {
        digitalWrite(BUILTIN_LED, 1);
    }
}


// ===============================================================
/// \fn     void setup()
///
/// \brief  One-off set-up calls.
// ===============================================================
void setup(void)
{
    // Set-up the LED GPIO
    pinMode(BUILTIN_LED, OUTPUT);
    writeLED(false);

    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    Serial.println("");
    Serial.println(""); Serial.print("Connected to ");
    Serial.println(ssid); Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    // Associate functions for root, ledon, ledoff and "Not Found"
    server.on("/", handleRoot);
    server.on("/ledon", handleLEDon);
    server.on("/ledon1", handleLEDon1);
    server.on("/ledon2", handleLEDon2);
    server.on("/ledon3", handleLEDon3);
    server.on("/ledoff", handleLEDoff);
    server.on("/tl", handletl);
    server.on("/Set1", handleSet1);
    server.on("/Set2", handleSet2);
    server.on("/Pelican", handlePelican);
    server.on("/Maintenance", handleMaintenance);
    server.on("/res", handleres);
//    server.onNotFound(handleNotFound);
    server.begin();

    // Kick-start the server
    server.begin();

    Serial.print("Connect to http://");
    Serial.println(WiFi.softAPIP());

    Wire.begin(D7, D6); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
}


// ===============================================================
/// \fn     loop()
///
/// \brief  Run ad-infinitum
// ===============================================================
void loop(void)
{
    server.handleClient(); 

    if (cmd != 0x00)
    {
        Serial.print(cmd);
        Serial.print(" = ");
        Serial.println(cmd,HEX);

        Wire.beginTransmission(8);
        Wire.write(cmd);
        Wire.endTransmission();
        Wire.requestFrom(8,1);
        while(Wire.available()){
          short jj;
          short ch=0;
          jj=Wire.read();
          ch=ch|(short)jj;
          Serial.println(ch);
        }

        cmd = 0x00;
    }
}
