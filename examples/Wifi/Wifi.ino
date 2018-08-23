#include <ESPVGAX.h>
#include <ESP8266WiFi.h>
#include <fonts/arial12.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

WiFiServer server(80);
ESPVGAX vga;

void drawMessage(const char *s) {
  vga.begin();
  vga.clear(0);
  vga.setFont((uint8_t*)fnt_arial12_data, FNT_ARIAL12_SYMBOLS_COUNT, 
              FNT_ARIAL12_HEIGHT, FNT_ARIAL12_GLYPH_WIDTH);
  // measure text
  ESPVGAX::PrintInfo sz=vga.print(s, 0, 0, true, -1, ESPVGAX_OP_SET, false, -1, true);
  // draw text centered
  vga.print(s, ESPVGAX_WIDTH/2-sz.w/2, ESPVGAX_HEIGHT/2-(sz.y+FNT_ARIAL12_HEIGHT)/2, true);
  vga.delay(10000);
  vga.end();
}
void setup() {
  Serial.begin(9600);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // start VGA signal generation without PIXELDATA output (paused)
  String text="ESPVGAX ready to receive messages.\nUse this URL to send messages:\n\nhttp://";
  text+=WiFi.localIP().toString();
  text+="/?s=Hello World from Wifi to ESPVGAX!\n\nThis screen will disappear after 10 seconds! Bye bye";
  drawMessage(text.c_str());
}
String message="";

void loop() {
  if (message.length()) {
    drawMessage(message.c_str());
    message="";
  }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int idx=req.indexOf("?s=");
  if (idx != -1) {
    message=req.substring(idx+3, req.lastIndexOf(' '));
    message.replace("%20", " ");
    // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\
      <!DOCTYPE HTML><html><center>OK. Message Received</center></html>\r\n";
    // Send the response to the client
    client.print(s);
    client.flush();
    delay(1);
    Serial.println("Client disonnected");
  } else {
    Serial.println("invalid request");
    client.stop();
  }
}
