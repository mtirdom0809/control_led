/*
 * Control de tres LEDs desde una página web mediante Web Serial API.
 * Para usar el control de brillo individual, conecta los LEDs a pines PWM.
 * En Arduino UNO se usan los pines 9, 10 y 11.
 * Comandos: LED1:valor, LED2:valor, LED3:valor, MASTER:valor,
 * BLINK:0/1 y SPEED:milisegundos.
 */
const int LED1 = 9;
const int LED2 = 10;
const int LED3 = 11;
const int pinesLED[] = {LED1, LED2, LED3};
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
    analogWrite(pinesLED[i], salida);
  }
}

void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(pinesLED[i], OUTPUT);
    analogWrite(pinesLED[i], 0);
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
    }

    aplicarBrillo();
  }

  if (parpadeoActivo && millis() - tiempoAnterior >= intervaloParpadeo) {
    tiempoAnterior = millis();
    estadoParpadeo = !estadoParpadeo;
    aplicarBrillo();
  }
}
