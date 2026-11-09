/* ============================================
 * SCRIPT PRINCIPAL - CONTROL LED ARDUINO
 * Usa la Web Serial API para comunicarse con Arduino
 * Compatible con: Chrome y Edge (escritorio)
 * ============================================ */

// ==========================================
// VARIABLES GLOBALES
// ==========================================
let port;           // Puerto serie conectado
let writer;         // Escritor para enviar datos a Arduino
let contadorEncendidos = 0;  // Contador de veces que se ha encendido cualquier LED

// ==========================================
// REFERENCIAS A ELEMENTOS DEL DOM
// ==========================================
const statusEl   = document.getElementById('status');
const btnConnect = document.getElementById('btnConnect');
const contadorEl = document.getElementById('contador');
const sliderPWM  = document.getElementById('sliderPWM');
const valorPWM   = document.getElementById('valorPWM');

// ==========================================
// VERIFICAR SOPORTE DEL NAVEGADOR
// ==========================================
// La Web Serial API solo está disponible en Chrome/Edge de escritorio
if ('serial' in navigator) {
    // El navegador soporta la API → activamos el botón
    btnConnect.addEventListener('click', connectArduino);
    console.log("✅ Web Serial API disponible");
} else {
    // El navegador NO soporta la API (ej: móvil, Firefox, Safari)
    statusEl.innerText = "❌ Tu navegador no soporta Web Serial API. Usa Chrome o Edge en PC.";
    statusEl.style.color = "red";
    btnConnect.disabled = true;
    btnConnect.innerText = "Navegador no compatible";
}

// ==========================================
// FUNCIÓN: CONECTAR CON ARDUINO
// ==========================================
async function connectArduino() {
    try {
        // 1) Abrimos el selector de puertos del navegador
        port = await navigator.serial.requestPort();
        
        // 2) Abrimos el puerto a 9600 baudios (DEBE coincidir con Arduino)
        await port.open({ baudRate: 9600 });
        
        // 3) Configuramos el escritor para enviar texto a Arduino
        // TextEncoderStream convierte texto → bytes automáticamente
        const textEncoder = new TextEncoderStream();
        const writableStreamClosed = textEncoder.readable.pipeTo(port.writable);
        writer = textEncoder.writable.getWriter();

        // 4) Actualizamos la interfaz: conexión exitosa
        statusEl.innerText = "🟢 ¡Conectado a Arduino!";
        statusEl.style.color = "green";
        btnConnect.innerText = "✅ Conectado";
        btnConnect.disabled = true;

        // 5) Habilitamos todos los botones de control
        habilitarBotones(true);

        // 6) Activamos el slider de PWM
        sliderPWM.disabled = false;

        // 7) Asignamos eventos a todos los botones con atributo data-cmd
        asignarEventosBotones();

        // 8) Asignamos evento al slider de PWM
        sliderPWM.addEventListener('input', manejarSlider);

        // 9) Empezamos a leer las respuestas de Arduino
        leerRespuestasArduino();

    } catch (err) {
        // Si el usuario cancela o hay un error
        console.error("Error de conexión:", err);
        statusEl.innerText = "❌ Error al conectar. ¿Seleccionaste el puerto?";
        statusEl.style.color = "red";
    }
}

// ==========================================
// FUNCIÓN: HABILITAR/DESHABILITAR BOTONES
// ==========================================
function habilitarBotones(habilitar) {
    // Seleccionamos todos los botones que tengan el atributo data-cmd
    const botones = document.querySelectorAll('[data-cmd]');
    botones.forEach(btn => {
        btn.disabled = !habilitar;
    });
}

// ==========================================
// FUNCIÓN: ASIGNAR EVENTOS A LOS BOTONES
// ==========================================
function asignarEventosBotones() {
    const botones = document.querySelectorAll('[data-cmd]');
    botones.forEach(btn => {
        btn.addEventListener('click', () => {
            const comando = btn.getAttribute('data-cmd');
            enviarComando(comando);
            
            // Si es un comando de encendido, incrementamos el contador
            if (comando === 'ON1' || comando === 'ON2' || comando === 'ON3' || comando === 'ALLON') {
                contadorEncendidos++;
                actualizarContador();
            }
        });
    });
}

// ==========================================
// FUNCIÓN: ENVIAR COMANDO A ARDUINO
// ==========================================
async function enviarComando(cmd) {
    if (writer) {
        // Añadimos \n al final para que Arduino lo detecte con readStringUntil('\n')
        await writer.write(cmd + '\n');
        console.log("📤 Enviado:", cmd);
    }
}

// ==========================================
// FUNCIÓN: MANEJAR EL SLIDER DE PWM
// ==========================================
function manejarSlider(e) {
    const valor = e.target.value;
    // Mostramos el valor actual al lado del slider
    valorPWM.innerText = valor;
    // Enviamos el comando PWM al Arduino
    enviarComando('PWM:' + valor);
}

// ==========================================
// FUNCIÓN: ACTUALIZAR CONTADOR EN PANTALLA
// ==========================================
function actualizarContador() {
    contadorEl.innerText = contadorEncendidos;
}

// ==========================================
// FUNCIÓN: LEER RESPUESTAS DE ARDUINO
// ==========================================
async function leerRespuestasArduino() {
    // TextDecoderStream convierte bytes → texto
    const textDecoder = new TextDecoderStream();
    const readableStreamClosed = port.readable.pipeTo(textDecoder.writable);
    const reader = textDecoder.readable.getReader();

    try {
        // Bucle infinito que lee mientras la conexión esté activa
        while (true) {
            const { value, done } = await reader.read();
            if (done) {
                // El puerto se cerró
                break;
            }
            if (value) {
                // Mostramos en consola lo que Arduino nos envía
                console.log("📥 Arduino dice:", value.trim());
            }
        }
    } catch (err) {
        console.error("Error leyendo datos:", err);
    } finally {
        // Liberamos el lector al finalizar
        reader.releaseLock();
    }
}