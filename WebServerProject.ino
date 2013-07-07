#include <WiFi.h>
#include <Servo.h>
//Dichiarazione variabili
Servo porta;
Servo garage;
int portaAng;
int garageAng;
const int motorino = 8;
const int luci = 9;
const int temp = A0;

// Dichiarazione SSID e corrispondente Password
char ssid[] = "*******";    
char pass[] = "********";  
int keyIndex = 0;                
//Inizializzazione status scheda WiFi
int status = WL_IDLE_STATUS;
//Abilitazione server
WiFiServer server(80);

//Setup componenti e inizializzazione
void setup()
{
  Serial.begin(9600);    
  pinMode(luci, OUTPUT);
  pinMode(motorino, OUTPUT);
  porta.attach(6);  
  garage.attach(3);
  porta.write(160);        //Angoli di partenza
  garage.write(168);
  
   if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Scheda WiFi non presente"); 
    while(true);
  } 
  
  while ( status != WL_CONNECTED) { 
    Serial.print("Tentativo di connessione WPA network, SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  
  Serial.print("Connessione alla rete riuscita!");
  printWifiData();
  server.begin();
}

void loop()
{
  WiFiClient client = server.available();          //Creazione variabile client
  if (client) {                                    //Client connesso e stampa della pagina web al client
    boolean currentLineIsBlank = true;
    String buffer = "";  
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);   
        buffer+=c;       
        if (c == '\n' && currentLineIsBlank) {
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          
          //Calcolo temperatura
          int tempval = analogRead(temp);
          float voltage = (tempval/1024.0) * 5.0;
          float temperatura = (voltage - .5) *100;
                    
          IPAddress ip = WiFi.localIP();
          client.print("<FORM action=\"http://");
          client.print(ip);
          client.print("\" >");
          client.print("<p><h1><font color='cyan'>Arduino Domotic Home</font></h1></p>");
          client.print("<p><h3>Comandi disponibili: </h3></p>");
          client.print("<br>");
          client.print("<table width=\"75%\" border=\"1\″ cellpadding=\"10\" cellspacing=\"0\">");
          client.print("<tr>");
            client.print("<td>");
              client.print("<p> Luci Abitazione: </p>");
              client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"1\">Accendi</P>");
              client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"0\">Spegni</P>");
            client.print("</td>");
            client.print("<td>");
              client.print("<p> Garage: </p>");
              client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"5\">Apri</P>");
            client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"4\">Chiudi</P>");
            client.print("</td>");
          client.print("</tr>");
          client.print("<tr>");
            client.print("<td>");
              client.print("<p> Porta: </p>");
              client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"3\">Aperta</P>");
              client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"2\">Chiusa</P>");
              client.print("</td>");
            client.print("<td>");
              client.print("<p> Aria Condizionata: </p>");
              client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"7\">Avvia</P>");
              client.print("<P> <INPUT type=\"radio\" name=\"status\" value=\"6\">Spegni</P>");
             client.print("</td>");
          client.print("</tr>");
          client.print("<tr>");
            client.print("<td>");
              client.print("<p> Temperatura: </p>");
              client.print(temperatura);
             client.print("</td>");
          client.print("</tr>");
          client.print("</table>");
          client.print("<P> <INPUT type=\"submit\" value=\"Invia\"> </FORM>");
          
      
          break;
        } //Controllo operazione scelta dal client
        if (c == '\n') {
          
          currentLineIsBlank = true;
          buffer="";       
        } else if (c == '\r') {            
          if(buffer.indexOf("GET /?status=1")>=0)
            digitalWrite(luci,HIGH);  
          
          if(buffer.indexOf("GET /?status=0")>=0)
            digitalWrite(luci,LOW); 
          //Porta
          if(buffer.indexOf("GET /?status=2")>=0)
            porta.write(160);  
            
          if(buffer.indexOf("GET /?status=3")>=0)
            porta.write(0);
          //Garage
          if(buffer.indexOf("GET /?status=4")>=0)
            garage.write(168);
         
          if(buffer.indexOf("GET /?status=5")>=0)
            garage.write(90);
          //Aria dcondizionata
          if(buffer.indexOf("GET /?status=6")>=0)
            digitalWrite(motorino,LOW); 
         
          if(buffer.indexOf("GET /?status=7")>=0)
            digitalWrite(motorino,HIGH);
        }
        else {         
          currentLineIsBlank = false;
        }
      }
    }
    
    delay(3);
    
    client.stop();
  }
}

  //Stampa Status Wifi
void printWifiData() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}
