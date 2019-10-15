#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network credentials
const char* ssid = "EsmoFi";
const char* password = "aa4bb3cc2";

WiFiServer server(80);

// Data wire is connected to GPIO15
#define ONE_WIRE_BUS 15

// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
byte sensaddr[3][8] = {
  { 0x28, 0xD6, 0xD, 0x79, 0x97, 0x13, 0x3, 0xB6 },
  { 0x28, 0xBB, 0x19, 0x79, 0x97, 0x11, 0x3, 0xCD },
  { 0x28, 0xC9, 0x23, 0x79, 0x97, 0x9, 0x3, 0x98 }
};

// Client variables
char linebuf[80];
int charcount = 0;

void setup() {

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop(void)
{

  // listen for incoming clients
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("New client");
    memset(linebuf, 0, sizeof(linebuf));
    charcount = 0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        //read char by char HTTP request
        linebuf[charcount] = c;
        if (charcount < sizeof(linebuf) - 1) charcount++;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        // send a standard http response header
        sensors.requestTemperatures();
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close"); // the connection will be closed after completion of the response
        client.println();
        client.println("<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        client.println("<meta http-equiv=\"refresh\" content=\"2\">");
        client.println("<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>");
        client.println("<script type=\"text/javascript\">");
        client.println("google.charts.load('current', {'packages':['corechart']});");
        client.println("google.charts.setOnLoadCallback(drawChart);");
        client.println("function drawChart() {");
        client.println("var data = google.visualization.arrayToDataTable([");
        client.println("['Dist','Temp'],");
        for (int i = 0; i < 3; i++) {
          char t[20];
          sprintf(t, "[%d, %f],", i, sensors.getTempC(sensaddr[i]));
          client.print(t);
        }
        client.println("]);");
        client.println("var options = { title: 'Temp profile', curveType: 'function', legend: { position: 'bottom' }, vAxis: {viewWindow: {min: 0,max: 30}}};");
        client.println("var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));");
        client.println("chart.draw(data, options)}</script>");
        client.println("</head>");
        client.println("<body><h2><p>ESP32 DS18B20</h2><p>");
        client.println("<div id=\"curve_chart\" style=\"width: 500px; height: 300px\"></div>");
        client.println("</body></html>");
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
      }
    }
  }
}
