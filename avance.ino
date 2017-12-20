#include <ESP8266WiFi.h>
#include <SPI.h>

char ssid[] = "Hale bopp";
char pass[] = "hyperVelocidad11";
char ip_server[] = "192, 168, 0, 25";
char port_string[] = "10000";
char pila_recv[4] = {' ', ' ', ' ', ' '};
char pila_data[] = {'A', 'B', 'C', 'D'};
IPAddress server(192, 168, 0, 25);
WiFiClient NodoB;
char msg_id = 'I m nodo B';
void setup() {
  Serial.begin(115200);
  Serial.println("Setup...");
  Serial.print("ssID: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Conectando:");
  while (WiFi.status() != WL_CONNECTED) {
    delay(700);
    Serial.print(".");
  }
  Serial.println("     ok.");
  Serial.println("Conexion correcta a la red");
  Serial.println("Iniciando conexion al servidor Host:  ");
  Serial.print(ip_server);
  Serial.print(" Port: ");
  Serial.println(port_string);
  Serial.print("Conectando:");
  while (!NodoB.connect(server, 10000)) {
    delay(700);
    Serial.print(".");
  }
  Serial.println("Conectado");

}
void loop() {

  char c;

  if (NodoB.available() > 0) {
    c = NodoB.read();
    pila_recv[0] = c;
  }


  NodoB.print(c);


  Serial.println('Desconectado');
  Serial.println(pila_recv[0]);
  NodoB.stop();
}


