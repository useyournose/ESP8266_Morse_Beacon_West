// ESP8266 Morse Code Webserver Example
//
// Display a web page with a form and blink out the submitted
// message in morse code on an LED!
//
// Note you need to have a LED connected to pin 13 to see the
// morse code messages.
//
// Author: Tony DiCola
//
// License: MIT License
//
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <FS.h>
#include "index_html.h"
#include "thanks_html.h"

// Details of AP to connect to on boot. 
const char* ssid = "MorseBeaconWest";
const char* password = "";
const int AP_Channel = 1;
const int max_conn = 1;

// some AP config
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

// DNS config
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Pin connected to the LED.
const int led = 2;
//needed to handle the led pin
//pinMode(led, OUTPUT);
//digitalWrite(led,LOW);


// Global web server instance.
ESP8266WebServer server(80);

// Number of milliseconds for a dot in morse code.
const int morse_dot_ms = 100;
// Dash is 3 times the length of a dot.
const int morse_dash_ms = morse_dot_ms*3;
// Delay between parts of a character is the length of a dot.
const int morse_char_delay_ms = morse_dot_ms;
// Delay between letters in a word is the length three dots.
const int morse_letter_delay_ms = morse_dot_ms*3;
// Delay between words is seven dots.
const int morse_word_delay_ms = morse_dot_ms*7;

// Signal to turn the LED on and off.  By default the code
// expects a HIGH signal is LED on and LOW signal is LED off,
// but you can reverse things if your circuit is different.
const int led_on  = HIGH;
const int led_off = LOW;

// Maximum length of message, used for internal buffer size.
const int max_message_len = 500;

// Table of character to morse code mapping.
// Each string represents the dashes and dots for a character.
const char* morse_codes[61] = {
  //"",// blank 33
  "..--.", // !
  ".-..-.", // "
  ".", // #
  "...-..-", // $
  "", // %
  ".-...", // &
  ".----.", // '
  "-.--.", // (
  "-.--.-", // )
  "-..-", // *
  ".-.-.", // +
  "--..--", // ,
  "-....-", // -
  ".-.-.-", // .
  "-----",   // 0 48
  ".----",  // 1
  "..---",  // 2
  "...--",  // 3
  "....-",  // 4
  ".....",  // 5
  "-....",  // 6
  "--...",  // 7
  "---..",  // 8
  "----.",   // 9 57
  "---...", // :
  "-.-.-.", // ;
  ".", // <
  "-...-", // =
  ".", // >
  "..--..", // ?
  "..-.-.", // @
  ".-",     // A 65
  "-...",   // B
  "-.-.",   // C
  "-..",    // D
  ".",      // E
  "..-.",   // F
  "--.",    // G
  "....",   // H
  "..",     // I
  ".---",   // J
  "-.-",    // K
  ".-..",   // L
  "--",     // M
  "-.",     // N
  "---",    // O
  ".--.",   // P
  "--.-",   // Q
  ".-.",    // R
  "...",    // S
  "-",      // T
  "..-",    // U
  "...-",   // V
  ".--",    // W
  "-..-",   // X
  "-.--",   // Y
  "--..",   // Z 90
  "-.--.", // [
  ".", // \
  "-.--.-",// ]
  ".", // ^
  "..--.-" // _ 95
};

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Here are all my files!");

  // show all files on Filesystem
  SPIFFS.begin();
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {    
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
  }
  Serial.printf("\n");
  
  // print SSID
  Serial.print("Welcome to the ");
  Serial.println(ssid);
  
  Serial.print("Switch off wifi during boot.");
  WiFi.mode(WIFI_OFF);
  // Setup LED as an output and turn it off.
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  WiFi.mode(WIFI_AP);
  Serial.println();
  Serial.print("Setting soft-AP configuration ...");
  Serial.println(WiFi.softAPConfig(local_IP, local_IP, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ...");
  Serial.println(WiFi.softAP(ssid, password,AP_Channel,false,max_conn) ? "Ready" : "Failed!");
  Serial.println();

  Serial.print("Connection possible to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", local_IP);
  
  // Configure web server root handler to return the main 
  server.on("/", HTTP_GET, handleRoot);
  server.on("/thanks", HTTP_GET, handleThanks);
  
  // Configure a handler for the /morse endpoint.
  server.on("/morse", HTTP_POST, handleMorsePage);

  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  //404 handling
  //server.onNotFound([](){if(!handleFileRead(server.uri())) handleNotFound;});
  //server.onNotFound(httpDefault);
  server.onNotFound( handleNotFound );
  
  // switch on the http server
  server.begin();
  Serial.println("Morse code HTTP server started!");
  blink_morse(led, "MORSE BEACON WEST ONLINE");
  Serial.println("ready");
}
 
void loop(void)
{
  dnsServer.processNextRequest();
  // Take care of handling any web server requests.
  server.handleClient();
} 

