#  Control de LEDs desde Web - Arduino + Web Serial API

Proyecto educativo que permite controlar LEDs físicos conectados a una placa **ELEGOO UNO R3** desde una página web usando la **Web Serial API**.

##  Objetivo
Comunicación bidireccional entre un navegador web (Chrome/Edge) y Arduino mediante cable USB.

##  Funcionalidades
- ✅ Control individual de 3 LEDs (pines 8, 9 y 10)
- ✅ Botón para encender/apagar todos los LEDs a la vez
- ✅ Modo parpadeo no bloqueante con `millis()`
- ✅ Control de luminosidad mediante PWM (slider)
- ✅ Contador de encendidos en tiempo real
- ✅ Confirmación de comandos desde Arduino

##  Material
- Arduino UNO R3 (ELEGOO)
- 3 LEDs (rojo, azul, verde)
- 3 resistencias de 220Ω o 330Ω
- Protoboard y cables dupont
- Cable USB

##  Conexiones
| LED | Color | Pin |
|-----|-------|-----|
| LED 1 | 🔴 Rojo | 8 |
| LED 2 | 🔵 Azul | 9 (PWM) |
| LED 3 | 🟢 Verde | 10 |

##  Cómo usarlo
1. Cargar `control_led.ino` en Arduino IDE
2. Cerrar el Monitor Serie
3. Abrir `index.html` con Live Server o subirlo a GitHub Pages
4. Pulsar "Conectar Arduino" y seleccionar el puerto
5. ¡Disfruta controlando los LEDs!

##  Nota importante
La Web Serial API **solo funciona** en Chrome y Edge de escritorio. No funciona en móviles ni en Firefox/Safari.

##  Autores
Mirian Tirado y Daiana Ruiz - Noviembre 2026
