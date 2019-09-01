#include <Adafruit_BMP280.h>

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

IPAddress    apIP(10, 10, 10, 1);

/* This are the WiFi access point settings. Update them to your likin */
const char *ssid = "ESP8266";
const char *password = "aaa";

float altitudeReference = 0;
float altitudeMax = 0;
float temp = 0;

bool acquisition = false;
long start = 0;

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

Adafruit_BMP280 bmp;

void handleStart() {

  Serial.println("Début d'acquisition");

  acquisition = true;
  altitudeMax = 0;
  temp = 0;
  start = millis();
  altitudeReference = bmp.readAltitude(1013.25);

  char html[1000];

  snprintf ( html, 1000,

"<html>\
  <head>\
    <meta name='viewport' content='viewport-fit=cover, width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no' />\
    <meta name='format-detection' content='telephone=no' />\
    <meta name='msapplication-tap-highlight' content='no' />\
    <title>ESP8266 WiFi Network</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
      h1 { Color: #AA0000; }\
    </style>\
  </head>\
  <body>\
    <h1>Fus&eacute;e &agrave; eau - Gestion de l'altitude</h1>\
    <p>Acquisition en cours</p>\
    <br/>\
    <form action='/stop' method='get'>\
      <input type='submit' value='Atterrir !' style='height:35px' />\
    </form>\
  </body>\
</html>"
  );
  server.send ( 200, "text/html", html );
}

void handleStop() {

  Serial.println("Fin d'acquisition");

  acquisition = false;

  char html[1000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  long time = millis() - start;
  

  Serial.print(altitudeMax);
  Serial.print(" m - ");
  Serial.print(temp);
  Serial.println(" °C");

  char altitudeReferenceStr[6];
  dtostrf(altitudeReference, 4, 2, altitudeReferenceStr);

  char altitudeMaxStr[6];
  dtostrf(altitudeMax, 4, 2, altitudeMaxStr);

  char altitudeMaxTempStr[6];
  dtostrf(temp, 4, 2, altitudeMaxTempStr);

  char tempsVol[6];
  dtostrf(time / 1000.0, 4, 2, tempsVol);

  char vitesse[6];
  dtostrf( (altitudeMax / 1000.0) / (time / 3600000.0), 4, 2, vitesse);

  snprintf ( html, 1000,

"<html>\
  <head>\
    <meta name='viewport' content='viewport-fit=cover, width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no' />\
    <meta name='format-detection' content='telephone=no' />\
    <meta name='msapplication-tap-highlight' content='no' />\
    <title>ESP8266 WiFi Network</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
      h1 { Color: #AA0000; }\
    </style>\
  </head>\
  <body>\
    <h1>Fus&eacute;e &agrave; eau - Gestion de l'altitude</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Altitude de ref&eacute;rence : %s m</p>\
    <br/>\
    <p>Altitude maximum : %s m</p>\
    <br/>\
    <p>Temp&eacute;rature &agrave; l altitude maximum : %s &deg;C</p>\    
    <br/>\
    <p>Temps de vol : %s secondes</p>\
    <br/>\
    <p>Vitesse moyenne : %s km par heure</p>\
    <br/>\
    <br/>\
    <form action='/start' method='get'>\
      <input type='submit' value='re-d&eacute;coller !' style='height:35px' />\
    </form>\
  </body>\
</html>",
    hr, min % 60, sec % 60,
    altitudeReferenceStr,
    altitudeMaxStr,
    altitudeMaxTempStr,
    tempsVol,
    vitesse
  );
  server.send ( 200, "text/html", html );
  
}

void handleRoot() {

  Serial.print(F("Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print("Altitude reference : ");
  Serial.print(altitudeReference);
  Serial.println(" metres");

  Serial.print("Current altitude : ");
  Serial.print(bmp.readAltitude(1013.25) - altitudeReference);
  Serial.println(" metres");

  char html[1000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  char altitudeReferenceStr[6];
  dtostrf(altitudeReference, 4, 2, altitudeReferenceStr);

  char altitudeCouranteStr[6];
  dtostrf(bmp.readAltitude(1013.25) - altitudeReference, 4, 2, altitudeCouranteStr);

  snprintf ( html, 1000,

"<html>\
  <head>\
    <meta name='viewport' content='viewport-fit=cover, width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no' />\
    <meta name='format-detection' content='telephone=no' />\
    <meta name='msapplication-tap-highlight' content='no' />\
    <title>ESP8266 WiFi Network</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; font-size: 1.5em; Color: #000000; }\
      h1 { Color: #AA0000; }\
    </style>\
  </head>\
  <body>\
    <h1>Fus&eacute;e &agrave; eau - Gestion de l'altitude</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <p>Altitude de ref&eacute;rence : %s m</p>\
    <br/>\
    <p>Altitude : %s m</p>\
    <form action='/start' method='get'>\
      <input type='submit' value='d&eacute;coller !' style='height:35px' />\
    </form>\
  </body>\
</html>",
    hr, min % 60, sec % 60,
    altitudeReferenceStr,
    altitudeCouranteStr
  );
  server.send ( 200, "text/html", html );

}




void handleNotFound() {

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

  server.send ( 404, "text/plain", message );
}


void setup() {

  Serial.begin(115200);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  else
  {
    Serial.println("BMP085 Sensor initialized");
    altitudeReference = bmp.readAltitude(1013.25);
  }
  
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 0, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  server.on ( "/", handleRoot);
  server.on ( "/start", handleStart);
  server.on ( "/stop", handleStop);
  
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (acquisition) {

    if (altitudeMax < (bmp.readAltitude(1013.25) - altitudeReference)) {
      altitudeMax = bmp.readAltitude(1013.25) - altitudeReference;
      temp = bmp.readTemperature();

      Serial.print(altitudeMax);
      Serial.print(" m - ");
      Serial.print(temp);
      Serial.println(" °C");
    }    
  }
  
}
