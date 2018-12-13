/** Handle root or redirect to captive portal */
void handleRoot() {
  Serial.println("handleRoot opened :");
  Serial.print(server.hostHeader());
  Serial.print(server.uri());
  if (captivePortal()) { // If captive portal redirect instead of displaying the page.
    return;
  }
  Serial.println("handleroot presenting file");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  //server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", index_html);
  //server.client().stop(); // Stop is needed because we sent no content length
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(ssid)+".local")) {
    Serial.print("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void handleMorsePage() {
  if (!server.hasArg("message")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }
//    String morsereply = "Sending morse light for \n\n";
//    morsereply += "URI: ";
//    morsereply += server.uri();
//    morsereply += "\nMethod: ";
//    morsereply += ( server.method() == HTTP_GET ) ? "GET" : "POST";
//    morsereply += "\nArguments: ";
//    morsereply += server.args();
//    morsereply   += "\n";
//    for ( uint8_t i = 0; i < server.args(); i++ ) {
//      morsereply += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
//    }
//    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
//    server.sendHeader("Pragma", "no-cache");
//    server.sendHeader("Expires", "-1");
//    server.send ( 200, "text/plain", morsereply );
  
    // Blink out the message in morse code, then respond 
    // with the root HTML page again.

    // Grab the message form parameter and fail if it's
    // larger than our max allowed size.
    //const char* 
    String morsemessage = server.arg("message");
    //.c_str();
    if (morsemessage.length() >= max_message_len) {
      server.send(500, "text/plain", "Message is too long!");
      return;
    }
    // Perform form URL decoding to get the plain message.
    //char decoded[max_message_len] = {0};
    //form_url_decode(morsemessage, decoded);
    morsemessage.toUpperCase();
    // Print and blink the message!
    Serial.print("Blinking message: "); Serial.println(morsemessage);
    blink_morse(led, morsemessage.c_str());
    // Return the main page again.
    // server.send(200,"text/html", index_html);
    handleRoot;
  }

void handleNotFound() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
}

void handlePureCss()
{
  Serial.print("CSS File opened");
  handleFileRead("/pure.css");
  //File file = SPIFFS.open("/pure.css.gz", "r");
  //server.streamFile(file, "text/css");
  //file.close();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") {
      output += ',';
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}

