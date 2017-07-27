#include <DS1302.h>
#include <Wire.h>  
DS1302 rtc(7, 8, 9);
#include <DHT11.h>
Time time;
const int ventilacion = 4;
const int luces = 5;
const int dacalor = 6;

int sensorth = 11;
DHT11 dht11(sensorth);



void setup()
{
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  Serial.begin(9600);

  pinMode(ventilacion, OUTPUT);
  pinMode(luces, OUTPUT);
  pinMode(dacalor, OUTPUT);

  //Inicializamos las cosas en apagado
  digitalWrite(ventilacion, LOW);
  digitalWrite(luces, LOW);
  digitalWrite(dacalor, LOW);

  
  /*
   * Solo se setea una vez con el horario actual
  rtc.setDOW(MONDAY);        // Set Day-of-Week to FRIDAY
  rtc.setTime(21, 35, 40);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(24, 7, 2017);   // Set the date to August 6th, 2010
  */
}

void loop()
{
  float temp, hum;

  //Muestro por pantalla cada elemento del tiempo por separado
  time = rtc.getTime();
  Serial.print(" Año: ");
  Serial.print(time.year, DEC);
  Serial.print(" Mes: ");
  Serial.print(time.mon, DEC);  
  Serial.print(" Dia: ");
  Serial.print(time.date, DEC);  
  Serial.println();
  Serial.print(" Hora: ");
  Serial.print(time.hour, DEC);
  Serial.print(" Minutos: ");
  Serial.print(time.min, DEC);  
  Serial.print(" Segundos: ");
  Serial.print(time.sec, DEC);
  Serial.println();

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
  delay(1000); 

  controlCalefaccion(temp);
  controlVentilacion(temp, hum);
  controlLuces();
}  

void controlCalefaccion(int temperatura){
  if(temperatura < 20){
    digitalWrite(dacalor, HIGH);
  }
}

void controlVentilacion(int temperatura, int humedad){
  if (temperatura > 26 || humedad > 65){
    digitalWrite(ventilacion, HIGH);
  }
}

void controlLuces(){
  if(time.hour>=06 && time.hour<24){
    digitalWrite(luces, HIGH);
  }else{
    digitalWrite(luces, LOW);
  }
}
