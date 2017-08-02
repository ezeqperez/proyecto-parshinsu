#include <Wire.h>
#include <DHT11.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Parshinsu.h>

int sVentilacion = 4;
int sCalor = 5;
int sLeds = 6;
int sLamparas = 7;
int sdiasRiego = 7;
int sensorth = 11;
DHT11 dht11(sensorth);

boolean riegoHecho = false;


EstadoPlanta* estado = vegetacion;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  pinMode(sVentilacion, OUTPUT);
  pinMode(sLeds, OUTPUT);
  pinMode(sLamparas, OUTPUT);
  pinMode(sCalor, OUTPUT);

  //Low lo prende
  //High lo apaga

  //Inicializamos las cosas en apagado
  digitalWrite(sVentilacion, HIGH);
  digitalWrite(sLeds, HIGH);
  digitalWrite(sLamparas, HIGH);
  digitalWrite(sCalor, HIGH);
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
  controlsVentilacion(temp, hum);
  controlLuces();
  controldiasRiego();
  Serial.println();
  Serial.println();
  delay(60000);
}

//Se prende si la temperatura es menor a 20
void controlCalefaccion(int temp) {
  if (temp<estado->temperaturaCalefaccion) {
    digitalWrite(sCalor,LOW);
  } else {
    digitalWrite(sCalor,HIGH);
  }
}

//Se prende si la temperatura es mayor a 26 o si la humedad es mayor a 65
void controlsVentilacion(int temp, int hum) {
  if (temp>estado->temperaturaVentilacion || hum>estado->humedad || primerosCincoMinutos() || horaEnPunto(13)) {
    digitalWrite(sVentilacion,LOW);
    
  } else {
    digitalWrite(sVentilacion,HIGH);
  }

}

void controlLuces() {
  mostrarHorario();
 // if(dentroDeLaDuracion(estado->horaPrendido, estado->horaApagado)){
 if(estado->horaPrendido<=hourRT && hourRT<estado->horaApagado){
      Serial.print("Las luces estan prendidas. Quedan ");
      Serial.print(estado->horaApagado - hourRT);
      Serial.print(":");
      Serial.print(minuteRT);
      Serial.println(" horas de luz");
      digitalWrite(sLeds,LOW);
      digitalWrite(sLamparas,LOW);
    } else {
      Serial.print("Las luces están apagadas");
      digitalWrite(sLamparas,HIGH);
      digitalWrite(sLeds,HIGH);
    }
}

void controldiasRiego(){
  if(diaSemana==3 && hourRT == 12 &&!riegoHecho){
    digitalWrite(sdiasRiego, LOW);
    delay(7000);
    digitalWrite(sdiasRiego, HIGH);
    riegoHecho = true;
  }
  if(hourRT == 13){
    riegoHecho = false;
  }
}

void mostrarHorario(){
      Serial.print("Son las ");
      Serial.print(hourRT);
      Serial.print("hs, Horario de prendido: ");
      Serial.print(estado->horaPrendido);
      Serial.print("hs Horario de apagado: ");
      Serial.print(estado->horaApagado);
      Serial.println("hs");
}


boolean dentroDeLaDuracion(int prendido, int apagado){
  //Analizar el caso de las 23 y las 0
 // Serial.println(prendido);
 // Serial.println(apagado);

  

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

