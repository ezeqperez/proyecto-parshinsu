#include <Wire.h>  
#include <DHT11.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define DS1307_ADDRESS 0x68
byte zero = 0x00; //workaround for issue #527
tmElements_t tm;

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
  Wire.begin();
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
//  setDateTime();
  
}

void loop(){

    mostrarFecha();


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
  controlLuces();
  delay(60000);
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

void controlLuces(){
   if(tm.Hour>=06 && tm.Hour<24){
     Serial.print("Luces Prendidas, son las "); 
     Serial.println(tm.Hour);
     digitalWrite(luces, LOW);
   }else{
    Serial.print("Luces Apagadas, son las ");
    Serial.println(tm.Hour);
     digitalWrite(luces, HIGH);
   }
 }

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void mostrarFecha(){
  RTC.read(tm);
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  
}


void setDateTime(){

  byte second =      00; //0-59
  byte minute =      59; //0-59
  byte hour =        19; //0-23
  byte weekDay =     7; //1-7
  byte monthDay =    29; //1-31
  byte month =       07; //1-12
  byte year  =       17; //0-99

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator

  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));

  Wire.write(zero); //start 

  Wire.endTransmission();

}

byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
  return ( (val/16*10) + (val%16) );
}

void printDate(){

  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  int second = bcdToDec(Wire.read());
  int minute = bcdToDec(Wire.read());
  int hour = bcdToDec(Wire.read() & 0b111111); //24 hour time
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int month = bcdToDec(Wire.read());
  int year = bcdToDec(Wire.read());

  //print the date EG   3/1/11 23:59:59
  Serial.print(month);
  Serial.print("/");
  Serial.print(monthDay);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

}

