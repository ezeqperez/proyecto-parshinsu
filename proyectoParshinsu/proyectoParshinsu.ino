#include <Wire.h>
#include <DHT11.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Parshinsu.h>

int ventilacion = 4;
int dacalor = 5;
int leds = 6;
int lamparas = 7;
int sensorth = 11;
DHT11 dht11(sensorth);


EstadoPlanta* estado = vegetacion;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  pinMode(ventilacion, OUTPUT);
  pinMode(leds, OUTPUT);
  pinMode(lamparas, OUTPUT);
  pinMode(dacalor, OUTPUT);

  //Low lo prende
  //High lo apaga

  //Inicializamos las cosas en apagado
  digitalWrite(ventilacion, HIGH);
  digitalWrite(leds, HIGH);
  digitalWrite(lamparas, HIGH);
  digitalWrite(dacalor, HIGH);

  //Solo voy a setear el tiempo cuando sea necesario
  //setDateTime();
}

void loop() {
  float temp, hum;

  //mostrarFecha();
  //Si es 0, leyo ok
  if (dht11.read(hum, temp) == 0) {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" Humedad: ");
    Serial.print(hum);
    Serial.println();
  }
  else {
    Serial.println("No se pudo leer el sensor :");
  }

  controlCalefaccion(temp);
  controlVentilacion(temp, hum);
  controlLuces();
  delay(300000);
}

//Se prende si la temperatura es menor a 20
void controlCalefaccion(int temp) {
  if (estado->temperaturaCalefaccion < 20) {
    prenderRele(dacalor);
  } else {
    apagarRele(dacalor);
  }
}

//Se prende si la temperatura es mayor a 26 o si la humedad es mayor a 65
void controlVentilacion(int temp, int hum) {
  if (estado->temperaturaVentilacion > 26 || estado->humedad > 80 || primerosCincoMinutos() || horaEnPunto(13)) {
    prenderRele(ventilacion);
  } else {
    apagarRele(ventilacion);
  }

}

void controlLuces() {
  if (tm.Hour >= estado->horaPrendido && tm.Hour < estado->horaApagado ) {
    Serial.print("Se prenden las luces, son las ");
    Serial.println(tm.Hour);
    prenderRele(leds);
    prenderRele(lamparas);
  } else {
    Serial.print("Luces Apagadas, son las ");
    Serial.println(tm.Hour);
    apagarRele(leds);
    apagarRele(lamparas);
  }
}

boolean horaParaComparar(){
  if(isAM()){
    return tm.Hour +24;
  }
  return tm.Hour;
}




