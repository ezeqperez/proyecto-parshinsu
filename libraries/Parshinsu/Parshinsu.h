#define Parshinsu
#define DS1307_ADDRESS 0x68
byte zero = 0x00; 

#include <Wire.h>  
#include <DHT11.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <stdlib.h>

tmElements_t tm;

typedef struct EstadoPlanta EstadoPlanta;

struct EstadoPlanta
{
    int temperatura;
    int humedad;
    int horaPrendido;
    int horaApagado;
    int temperaturaVentilacion;
    int temperaturaCalefaccion;
};

struct EstadoPlanta* nuevoEstadoPlanta(int temperaturaVentilacion, int temperaturaCalefaccion, int humedad, int horaPrendido, int horaApagado)
{
    // Aquí requerimos la función "malloc" para reservar la memoria para el nuevo objeto
    struct EstadoPlanta* estado = malloc(sizeof(struct EstadoPlanta));

    estado->temperaturaVentilacion = temperaturaVentilacion;
    estado->temperaturaCalefaccion = temperaturaCalefaccion;
    estado->humedad = humedad;
    estado->horaPrendido = horaPrendido;
    estado->horaApagado = horaApagado;

    return estado;
}

EstadoPlanta* vegetacion = nuevoEstadoPlanta(26, 20, 80, 06, 24);
//Reemplazar por los valores correctos
EstadoPlanta* floracion  = nuevoEstadoPlanta(26, 20, 80, 06, 24);


void prenderRele(int puerto){ digitalWrite(puerto, HIGH);}
void apagarRele(int puerto) { digitalWrite(puerto, HIGH);}

	/*
	*
	*	Abajo está el código que sacamos de internet
	*	Tocar a concienca
	*
	*/
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