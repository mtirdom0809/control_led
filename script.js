/* Comunicación con Arduino mediante Web Serial API. */
let port;
let writer;
let contadorEncendidos = 0;
let blinkActivo = false;
const niveles = { 1: 128, 2: 128, 3: 128 };
const ultimoNivel = { 1: 128, 2: 128, 3: 128 };

const statusEl = document.getElementById('status');
const btnConnect = document.getElementById('btnConnect');
const contadorEl = document.getElementById('contador');
const slidersLed = [1, 2, 3].map(id => document.getElementById(`sliderLED${id}`));
const slidersMaster = document.getElementById('sliderMaster');
const sliderBlink = document.getElementById('sliderBlink');
const btnBlink = document.getElementById('btnBlink');
const controlButtons = document.querySelectorAll('[data-cmd]');

if ('serial' in navigator) {
    btnConnect.addEventListener('click', connectArduino);
} else {
    setStatus('Web Serial no está disponible. Abre esta página en Chrome o Edge de escritorio.', 'error');
    btnConnect.disabled = true;
}

async function connectArduino() {
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 9600 });
        const encoder = new TextEncoderStream();
        encoder.readable.pipeTo(port.writable);
        writer = encoder.writable.getWriter();

        setStatus('Conectado a Arduino', 'connected');
        btnConnect.textContent = 'Conectado';
        btnConnect.disabled = true;
        [...slidersLed, slidersMaster, sliderBlink].forEach(slider => slider.disabled = false);
        controlButtons.forEach(button => button.disabled = false);
        bindControls();
        leerRespuestasArduino();
    } catch (error) {
        console.error('Error de conexión:', error);
        setStatus('No se pudo conectar. Comprueba el puerto y vuelve a intentarlo.', 'error');
    }
}

function setStatus(message, state) {
    statusEl.textContent = message;
    statusEl.dataset.state = state;
}

function bindControls() {
    slidersLed.forEach((slider, index) => {
        const id = index + 1;
        slider.addEventListener('input', () => {
            const value = Number(slider.value);
            document.getElementById(`valorLED${id}`).textContent = value;
            if (value > 0 && niveles[id] === 0) {
                contadorEncendidos++;
                actualizarContador();
            }
            niveles[id] = value;
            if (value > 0) ultimoNivel[id] = value;
            enviarComando(`LED${id}:${value}`);
        });
    });

    slidersMaster.addEventListener('input', () => {
        document.getElementById('valorMaster').textContent = slidersMaster.value;
        enviarComando(`MASTER:${slidersMaster.value}`);
    });

    sliderBlink.addEventListener('input', () => {
        document.getElementById('valorBlink').textContent = `${sliderBlink.value} ms`;
        enviarComando(`SPEED:${sliderBlink.value}`);
    });

    controlButtons.forEach(button => button.addEventListener('click', async () => {
        const command = button.dataset.cmd;
        await enviarComando(command);
        if (command === 'ALLON') {
            [1, 2, 3].forEach(id => {
                if (niveles[id] === 0) contadorEncendidos++;
                niveles[id] = ultimoNivel[id];
                slidersLed[id - 1].value = ultimoNivel[id];
                document.getElementById(`valorLED${id}`).textContent = ultimoNivel[id];
            });
            actualizarContador();
        } else if (command === 'ALLOFF') {
            [1, 2, 3].forEach(id => {
                if (niveles[id] > 0) ultimoNivel[id] = niveles[id];
                niveles[id] = 0;
                slidersLed[id - 1].value = 0;
                document.getElementById(`valorLED${id}`).textContent = '0';
                enviarComando(`LED${id}:0`);
            });
        }
    }));

    btnBlink.addEventListener('click', async () => {
        blinkActivo = !blinkActivo;
        await enviarComando(`BLINK:${blinkActivo ? 1 : 0}`);
        btnBlink.textContent = blinkActivo ? 'Detener parpadeo' : 'Activar parpadeo';
        btnBlink.classList.toggle('is-active', blinkActivo);
    });
}

async function enviarComando(command) {
    if (!writer) return;
    try {
        await writer.write(`${command}\n`);
    } catch (error) {
        console.error('No se pudo enviar el comando:', error);
        setStatus('Se perdió la comunicación con Arduino.', 'error');
    }
}

function actualizarContador() {
    contadorEl.textContent = contadorEncendidos;
}

async function leerRespuestasArduino() {
    const decoder = new TextDecoderStream();
    port.readable.pipeTo(decoder.writable);
    const reader = decoder.readable.getReader();
    try {
        while (true) {
            const { value, done } = await reader.read();
            if (done) break;
            if (value) console.log('Arduino:', value.trim());
        }
    } catch (error) {
        console.error('Error leyendo datos:', error);
    } finally {
        reader.releaseLock();
    }
}
