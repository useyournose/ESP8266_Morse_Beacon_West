void blink_morse_char(int led_pin, char c) {
  // Blink out the morse code for the specified character.
  // Unrecognized characters are ignored.

  // First look up the string of morse code for the character.
  const char* code = NULL;
  if ((c >= '!') && (c <= '_')) {
    // Get the code for an alphabet character.
    code = morse_codes[c-34];
  }
  else {
    // Unknown character, ignore it!
    return;
  }
  
  // Now loop through the components of the code and display them.
  // Be careful to not delay after the last character.
  for (int i = 0; i < strlen(code)-1; ++i) {
     // Turn the LED on, delay for the appropriate dot / dash time,
     // then turn the LED off again.
     digitalWrite(led_pin, led_on);
     delay((code[i] == '.') ? morse_dot_ms : morse_dash_ms);
     digitalWrite(led_pin, led_off);
     // Delay between characters.
     delay(morse_char_delay_ms);
  }
  // Handle the last character without any delay after it.
  digitalWrite(led_pin, led_on);
  delay((code[strlen(code)-1] == '.') ? morse_dot_ms : morse_dash_ms);
  digitalWrite(led_pin, led_off);
}

void blink_morse(int led_pin, const char* message) {
  // Blink out the morse code version of the message on the LED.
  Serial.println("Starting morsing");
  Serial.print(message);
  // Process each character in the message and send them out 
  // as morse code. Keep track of the previously seen character
  // to find word boundaries.
  char old = toupper(message[0]);
  blink_morse_char(led_pin, old);
  for (int i = 1; i < strlen(message); ++i) {
    char c = toupper(message[i]);
    // Delay for word boundary if last char is whitespace and 
    // new char is alphanumeric.
    if (isspace(old) && isalnum(c)) {
      delay(morse_word_delay_ms); 
    }
    // Delay for letter boundary if both last and new char 
    // are alphanumeric.
    else if (isalnum(old) && isalnum(c)) {
      delay(morse_letter_delay_ms);
    }
    // Else do nothing for other character boundaries.
    
    // Blink out the current character and move on to the next.
    blink_morse_char(led_pin, c);
    old = c;
  }
}

void form_url_decode(const char* encoded, char* decoded) {
  // Decode a string in 'application/x-www-form-urlencoded' format
  // to its normal ASCII representation.  See details on this
  // format in the HTML spec at:
  //   http://www.w3.org/MarkUp/html-spec/html-spec_8.html#SEC8.2.1
  // Note the output string MUST be large enough to hold the string!
  // As long as the output is at least as large as the input then
  // you will never have a problem (i.e. decoded strings are smaller).
  
  // Go through each character and transform it into the decoded
  // output as appropriate.
  for (int i = 0, j = 0; i < strlen(encoded); ++i, ++j) {
    char c = encoded[i];
    // The + character turns into a space.
    if (c == '+') {
      decoded[j] = ' '; 
    }
    // The % character indicates a special character in the
    // form of '%HH' where HH are the hex digits that represent
    // the character to use.
    else if (c == '%') {
      // If there isn't enough input data to read the next two
      // characters then just stop because this string is malformed.
      if (i >= strlen(encoded)-2) {
        return; 
      }
      // Grab the next two encoded characters.
      char high = toupper(encoded[++i]);
      char low  = toupper(encoded[++i]);
      // Skip this character if the encoded characters aren't hex.
      if (!isxdigit(high) || !isxdigit(low)) {
        continue;
      }
      // Convert high and low hex digits to the character
      // represented by their value.
      decoded[j] = (low > '9') ? (low-'A')+10 : low-'0';
      decoded[j] += 16*((high > '9') ? (high-'A')+10 : high-'0');

  //    int StrToHex(char str[])
  //    {
  //      return (int) strtol(str, 0, 16);
  //    }

    }
    // All other encoded characters are just copied over.
    else {
      decoded[j] = c;
    }
  } 
}
