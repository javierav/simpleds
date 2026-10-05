'use strict';

const OTHER_NETWORK = '__other__';
const SCAN_POLL_MS = 2000;
const TEST_POLL_MS = 1000;
const SIGNAL_BARS = ['▂', '▂▄', '▂▄▆', '▂▄▆█'];

const ERROR_MESSAGES = {
  ssid_empty: 'Indica el nombre de la red.',
  ssid_too_long: 'El nombre de la red no puede superar 32 caracteres.',
  password_too_short:
    'La contraseña debe tener al menos 8 caracteres (o dejarla vacía si la red es abierta).',
  password_too_long: 'La contraseña no puede superar 63 caracteres.',
  storage: 'No se ha podido guardar la configuración. Inténtalo de nuevo.',
  busy: 'Ya se está probando una conexión. Espera un momento.',
  wrong_password: 'No se ha podido conectar: la contraseña no es correcta.',
  network_not_found: 'No se encuentra la red. Comprueba que el nombre es correcto y que está al alcance.',
  timeout: 'La red no ha respondido a tiempo. Inténtalo de nuevo.',
  connection_failed: 'No se ha podido conectar a la red. Inténtalo de nuevo.',
};

const $ = (id) => document.getElementById(id);

// Set once the user picks an option, so list refreshes keep their choice.
let userPickedNetwork = false;

function showError(message) {
  $('error').textContent = message;
  $('error').hidden = !message;
}

function showSection(id) {
  for (const section of ['config', 'testing', 'saved']) $(section).hidden = section !== id;
}

function updateOtherField() {
  userPickedNetwork = true;
  const isOther = $('ssid').value === OTHER_NETWORK;
  $('other').hidden = !isOther;
  if (isOther) $('other').focus();
}

function renderNetworks(networks) {
  const select = $('ssid');
  const previous = select.value;

  select.replaceChildren();
  for (const network of networks) {
    const label = `${network.ssid}  ${SIGNAL_BARS[network.bars - 1]}${network.secured ? ' 🔒' : ''}`;
    select.add(new Option(label, network.ssid));
  }
  select.add(new Option('Otra red…', OTHER_NETWORK));

  const options = Array.from(select.options);
  if (userPickedNetwork && options.some((option) => option.value === previous)) {
    select.value = previous;
  }
  $('other').hidden = select.value !== OTHER_NETWORK;
}

async function loadNetworks() {
  try {
    const response = await fetch('/api/networks');
    const data = await response.json();
    renderNetworks(data.networks);
    if (data.scanning) setTimeout(loadNetworks, SCAN_POLL_MS);
  } catch {
    setTimeout(loadNetworks, SCAN_POLL_MS);
  }
}

function showFailure(code) {
  showSection('config');
  showError(ERROR_MESSAGES[code] || ERROR_MESSAGES.connection_failed);
  $('submit').disabled = false;
}

function showSuccess({ ip, hostname }) {
  $('hostname-url').textContent = `http://${hostname}`;
  $('ip-url').textContent = `http://${ip}`;
  showSection('saved');
}

// The phone may lose the access point for a few seconds while the device
// joins the network (channel change), so request errors are retried.
async function pollTestStatus() {
  try {
    const response = await fetch('/api/wifi/status');
    const data = await response.json();
    if (data.status === 'connected') return showSuccess(data);
    if (data.status === 'failed') return showFailure(data.error);
  } catch {
    // Retry below.
  }
  setTimeout(pollTestStatus, TEST_POLL_MS);
}

async function save(event) {
  event.preventDefault();
  showError('');

  const select = $('ssid');
  const ssid = select.value === OTHER_NETWORK ? $('other').value : select.value;
  const body = new URLSearchParams({ ssid, password: $('password').value });

  $('submit').disabled = true;
  try {
    const response = await fetch('/api/wifi', { method: 'POST', body });
    const data = await response.json();
    if (!response.ok) return showFailure(data.error);
  } catch {
    showError('No se ha podido contactar con el dispositivo.');
    $('submit').disabled = false;
    return;
  }

  for (const element of document.querySelectorAll('.target-ssid')) element.textContent = ssid;
  showSection('testing');
  setTimeout(pollTestStatus, TEST_POLL_MS);
}

$('ssid').addEventListener('change', updateOtherField);
$('rescan').addEventListener('click', loadNetworks);
$('show-password').addEventListener('change', (event) => {
  $('password').type = event.target.checked ? 'text' : 'password';
});
$('form').addEventListener('submit', save);

loadNetworks();
