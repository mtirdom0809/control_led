/*
 * =====================================================
 * PROYECTO: Control de LEDs desde Web mediante Web Serial API
 * PLACA: ELEGOO UNO R3
 * =====================================================
 * 
 * PINES UTILIZADOS:
 * - Pin 8:  LED 1 (Rojo)  -> Encendido/Apagado digital
 * - Pin 9:  LED 2 (Azul)  -> Encendido/Apagado + PWM (luminosidad)
 * - Pin 10: LED 3 (Verde) -> Encendido/Apagado digital
 * 
 * COMANDOS QUE RECIBE POR PUERTO SERIE:
 * - "ON1" / "OFF1"  -> LED 1
 * - "ON2" / "OFF2"  -> LED 2
 * - "ON3" / "OFF3"  -> LED 3
 * - "ALLON" / "ALLOFF" -> Todos los LEDs
 * - "BLINK"         -> Activa/desactiva parpadeo del LED 1
 * - "PWM:0-255"     -> Regula luminosidad del LED 2 (PWM)
 * =====================================================
 */

// Definición de pines de los LEDs
const int LED1 = 8;   // LED 1 - solo digital
const int LED2 = 9;   // LED 2 - soporta PWM (pin con ~)
const int LED3 = 10;  // LED 3 - solo digital

// Variables para el control del parpadeo (BLINK)
bool parpadeoActivo = false;          // Indica si el modo parpadeo está activo
bool estadoLed1 = false;              // Estado actual del LED 1 durante el parpadeo
unsigned long tiempoAnterior = 0;     // Guarda el último momento en que cambió el LED
const long intervaloParpadeo = 500;   // Intervalo de parpadeo en milisegundos (0.5s)

// Variable que almacena el comando recibido por serie
String comando = "";

void setup() {
  // Configuramos los pines como salidas
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // Nos aseguramos de que todos los LEDs empiecen apagados
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  // Iniciamos la comunicación serie a 9600 baudios
  // IMPORTANTE: debe coincidir con la velocidad del navegador
  Serial.begin(9600);
  
  // Mensaje de bienvenida que se envía al conectarse
  Serial.println("ARDUINO_LISTO");
}

void loop() {
  // ==========================================
  // 1) LEER COMANDOS ENTRANTES DESDE LA WEB
  // ==========================================
  if (Serial.available() > 0) {
    // Leemos todo hasta el salto de línea (\n)
    comando = Serial.readStringUntil('\n');
    comando.trim(); // Eliminamos espacios o saltos de línea invisibles

    // ---------- CONTROL LED 1 ----------
    if (comando == "ON1") {
      digitalWrite(LED1, HIGH);
      Serial.println("OK:LED1_ON");
    } 
    else if (comando == "OFF1") {
      digitalWrite(LED1, LOW);
      Serial.println("OK:LED1_OFF");
    }
    // ---------- CONTROL LED 2 ----------
    else if (comando == "ON2") {
      digitalWrite(LED2, HIGH);
      Serial.println("OK:LED2_ON");
    } 
    else if (comando == "OFF2") {
      digitalWrite(LED2, LOW);
      Serial.println("OK:LED2_OFF");
    }
    // ---------- CONTROL LED 3 ----------
    else if (comando == "ON3") {
      digitalWrite(LED3, HIGH);
      Serial.println("OK:LED3_ON");
    } 
    else if (comando == "OFF3") {
      digitalWrite(LED3, LOW);
      Serial.println("OK:LED3_OFF");
    }
    // ---------- TODOS LOS LEDs ----------
    else if (comando == "ALLON") {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      Serial.println("OK:TODOS_ON");
    } 
    else if (comando == "ALLOFF") {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      Serial.println("OK:TODOS_OFF");
    }
    // ---------- PARPADEO (toggle) ----------
    else if (comando == "BLINK") {
      parpadeoActivo = !parpadeoActivo; // Cambia entre true/false
      if (!parpadeoActivo) {
        // Si desactivamos el parpadeo, apagamos el LED 1
        digitalWrite(LED1, LOW);
      }
      Serial.println("OK:BLINK_" + String(parpadeoActivo ? "ON" : "OFF"));
    }
    // ---------- PWM (luminosidad LED 2) ----------
    // Formato esperado: "PWM:150" (valor entre 0 y 255)
    else if (comando.startsWith("PWM:")) {
      String valorStr = comando.substring(4); // Quitamos "PWM:"
      int valor = valorStr.toInt();           // Convertimos a número entero
      
      // Limitamos el valor al rango válido de PWM (0-255)
      if (valor < 0) valor = 0;
      if (valor > 255) valor = 255;
      
      analogWrite(LED2, valor); // Escribimos el valor PWM en el pin 9
      Serial.println("OK:PWM_" + String(valor));
    }
  }

  // ==========================================
  // 2) LÓGICA DE PARPADEO (no bloqueante)
  // ==========================================
  // Usamos millis() en lugar de delay() para que no se congele el programa
  if (parpadeoActivo) {
    unsigned long tiempoActual = millis();
    // Si ha pasado el intervalo, cambiamos el estado del LED
    if (tiempoActual - tiempoAnterior >= intervaloParpadeo) {
      tiempoAnterior = tiempoActual;       // Reiniciamos el contador de tiempo
      estadoLed1 = !estadoLed1;            // Invertimos el estado (true<->false)
      digitalWrite(LED1, estadoLed1);      // Aplicamos el nuevo estado
    }
  }
}