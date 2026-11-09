/*
 * LEDs: rojo en pin 8, azul en pin 9 y verde en pin 10.
 * El pin 8 es digital; los pines 9 y 10 permiten PWM.
 * Comandos: LED1:valor, LED2:valor, LED3:valor, MASTER:valor,
 * BLINK:0/1 (también se acepta BLINK para alternar), SPEED:milisegundos.
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

    // El pin 8 no admite PWM: se conmuta digitalmente; 9 y 10 usan PWM.
    if (i == 0) {
      digitalWrite(pinesLED[i], salida > 0 ? HIGH : LOW);
    } else {
      analogWrite(pinesLED[i], salida);
    }
  }
}

void procesarComando(String recibido) {
  recibido.trim();
  if (recibido.length() == 0) return;

  if (recibido.startsWith("LED1:")) {
    nivelesLED[0] = constrain(recibido.substring(5).toInt(), 0, 255);
  } else if (recibido.startsWith("LED2:")) {
    nivelesLED[1] = constrain(recibido.substring(5).toInt(), 0, 255);
  } else if (recibido.startsWith("LED3:")) {
    nivelesLED[2] = constrain(recibido.substring(5).toInt(), 0, 255);
  } else if (recibido.startsWith("MASTER:")) {
    brilloGeneral = constrain(recibido.substring(7).toInt(), 0, 255);
  } else if (recibido == "ALLON") {
    for (int i = 0; i < 3; i++) {
      if (nivelesLED[i] == 0) nivelesLED[i] = 128;
    }
  } else if (recibido == "ALLOFF") {
    for (int i = 0; i < 3; i++) nivelesLED[i] = 0;
  } else if (recibido == "BLINK") {
    parpadeoActivo = !parpadeoActivo;
    estadoParpadeo = true;
    tiempoAnterior = millis();
  } else if (recibido.startsWith("BLINK:")) {
    parpadeoActivo = recibido.substring(6).toInt() == 1;
    estadoParpadeo = true;
    tiempoAnterior = millis();
  } else if (recibido.startsWith("SPEED:")) {
    intervaloParpadeo = constrain(recibido.substring(6).toInt(), 100, 1500);
  } else if (recibido == "ON1" || recibido == "ON2" || recibido == "ON3") {
    int indice = recibido.charAt(2) - '1';
    nivelesLED[indice] = 128;
  } else if (recibido == "OFF1" || recibido == "OFF2" || recibido == "OFF3") {
    int indice = recibido.charAt(3) - '1';
    nivelesLED[indice] = 0;
  }

  aplicarBrillo();
}

void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(pinesLED[i], OUTPUT);
    digitalWrite(pinesLED[i], LOW);
  }
  Serial.begin(9600);
  // Reducir el bloqueo si una transmisión se interrumpe antes del salto de línea.
  Serial.setTimeout(25);
  Serial.println("ARDUINO_LISTO");
}

void loop() {
  while (Serial.available() > 0) {
    comando = Serial.readStringUntil('\n');
    procesarComando(comando);
  }

  if (parpadeoActivo && millis() - tiempoAnterior >= intervaloParpadeo) {
    tiempoAnterior = millis();
    estadoParpadeo = !estadoParpadeo;
    aplicarBrillo();
  }
}
