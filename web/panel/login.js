'use strict';

const ERRORS = {
  401: 'Contraseña incorrecta.',
  429: 'Demasiados intentos fallidos. Espera un minuto y vuelve a probar.',
};

const $ = (id) => document.getElementById(id);

$('form').addEventListener('submit', async (event) => {
  event.preventDefault();
  $('submit').disabled = true;
  try {
    const response = await fetch('/api/login', {
      method: 'POST',
      body: new URLSearchParams({ password: $('password').value }),
    });
    if (response.ok) {
      location.href = '/';
      return;
    }
    $('error').textContent = ERRORS[response.status] || 'Error inesperado.';
  } catch {
    $('error').textContent = 'No se puede contactar con el dispositivo.';
  }
  $('error').hidden = false;
  $('submit').disabled = false;
});
