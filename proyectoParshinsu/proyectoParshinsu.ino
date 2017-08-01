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
//Revisar  if (estado->horaPrendido<=(horaParaComparar(tm.Hour,(false))) && (tm.Hour <= horaParaComparar(estado->horaApagado, (estado->horaApagado-tm.Hour)>=0)) ) {
if(tm.Hour>=06 && tm.Hour<24){
    Serial.print("Se prenden las luces, son las ");
    mostrarFecha();
    Serial.println(tm.Hour);

        Serial.println(estado->horaPrendido);
    Serial.print(" Tiene que ser menor a ");
    Serial.print(horaParaComparar(tm.Hour,(false)));
    Serial.println();
    print2digits(tm.Hour);
    Serial.print(" menor a ");
    Serial.println( horaParaComparar(estado->horaApagado, (estado->horaApagado-tm.Hour)>=0));
    //prenderRele(leds);
    //prenderRele(lamparas);

    digitalWrite(leds,LOW);
    digitalWrite(lamparas,LOW);
  } else {
    Serial.println("Luces Apagadas, son las ");
    print2digits(tm.Hour);
    
        Serial.println(horaParaComparar(tm.Hour,(estado->horaPrendido-tm.Hour)>=0));
    Serial.print(" Tiene que ser mayor a ");
    Serial.println(estado->horaPrendido);
    Serial.println();
    print2digits(tm.Hour);
    Serial.print(" menor a ");
    Serial.println( horaParaComparar(estado->horaApagado, (estado->horaApagado-tm.Hour)>=0));
//    apagarRele(leds);
//    apagarRele(lamparas);
    digitalWrite(lamparas,HIGH);
    digitalWrite(leds,HIGH);
  }
}

int horaParaComparar(int hora, bool seHace){
  int aComparar = hora+12;
  if(aComparar>=24 && seHace){
    return hora;
  }else{
  return (hora+24);
  }
}




