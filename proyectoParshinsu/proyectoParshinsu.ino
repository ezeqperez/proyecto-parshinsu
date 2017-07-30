#include <Wire.h>  
#include <DHT11.h>
const int ventilacion = 4;
const int dacalor = 5;
const int luces = 6;
int led = 13;
int sensorth = 11;
DHT11 dht11(sensorth);

void setup()
{
  // Set the clock to run-mode, and disable the write protection
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(ventilacion, OUTPUT);
  pinMode(luces, OUTPUT);
  pinMode(dacalor, OUTPUT);

  //Low lo prende
  //High lo apaga

  //Inicializamos las cosas en apagado
  digitalWrite(ventilacion, HIGH);
  digitalWrite(luces, HIGH);
  digitalWrite(dacalor, HIGH);

  
  /*
   * Solo se setea una vez con el horario actual
  rtc.setDOW(MONDAY);        // Set Day-of-Week to FRIDAY
  rtc.setTime(21, 35, 40);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(24, 7, 2017);   // Set the date to August 6th, 2010
  */
}

void loop(){
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  delay(1000);
  float temp, hum;
  //Si es 0, leyo ok
  if (dht11.read(hum, temp) == 0) 
  {
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print(" Humedad: ");
    Serial.print(hum);
    Serial.println();
  }
  else
  {
    Serial.println();
    Serial.print("No se pudo leer el sensor :");
    Serial.println();
  }
 
  controlCalefaccion(temp);
  controlVentilacion(temp, hum);
  }  

//Se prende si la temperatura es menor a 20
void controlCalefaccion(int temp){
  if(temp < 20){
    digitalWrite(dacalor, LOW);
  }else{
    digitalWrite(dacalor, HIGH);
  }
}

//Se prende si la temperatura es mayor a 26 o si la humedad es mayor a 65
void controlVentilacion(int temp, int hum){
  if (temp > 26 || hum > 80){
    digitalWrite(ventilacion, LOW);
  }else{
    digitalWrite(ventilacion, HIGH);
  }
  
}


