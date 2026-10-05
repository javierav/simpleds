#include "services/provisioning/portal_html.h"

#include <algorithm>

#include "text/html_escape.h"

namespace simpleds::services {

namespace {

constexpr const char* kHead = R"(<!doctype html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>SimpleDS</title>
<style>
body{font-family:system-ui,sans-serif;max-width:420px;margin:0 auto;padding:24px 16px;color:#222;background:#f6f6f4}
h1{font-size:1.4rem;margin:0 0 8px}
p{color:#555}
label{display:block;margin:16px 0 6px;font-weight:600}
input[type=text],input[type=password],select{box-sizing:border-box;width:100%;padding:10px;font-size:1rem;border:1px solid #bbb;border-radius:6px}
#other{margin-top:8px}
button{margin-top:20px;width:100%;padding:12px;font-size:1rem;border:0;border-radius:6px;background:#d98a00;color:#fff;font-weight:600}
.error{background:#fde8e8;color:#9b1c1c;padding:10px;border-radius:6px}
.small{font-size:.9rem}
</style>
</head>
<body>
<h1>SimpleDS</h1>
)";

constexpr const char* kTail = "</body>\n</html>\n";

constexpr const char* kSignalBars[] = {"▂", "▂▄", "▂▄▆", "▂▄▆█"};

std::string wrap(const std::string& body) { return kHead + body + kTail; }

}  // namespace

std::string renderConfigPage(const std::vector<core::ScannedNetwork>& networks,
                             std::string_view selectedSsid, std::string_view errorMessage) {
  std::string body = "<p>Configura la red WiFi a la que debe conectarse el dispositivo.</p>\n";

  if (!errorMessage.empty()) {
    body += "<p class=\"error\">" + core::escapeHtml(errorMessage) + "</p>\n";
  }

  const bool selectedIsListed =
      std::any_of(networks.begin(), networks.end(),
                  [&](const auto& network) { return network.ssid == selectedSsid; });
  const bool showOther = networks.empty() || (!selectedSsid.empty() && !selectedIsListed);

  body += "<form method=\"post\" action=\"/save\">\n";
  body += "<label for=\"ssid\">Red WiFi</label>\n";
  body += "<select id=\"ssid\" name=\"" + std::string(kSsidField) +
          "\" onchange=\"other.hidden=this.value!=='';if(!other.hidden)other.focus()\">\n";
  for (const auto& network : networks) {
    const bool selected = network.ssid == selectedSsid;
    body += "<option value=\"" + core::escapeHtml(network.ssid) + "\"" +
            (selected ? " selected" : "") + ">" + core::escapeHtml(network.ssid) + "  " +
            std::string(kSignalBars[core::signalBars(network.rssi) - 1]) +
            (network.secured ? " 🔒" : "") + "</option>\n";
  }
  body += std::string("<option value=\"\"") + (showOther ? " selected" : "") +
          ">Otra red…</option>\n";
  body += "</select>\n";
  body += "<input type=\"text\" id=\"other\" name=\"" + std::string(kOtherSsidField) +
          "\" maxlength=\"32\" autocomplete=\"off\" placeholder=\"Nombre de la red\"" +
          (showOther ? "" : " hidden") + " value=\"" +
          (showOther ? core::escapeHtml(selectedSsid) : "") + "\">\n";

  body += "<label for=\"password\">Contraseña</label>\n";
  body += "<input type=\"password\" id=\"password\" name=\"" + std::string(kPasswordField) +
          "\" maxlength=\"63\">\n";
  body += "<label class=\"small\"><input type=\"checkbox\" "
          "onclick=\"password.type=this.checked?'text':'password'\"> Mostrar contraseña</label>\n";
  body += "<button type=\"submit\">Guardar y conectar</button>\n";
  body += "</form>\n";

  body += networks.empty() ? "<p class=\"small\">No se han encontrado redes. "
                             "<a href=\"/\">Buscar de nuevo</a></p>\n"
                           : "<p class=\"small\"><a href=\"/\">Buscar redes de nuevo</a></p>\n";
  return wrap(body);
}

std::string renderSavedPage(std::string_view ssid) {
  return wrap("<p>Configuración guardada. El dispositivo se reiniciará y se conectará a <strong>" +
              core::escapeHtml(ssid) +
              "</strong>.</p>\n<p>Ya puedes cerrar esta página y volver a tu red WiFi.</p>\n");
}

std::string renderErrorPage(std::string_view message) {
  return wrap("<p class=\"error\">" + core::escapeHtml(message) +
              "</p>\n<p><a href=\"/\">Volver</a></p>\n");
}

const char* describeError(core::CredentialsError error) {
  switch (error) {
    case core::CredentialsError::kNone: return "";
    case core::CredentialsError::kSsidEmpty: return "Indica el nombre de la red.";
    case core::CredentialsError::kSsidTooLong:
      return "El nombre de la red no puede superar 32 caracteres.";
    case core::CredentialsError::kPasswordTooShort:
      return "La contraseña debe tener al menos 8 caracteres (o dejarla vacía si la red es abierta).";
    case core::CredentialsError::kPasswordTooLong:
      return "La contraseña no puede superar 63 caracteres.";
  }
  return "Datos no válidos.";
}

}  // namespace simpleds::services
