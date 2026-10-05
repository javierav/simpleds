'use strict';

const REFRESH_MS = 5000;
const SIGNAL_BARS = ['▂', '▂▄', '▂▄▆', '▂▄▆█'];

const $ = (id) => document.getElementById(id);

function formatUptime(totalSeconds) {
  const days = Math.floor(totalSeconds / 86400);
  const hours = Math.floor((totalSeconds % 86400) / 3600);
  const minutes = Math.floor((totalSeconds % 3600) / 60);
  if (days > 0) return `${days} d ${hours} h`;
  if (hours > 0) return `${hours} h ${minutes} min`;
  return `${minutes} min`;
}

function render(status) {
  $('ssid').textContent = status.ssid;
  $('signal').textContent = `${SIGNAL_BARS[status.bars - 1]} ${status.rssi} dBm`;
  $('address').textContent = `${status.hostname} · ${status.ip}`;
  $('uptime').textContent = formatUptime(status.uptime);
  $('heap').textContent = `${Math.round(status.freeHeap / 1024)} KB`;
  $('firmware').textContent = status.firmware;
}

async function refresh() {
  try {
    const response = await fetch('/api/status');
    render(await response.json());
    $('error').hidden = true;
  } catch {
    $('error').hidden = false;
  }
  setTimeout(refresh, REFRESH_MS);
}

refresh();
