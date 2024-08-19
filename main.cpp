#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ESP32Servo.h>
#include <WiFiClientSecure.h>

// define el servidor web en el puerto 80
WebServer server(80);
String data = "nada";
Servo servo1;
Servo servo2;
int servo1Pin = 5;
int servo2Pin = 32;

// IPAddress local_IP(192, 168, 135, 232);  // Cambia esta dirección a la IP que deseas asignar al ESP32
// IPAddress gateway(192, 168, 1, 1);     // Dirección IP de la puerta de enlace (generalmente la IP del router)
// IPAddress subnet(255, 255, 255, 0);

String leerStringDeEEPROM(int direccion)
{
  String cadena = "";
  char caracter = EEPROM.read(direccion);
  int i = 0;
  while (caracter != '\0' && i < 100)
  {
    cadena += caracter;
    i++;
    caracter = EEPROM.read(direccion + i);
  }
  return cadena;
}

void handleGetData()
{
  if (server.hasArg("data"))
  {
    data = server.arg("data");
    Serial.println("Datos recibidos: " + data);
    server.send(200, "text/plain", "Datos recibidos");
  }
  else
  {
    server.send(400, "text/plain", "No se recibieron datos");
  }
}

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(512);
  // WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(leerStringDeEEPROM(50), leerStringDeEEPROM(70));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  // Una vez conectado, muestra la dirección IP
  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Inicia el servidor
  server.on("/data", HTTP_GET, handleGetData);
  server.begin();
  Serial.println("Servidor HTTP iniciado");

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.writeMicroseconds(1500);
  servo2.writeMicroseconds(1500);
}

void loop()
{
  server.handleClient();
  if (!(data == "stop"))
  {
    int dato1 = data.indexOf(',');
    int dato2 = data.indexOf(',', dato1 + 1);
    String elevacionx = data.substring(0, dato1);
    String elevaciony = data.substring(dato1 + 1, dato2);
    float datox = elevacionx.toFloat();
    float datoy = elevaciony.toFloat();
    int velocidad;
    if (datoy < -75)
    {
      velocidad = 1000;
    }
    else if (datoy < -45)
    {
      velocidad = 1200;
    }
    else if (datoy < -15)
    {
      velocidad = 1400;
    }
    else if (datoy < 15)
    {
      velocidad = 1500;
    }
    else if (datoy < 45)
    {
      velocidad = 1600;
    }
    else if (datoy < 75)
    {
      velocidad = 1800;
    }
    else
    {
      velocidad = 2000;
    }
    int giro;
    if (datox < -75)
    {
      giro = 1000;
    }
    else if (datox < -45)
    {
      giro = 1200;
    }
    else if (datox < -15)
    {
      giro = 1400;
    }
    else if (datox < 15)
    {
      giro = 1500;
    }
    else if (datox < 45)
    {
      giro = 1600;
    }
    else if (datox < 75)
    {
      giro = 1800;
    }
    else
    {
      giro = 2000;
    }

    int control = ((velocidad - 1500) * (giro - 1500)) / 500;

    if (giro > 1500)
    {
      servo1.writeMicroseconds(velocidad - control);
      servo2.writeMicroseconds(3000 - velocidad);
    }
    else
    {
      servo1.writeMicroseconds(velocidad);
      servo2.writeMicroseconds(3000 - velocidad - control);
    }
    delay(200);
  }
  else
  {
    servo1.writeMicroseconds(1500);
    servo2.writeMicroseconds(1500);
  }
  delay(200);
}