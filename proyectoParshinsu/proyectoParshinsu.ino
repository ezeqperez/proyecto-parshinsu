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
  //setTime(17,15,0,1, 8,2017);
  //Solo voy a setear el tiempo cuando sea necesario
  //setDateTime();
}

void loop() {
  float temp, hum;
  leerHora();

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
  if (temp<estado->temperaturaCalefaccion) {
    Serial.println(estado->temperaturaCalefaccion);
    //prenderRele(dacalor);
    digitalWrite(dacalor,LOW);
  } else {
    //apagarRele(dacalor);
    Serial.println(estado->temperaturaCalefaccion);
    digitalWrite(dacalor,HIGH);
  }
}

//Se prende si la temperatura es mayor a 26 o si la humedad es mayor a 65
void controlVentilacion(int temp, int hum) {
  if (temp>estado->temperaturaVentilacion || hum>estado->humedad || primerosCincoMinutos() || horaEnPunto(13)) {
    //prenderRele(ventilacion);
    digitalWrite(ventilacion,LOW);
    
  } else {
    //apagarRele(ventilacion);
    digitalWrite(ventilacion,HIGH);
  }

}

void controlLuces() {
  if(dentroDeLaDuracion(estado->horaPrendido, estado->horaApagado)){
      mostrarFecha();
      Serial.print("Se prenden las luces, son las ");
      mostarHorario();
      digitalWrite(leds,LOW);
      digitalWrite(lamparas,LOW);
    } else {
      mostrarFecha();
      Serial.print("Luces Apagadas, son las ");
      mostarHorario();
      digitalWrite(lamparas,HIGH);
      digitalWrite(leds,HIGH);
    }
}

void mostarHorario(){
      Serial.print(hourRT);
      Serial.print(", Horario de prendido: ");
      Serial.print(estado->horaPrendido);
      Serial.print(" Horario de apagado: ");
      Serial.print(estado->horaApagado);
      Serial.println();
}


boolean dentroDeLaDuracion(int prendido, int apagado){
  //Analizar el caso de las 23 y las 0
  Serial.println(prendido);
  Serial.println(apagado);

  //Si la iluminacion esta prendida, llegado al horario de apagado lo apago
  if(iluminacionPrendida && apagado<=hourRT){
    iluminacionPrendida = false;
  }
  //Si la hora es mayor a la de encendido, se prende
  if(prendido<=hourRT){
    iluminacionPrendida = true;
  }
  return iluminacionPrendida;
}

