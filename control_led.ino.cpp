/*
 * Control de tres LEDs desde una página web mediante Web Serial API.
 * Conexiones correctas: LED rojo en pin 8, LED azul en pin 9, LED verde en pin 10.
 * Los pines 9 y 10 admiten PWM. El pin 8 es digital y solo actúa como ON/OFF.
 * Comandos: LED1:valor, LED2:valor, LED3:valor, MASTER:valor,
 * BLINK:0/1 y SPEED:milisegundos.
 */
const int LED_ROJO = 8;
const int LED_AZUL = 9;
const int LED_VERDE = 10;
const int pinesLED[] = {LED_ROJO, LED_AZUL, LED_VERDE};
int nivelesLED[] = {128, 128, 128};
int brilloGeneral = 255;
bool parpadeoActivo = false;
bool estadoParpadeo = true;
unsigned long tiempoAnterior = 0;
unsigned long intervaloParpadeo = 500;
String comando = "";

void aplicarBrillo() {
  for (int i = 0; i < 3; i++) {
    int salida = (parpadeoActivo && !estadoParpadeo) ? 0 : nivelesLED[i];
    salida = (salida * brilloGeneral) / 255;

    if (i == 0) {
      digitalWrite(pinesLED[i], salida > 0 ? HIGH : LOW);
    } else {
      analogWrite(pinesLED[i], salida);
    }
  }
}

void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(pinesLED[i], OUTPUT);
    digitalWrite(pinesLED[i], LOW);
  }
  Serial.begin(9600);
  Serial.println("ARDUINO_LISTO");
}

void loop() {
  if (Serial.available() > 0) {
    comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando.startsWith("LED1:")) {
      nivelesLED[0] = constrain(comando.substring(5).toInt(), 0, 255);
    } else if (comando.startsWith("LED2:")) {
      nivelesLED[1] = constrain(comando.substring(5).toInt(), 0, 255);
    } else if (comando.startsWith("LED3:")) {
      nivelesLED[2] = constrain(comando.substring(5).toInt(), 0, 255);
    } else if (comando.startsWith("MASTER:")) {
      brilloGeneral = constrain(comando.substring(7).toInt(), 0, 255);
    } else if (comando == "ALLON") {
      for (int i = 0; i < 3; i++) {
        if (nivelesLED[i] == 0) nivelesLED[i] = 128;
      }
    } else if (comando == "ALLOFF") {
      for (int i = 0; i < 3; i++) nivelesLED[i] = 0;
    } else if (comando.startsWith("BLINK:")) {
      parpadeoActivo = comando.substring(6).toInt() == 1;
      estadoParpadeo = true;
      tiempoAnterior = millis();
    } else if (comando.startsWith("SPEED:")) {
      intervaloParpadeo = constrain(comando.substring(6).toInt(), 100, 1500);
    } else if (comando == "ON1" || comando == "ON2" || comando == "ON3") {
      int indice = comando.charAt(2) - '1';
      nivelesLED[indice] = 128;
    } else if (comando == "OFF1" || comando == "OFF2" || comando == "OFF3") {
      int indice = comando.charAt(3) - '1';
      nivelesLED[indice] = 0;
    }

    aplicarBrillo();
  }

  if (parpadeoActivo && millis() - tiempoAnterior >= intervaloParpadeo) {
    tiempoAnterior = millis();
    estadoParpadeo = !estadoParpadeo;
    aplicarBrillo();
  }
}
