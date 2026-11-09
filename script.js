let port;
let writer;
let contadorEncendidos = 0;
let blinkActivo = false;
const niveles = { 1: 128, 2: 128, 3: 128 };
const ultimosNiveles = { 1: 128, 2: 128, 3: 128 };

const statusEl = document.getElementById('status');
const btnConnect = document.getElementById('btnConnect');
const contadorEl = document.getElementById('contador');
const sliderMaster = document.getElementById('sliderMaster');
const sliderBlink = document.getElementById('sliderBlink');
const btnBlink = document.getElementById('btnBlink');
const sliders = [1, 2, 3].map(id => document.getElementById(`sliderLED${id}`));

if ('serial' in navigator) {
    btnConnect.addEventListener('click', connectArduino);
} else {
    setStatus('Tu navegador no soporta Web Serial. Usa Chrome o Edge en PC.', 'error');
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
        document.querySelectorAll('[data-cmd]').forEach(button => button.disabled = false);
        [...sliders, sliderMaster, sliderBlink].forEach(slider => slider.disabled = false);
        bindControls();
        leerRespuestasArduino();
    } catch (error) {
        console.error('Error de conexión:', error);
        setStatus('Error al conectar. Comprueba el puerto seleccionado.', 'error');
    }
}

function setStatus(message, state) {
    statusEl.textContent = message;
    statusEl.dataset.state = state;
}

function updateCounter() {
    contadorEl.textContent = contadorEncendidos;
}

function setLevel(id, value, send = true) {
    const newValue = Number(value);
    if (newValue > 0 && niveles[id] === 0) {
        contadorEncendidos++;
        updateCounter();
    }
    if (newValue > 0) ultimosNiveles[id] = newValue;
    niveles[id] = newValue;
    sliders[id - 1].value = newValue;
    document.getElementById(`valorLED${id}`).textContent = newValue;
    if (send) enviarComando(`LED${id}:${newValue}`);
}

function bindControls() {
    sliders.forEach((slider, index) => {
        slider.addEventListener('input', () => setLevel(index + 1, slider.value));
    });

    sliderMaster.addEventListener('input', () => {
        document.getElementById('valorMaster').textContent = sliderMaster.value;
        enviarComando(`MASTER:${sliderMaster.value}`);
    });

    sliderBlink.addEventListener('input', () => {
        document.getElementById('valorBlink').textContent = `${sliderBlink.value} ms`;
        enviarComando(`SPEED:${sliderBlink.value}`);
    });

    document.querySelectorAll('[data-cmd]').forEach(button => {
        button.addEventListener('click', async () => {
            const command = button.dataset.cmd;
            if (/^ON[123]$/.test(command)) {
                setLevel(Number(command.slice(-1)), ultimosNiveles[Number(command.slice(-1))]);
            } else if (/^OFF[123]$/.test(command)) {
                setLevel(Number(command.slice(-1)), 0);
            } else if (command === 'ALLON') {
                [1, 2, 3].forEach(id => setLevel(id, ultimosNiveles[id] || 128));
            } else if (command === 'ALLOFF') {
                [1, 2, 3].forEach(id => setLevel(id, 0));
            }
        });
    });

    btnBlink.addEventListener('click', async () => {
        blinkActivo = !blinkActivo;
        await enviarComando(`BLINK:${blinkActivo ? 1 : 0}`);
        btnBlink.textContent = blinkActivo ? 'DETENER PARPADEO' : 'ACTIVAR PARPADEO';
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
