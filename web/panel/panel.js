'use strict';

const REFRESH_MS = 5000;
const SIGNAL_BARS = ['▂', '▂▄', '▂▄▆', '▂▄▆█'];
const LOGIN_URL = '/panel/login.html';

const STATES = {
  starting: 'Iniciando',
  active: 'Activo',
  emergency: 'Emergencia',
};
const REASONS = {
  overheat: 'sobrecalentamiento del SSR',
  sensor_failure: 'fallo de la sonda del SSR',
};
const MQTT_STATES = {
  disabled: 'Desactivado',
  connecting: 'Conectando…',
  connected: 'Conectado',
};
const FIELD_ERRORS = {
  out_of_range: 'valor fuera de rango',
  not_below_trip: 'debe ser menor que la temperatura de emergencia',
  not_below_on: 'debe ser menor que la de encendido',
  invalid: 'valor no válido',
  too_long: 'demasiado largo',
  too_short: 'debe tener al menos 8 caracteres',
  same_as_command: 'no puede coincidir con el topic de órdenes',
  duplicated: 'repetido en otro topic',
};

const $ = (id) => document.getElementById(id);

async function api(path, data) {
  const options = data ? { method: 'POST', body: new URLSearchParams(data) } : {};
  const response = await fetch(path, options);
  if (response.status === 401) {
    location.href = LOGIN_URL;
    throw new Error('unauthorized');
  }
  const body = response.status === 204 ? {} : await response.json();
  return { ok: response.ok, body };
}

function formatUptime(totalSeconds) {
  const days = Math.floor(totalSeconds / 86400);
  const hours = Math.floor((totalSeconds % 86400) / 3600);
  const minutes = Math.floor((totalSeconds % 3600) / 60);
  if (days > 0) return `${days} d ${hours} h`;
  if (hours > 0) return `${hours} h ${minutes} min`;
  return `${minutes} min`;
}

function formatTemperature(value, sampled) {
  if (!sampled) return '—';
  return value === null ? 'Error de lectura' : `${value.toFixed(1)} °C`;
}

function renderDiverter(d, mqtt) {
  const state = $('state');
  state.textContent = STATES[d.state] + (d.state === 'emergency' ? `: ${REASONS[d.reason]}` : '');
  state.className = `state-${d.state}`;
  $('output').textContent = `${d.output.toFixed(1)} %`;
  $('command').textContent = d.command === null ? 'Sin orden' : `${d.command.toFixed(1)} %`;
  $('ssr-temperature').textContent = formatTemperature(d.ssrTemperature, d.ssrSampled);
  $('tank-temperature').textContent = formatTemperature(d.tankTemperature, d.tankSampled);
  $('fan').textContent = d.fan ? 'Encendido' : 'Apagado';
  $('mqtt').textContent = MQTT_STATES[mqtt];
}

function renderDevice(status) {
  $('ssid').textContent = status.ssid;
  $('signal').textContent = `${SIGNAL_BARS[status.bars - 1]} ${status.rssi} dBm`;
  $('address').textContent = `${status.hostname} · ${status.ip}`;
  $('uptime').textContent = formatUptime(status.uptime);
  $('heap').textContent = `${Math.round(status.freeHeap / 1024)} KB`;
  $('firmware').textContent = status.firmware;
}

async function refresh() {
  try {
    const { body } = await api('/api/status');
    renderDiverter(body.diverter, body.mqtt);
    renderDevice(body.device);
    $('error').hidden = true;
  } catch {
    $('error').hidden = false;
  }
  setTimeout(refresh, REFRESH_MS);
}

function showMessage(form, text, ok) {
  const message = form.querySelector('.message');
  message.textContent = text;
  message.className = `message ${ok ? 'ok' : 'error'}`;
  message.hidden = false;
}

function errorText(form, body) {
  if (body.error === 'invalid') {
    const label = form.querySelector(`[name="${body.field}"]`)?.labels[0]?.textContent;
    return `${label || body.field}: ${FIELD_ERRORS[body.code] || body.code}.`;
  }
  if (body.error === 'storage') return 'No se ha podido guardar la configuración.';
  return 'Error inesperado.';
}

function fillForm(form, values) {
  for (const [name, value] of Object.entries(values)) {
    const input = form.elements.namedItem(name);
    if (input && input.type !== 'password') input.value = value;
  }
}

function renderPasswordState(hasPassword) {
  $('password-status').textContent = hasPassword
    ? 'El panel está protegido con contraseña.'
    : 'El panel no tiene contraseña: cualquiera en tu red puede acceder.';
  $('remove-password').hidden = !hasPassword;
  $('logout').hidden = !hasPassword;
}

async function loadSettings() {
  const { body } = await api('/api/settings');
  fillForm($('control-form'), body.control);
  fillForm($('mqtt-form'), body.mqtt);
  $('mqtt-password').placeholder = body.mqtt.hasPassword ? 'Sin cambios' : '';
  renderPasswordState(body.panel.hasPassword);
}

function onSubmit(id, handler) {
  const form = $(id);
  form.addEventListener('submit', async (event) => {
    event.preventDefault();
    const button = form.querySelector('button[type="submit"]');
    button.disabled = true;
    try {
      await handler(form);
    } catch {
      showMessage(form, 'No se puede contactar con el dispositivo.', false);
    }
    button.disabled = false;
  });
}

onSubmit('control-form', async (form) => {
  const { ok, body } = await api('/api/settings/control', new FormData(form));
  showMessage(form, ok ? 'Configuración guardada.' : errorText(form, body), ok);
});

onSubmit('mqtt-form', async (form) => {
  const { ok, body } = await api('/api/settings/mqtt', new FormData(form));
  if (!ok) {
    showMessage(form, errorText(form, body), false);
    return;
  }
  showMessage(form, 'Configuración guardada. El dispositivo se está reiniciando…', true);
  setTimeout(() => location.reload(), 8000);
});

async function savePassword(form, password) {
  const { ok, body } = await api('/api/settings/password', { password });
  if (!ok) {
    showMessage(form, errorText(form, body), false);
    return;
  }
  form.reset();
  renderPasswordState(body.hasPassword);
  showMessage(form, body.hasPassword ? 'Contraseña guardada.' : 'Contraseña eliminada.', true);
}

onSubmit('password-form', async (form) => {
  const password = $('new-password').value;
  if (password !== $('confirm-password').value) {
    showMessage(form, 'Las contraseñas no coinciden.', false);
    return;
  }
  await savePassword(form, password);
});

$('remove-password').addEventListener('click', async () => {
  if (!confirm('¿Quitar la contraseña? Cualquiera en tu red podrá acceder al panel.')) return;
  try {
    await savePassword($('password-form'), '');
  } catch {
    showMessage($('password-form'), 'No se puede contactar con el dispositivo.', false);
  }
});

$('logout').addEventListener('click', async () => {
  await fetch('/api/logout', { method: 'POST' });
  location.href = LOGIN_URL;
});

refresh();
loadSettings().catch(() => {
  $('error').hidden = false;
});
