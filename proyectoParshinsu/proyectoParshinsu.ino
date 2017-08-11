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
  //Estos creo que no van, borrar cuando este seguro
  //setTime(hr,min,sec,day,month,yr);
  //setTime(23,37,0,2,8,2017);
  
  
  //Solo voy a setear el tiempo cuando sea necesario
  //setDateTime();
}

void loop() {
  float temp, hum;
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
  controldiasRiego();
  escribirDisplay(hum, temp);
  controlLuces();
  Serial.println();
  Serial.println();
  delay(30000);
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
    lcd.setCursor(10,1);
    lcd.print("V");
  } else {
    digitalWrite(sCalor,HIGH);
  }
  lcd.setCursor(12,1);
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
    
  } else {
    digitalWrite(sVentilacion,HIGH);
    Serial.println("Ventilacion Apagada");
  }

}

void controlLuces() {
  mostrarHorario();
  if(estado->horaPrendido<=hourRT && hourRT<estado->horaApagado){
 // Para dejarlas prendidas if(true){
      lcd.setCursor(12,1);
      lcd.print("L");
      Serial.print("Las luces estan prendidas. Quedan ");
      Serial.print(estado->horaApagado - hourRT -1);
      Serial.print(":");
      Serial.print(60 - minuteRT);
      Serial.println(" horas de luz");
      digitalWrite(sLeds,LOW);
      digitalWrite(sLamparas,LOW);
    } else {
      Serial.print("Las luces están apagadas.");
      digitalWrite(sLamparas,HIGH);
      digitalWrite(sLeds,HIGH);
    }
}

void controldiasRiego(){
  /*
   * que sea el dia de la semana que yo quiero (diaSemana=1 es lunes, diaSemana=7 es domingo)
   * que sea la hora que quiera (y en el IF de abajo agregar una hora mas)
   */
  if(diaSemana==5 && hourRT == 22 &&!riegoHecho){
    Serial.println("Comienza el riego...");
    digitalWrite(sdiasRiego, LOW);
    imprimirPuntos();
    digitalWrite(sdiasRiego, HIGH);
    Serial.println("Riego finalizado!");
    riegoHecho = true;
  }
  if(hourRT == 23){
    riegoHecho = false;
  }
}

void imprimirPuntos(){
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
