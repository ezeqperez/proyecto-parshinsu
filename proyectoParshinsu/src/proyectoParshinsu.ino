#include <Wire.h>
#include <DHT11.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Parshinsu.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16,2);
//LiquidCrystal_I2C lcd(0x20,16,2);
//LiquidCrystal_I2C lcd(0x27,16,2);

int sVentilacion = 4;
int sCalor = 5;
int sLeds = 6;
int sLamparas = 7;
int sdiasRiego = 9;
int sensorth = 11;
DHT11 dht11(sensorth);

boolean riegoHecho = false;
int horaVieja; 
float temp, hum;


/*  Maneja los estados de la planta:
 *    int temperaturaVentilacion;
 *    int temperaturaCalefaccion;
 *    Calefaccion* calefaccion (NO IMPLEMENTADO);
 *    int humedad;
 *    int horaPrendido;
 *    int horaApagado;
 */
EstadoPlanta* estado = nuevoEstadoPlanta(26, 20, temperaturaVegetacion, 80, 5, 23);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.backlight();

  pinMode(sVentilacion, OUTPUT);
  pinMode(sLeds, OUTPUT);
  pinMode(sLamparas, OUTPUT);
  pinMode(sCalor, OUTPUT);
  pinMode(sdiasRiego, OUTPUT);

  //Low lo prende
  //High lo apaga

  //Inicializamos las cosas en apagado
  digitalWrite(sVentilacion, HIGH);
  digitalWrite(sLeds, HIGH);
  digitalWrite(sLamparas, HIGH);
  digitalWrite(sCalor, HIGH);
  digitalWrite(sdiasRiego, HIGH);
  leerHora();
  horaVieja = hourRT;
  
  //(minuto, hora, diaDeLaSemana)
  //Lunes = 1
  //Domingo = 7
  //setDateTime();
  //setearFecha(54, 20, 6);

}
void (*pseudoReset)(void)=0;

void loop() {
  leerHora();
  mostrarFecha();
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
  escribirDisplay(hum, temp);
  controldiasRiego();
  Serial.println();
  Serial.println();
  delay(30000);
  
  //Sacar esto si funciona mal el reset
  resetear();

}

void resetear(){
  if(horaVieja != hourRT){
       Serial.println("Se resetea"); 

    pseudoReset();  
  }
  horaVieja = hourRT;
}

void fechaConCeros(int hora, int minuto){
  if(hora<10){
    lcd.print("0");
    lcd.setCursor(9,0);
    lcd.print(hora);
    lcd.setCursor(10,0);
  }else{
    lcd.print(hora);
    lcd.setCursor(10,0);  
  }
  lcd.print(":");
  lcd.setCursor(11,0);
  if(minuto<10){
    lcd.print("0");
    lcd.setCursor(12,0);
    lcd.print(minuto);
  }else{
    lcd.print(minuto);  
  }
}


void escribirDisplay(int hum, int temp){
  lcd.setCursor(0, 0);
  lcd.print("Indobar ");
  lcd.setCursor(8,0);
  fechaConCeros(hourRT, minuteRT);

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.setCursor(2, 1);
  lcd.print(hum);
  lcd.setCursor(5, 1);
  lcd.print("T:");
  lcd.print(temp);
  }

//Se prende si la temperatura es menor a 20
void controlCalefaccion(int temp) {
  if (temp<estado->temperaturaCalefaccion) {
    digitalWrite(sCalor,LOW);
    } else {
    digitalWrite(sCalor,HIGH);
   }
  }

boolean prendePorTemperaturaVentilacion(int temp){
  if(temp>estado->temperaturaVentilacion){
    Serial.println("Temperatura mayor a 26 grados");
  }
}

boolean prendePorHumedad(int hum){
  if( hum>estado->humedad){
    Serial.println("Humedad mayor a 65");
    
  }
}

boolean prendePorCincoMin(){
  if(primerosCincoMinutos()){
    Serial.println("Son los primeros 5 minutos de la hora");
  }
}


boolean prendePorEnPunto(){
  if(horaEnPunto(13)){
    Serial.println("Son las 13 en punto");
  }
}


//Se prende si la temperatura es mayor a 26 o si la humedad es mayor a 65
void controlVentilacion(int temp, int hum) {
  if (temp>estado->temperaturaVentilacion || hum>estado->humedad ||// primerosCincoMinutos() || 
  horaEnPunto(13)) {

    //Para ver quien lo prende
    prendePorTemperaturaVentilacion(temp);
    prendePorHumedad(hum);
  //  prendePorCincoMin();
    prendePorEnPunto();
    
    digitalWrite(sVentilacion,LOW);
    Serial.println("Ventilacion Prendida");
    lcd.setCursor(10,1);
    lcd.print("V");
    
  } else {
    digitalWrite(sVentilacion,HIGH);
    Serial.println("Ventilacion Apagada");
    lcd.setCursor(10,1);
    lcd.print(" ");
  }

}

void controlLuces() {
  mostrarHorario();
  if(estado->horaPrendido<=hourRT && hourRT<estado->horaApagado){
 // Para dejarlas prendidas if(true){
      Serial.print("Las luces estan prendidas. Quedan ");
      Serial.print(estado->horaApagado - hourRT -1);
      Serial.print(":");
      Serial.print(60 - minuteRT);
      Serial.println(" horas de luz");
      digitalWrite(sLeds,LOW);
      digitalWrite(sLamparas,LOW);
      lcd.setCursor(12,1);
      lcd.print("L");
    } else {
      Serial.print("Las luces están apagadas.");
      digitalWrite(sLamparas,HIGH);
      digitalWrite(sLeds,HIGH);
      lcd.setCursor(12,1);
      lcd.print(" ");
    }
}

void controldiasRiego(){
  /*
   * que sea el dia de la semana que yo quiero (diaSemana=1 es lunes, diaSemana=7 es domingo)
   * que sea la hora que quiera (y en el IF de abajo agregar una hora mas)
   */
  if(esUnDiaDeRiego() && hourRT == 20 &&!riegoHecho){
    Serial.println("Comienza el riego...");
    digitalWrite(sdiasRiego, LOW);
    imprimirPuntos();
    digitalWrite(sdiasRiego, HIGH);
    Serial.println("Riego finalizado!");
    riegoHecho = true;
  }
  if(hourRT == 19){
    riegoHecho = false;
  }
}

boolean esUnDiaDeRiego(){
  return (diaSemana == 1 || diaSemana == 4 || diaSemana == 6);
}

void imprimirPuntos(){
    //Tarda 40 segundos
    delay(5000);
    Serial.print(".");
    delay(5000);
    Serial.print(".");
    delay(5000);
    Serial.print(".");
    delay(5000);
    Serial.print(".");
    delay(5000);
    Serial.print(".");
    delay(5000);
    Serial.print(".");
    delay(5000);
    Serial.print(".");
    delay(5000);
    Serial.print(".");
    Serial.println(".");
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

/*
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

*/
void setDateTime(){

  byte second =      30; //0-59
  byte minute =      01; //0-59
  byte hour =        12; //0-23
  byte weekDay =     7; // lunes = 1   - 7=domingo
  byte monthDay =    01; //1-31
  byte month =       10; //1-12
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

