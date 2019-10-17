#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Replace with your network credentials
//const char* ssid = "WSHYDRO";
//const char* password = "d7f4r5aes";
const char* ssid = "MOSNET15022";
const char* password = "655dtv22";

WiFiServer server(80);

// Data wire is connected to GPIO15
#define ONE_WIRE_BUS 15

// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);
// put your DS18B20 sensor addresses here. sensaddr first subscript determines the number of addresses (10 now)
byte sensaddr[10][8] = {
     { 0x28, 0xC9, 0x23, 0x79, 0x97, 0x9, 0x3, 0x98 },
    { 0x28, 0xE9, 0x76, 0x79, 0x97, 0x13, 0x3, 0x62 },
    { 0x28, 0x27, 0x80, 0x79, 0x97, 0x11, 0x3, 0x45 },
    { 0x28, 0x19, 0x62, 0x79, 0x97, 0x11, 0x3, 0xE2 },
    { 0x28, 0x97, 0xE7, 0x79, 0x97, 0x11, 0x3, 0x2 },
    { 0x28, 0x11, 0x5F, 0x79, 0x97, 0x9, 0x3, 0xB1 },
    { 0x28, 0x81, 0x4A, 0x79, 0x97, 0x13, 0x3, 0xA5 },
    { 0x28, 0xB6, 0xD3, 0x79, 0x97, 0x13, 0x3, 0x89 },
    { 0x28, 0x80, 0x44, 0x79, 0x97, 0x11, 0x3, 0xA1 },
    { 0x28, 0xBC, 0xCC, 0x79, 0x97, 0x13, 0x3, 0x55 }
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
        int n = sizeof(sensaddr) / sizeof(sensaddr[0]);
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close"); // the connection will be closed after completion of the response
        client.println();
        client.println("<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        client.println("<meta http-equiv=\"refresh\" content=\"2\">");
        client.println("<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>");
        client.println("<script type=\"text/javascript\">");
        client.println("google.load('visualization', '1.0', {'packages': ['corechart']});");
        client.println("google.setOnLoadCallback(drawChart);");
        client.println("function drawChart() {");
        
        client.println("var data = new google.visualization.DataTable();");
        client.println("data.addColumn('number', 'Dist');");
        client.println("data.addColumn('number', 'Temp');");
        client.println("data.addColumn({type:'number', role:'annotation'});");
        client.println("data.addRows([");
        for (int i = 0; i < 10; i++) {
                char t[35];
//                  float tempread = sensors.getTempC(sensaddr[i]);
                  sprintf(t, "[%d, %f, %f ],", i, sensors.getTempC(sensaddr[i]), sensors.getTempC(sensaddr[i]) );
                  client.print(t);
                };
        client.println("]);");
        client.println("var options = { ");
        client.println("'title': 'Thermowire', ");
        client.println("hAxis: { gridlines: { count: 8 } },");
        client.println("lineWidth: 3, curveType: 'function', legend: { position: 'top' }, pointSize: 5");
        client.println("};");
        client.println("var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));");
        client.println("chart.draw(data, options);}");
        client.println("</script>");
        client.println("</head>");
        client.println("<body><h2><p>ESP32 DS18B20</h2><p>");
        client.println("<div id=\"curve_chart\" style=\"width: 1000px; height: 500px\"></div>");
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
