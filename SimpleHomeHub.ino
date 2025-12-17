#include <WiFi.h>
#include <WebServer.h>

// ================== AP CONFIG ==================
#define AP_SSID "ESP32-BRIDGE"
#define AP_PASS "87654321"

// AP network settings
IPAddress ap_ip(192, 168, 4, 1);
IPAddress ap_gateway(192, 168, 4, 1);
IPAddress ap_mask(255, 255, 255, 0);

// ================== DEVICE LIST ==================
// IMPORTANT: These must match the static IPs you give each device
struct DeviceInfo {
  const char* name;
  const char* url;
  const char* description;
};

DeviceInfo devices[] = {
  { "Bedroom Vent", "http://192.168.4.10", "Smart Heating Vent" },
  { "Fireplace",    "http://192.168.4.11", "Smart Fireplace" },
  { "Light",        "http://192.168.4.12", "Smart Light" },
  // Add more here as you build them
};

const size_t DEVICE_COUNT = sizeof(devices) / sizeof(devices[0]);

// ================== WEB SERVER ==================
WebServer server(80);
bool routesConfigured = false;
bool serverStarted    = false;

// ----------------- HTML PAGE -----------------
String generateMainPage() {
  String html;

  IPAddress apIP = WiFi.softAPIP();

  html  = "<!doctype html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>DIY Device Hub</title><style>";

  // ===== COMMON =====
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(280px,1fr));gap:16px;}";
  html += ".card{border-radius:16px;padding:14px 14px 12px;box-sizing:border-box;display:flex;flex-direction:column;gap:8px;}";
  html += ".card-header h2{margin:0;font-size:1.05rem;}";
  html += ".card-header p{margin:2px 0 4px;font-size:0.85rem;}";
  html += ".chip{display:inline-block;padding:2px 8px;border-radius:999px;font-size:0.75rem;margin-top:2px;}";
  html += ".card-body{margin-top:6px;}";
  html += ".device-frame{width:100%;min-height:420px;border:0;border-radius:12px;overflow:hidden;}";
  html += ".btn-row{margin-top:8px;display:flex;gap:8px;flex-wrap:wrap;}";
  html += ".btn{flex:1;text-align:center;text-decoration:none;font-size:0.9rem;font-weight:500;";
  html += "padding:8px 10px;border-radius:999px;border:none;cursor:pointer;}";
  html += "footer{margin-top:24px;font-size:0.75rem;text-align:center;}";

  // ===== LIGHT MODE =====
  html += "@media (prefers-color-scheme: light){";
  html += "body{margin:0;font-family:system-ui,-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;";
  html += "background:#f5f5f5;color:#111;min-height:100vh;display:flex;justify-content:center;}";
  html += ".wrap{max-width:1200px;width:100%;padding:20px;box-sizing:border-box;}";
  html += "h1{margin:0 0 8px;font-size:1.8rem;}";
  html += "p.subtitle{margin:0 0 18px;color:#555;font-size:0.95rem;}";
  html += ".card{background:#ffffff;box-shadow:0 10px 25px rgba(0,0,0,0.08);border:1px solid #e0e0e0;}";
  html += ".card-header h2{color:#111;}";
  html += ".card-header p{color:#555;}";
  html += ".chip{background:#eee;color:#666;}";
  html += ".btn{background:#111;color:#fff;transition:transform .1s,box-shadow .1s,background .1s;}";
  html += ".btn:hover{transform:translateY(-1px);box-shadow:0 6px 14px rgba(0,0,0,0.25);background:#222;}";
  html += "footer{color:#777;}";
  html += "}";

  // ===== DARK MODE =====
  html += "@media (prefers-color-scheme: dark){";
  html += "body{margin:0;font-family:system-ui,-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;";
  html += "background:#020617;color:#e5e7eb;min-height:100vh;display:flex;justify-content:center;}";
  html += ".wrap{max-width:1200px;width:100%;padding:20px;box-sizing:border-box;}";
  html += "h1{margin:0 0 8px;font-size:1.8rem;}";
  html += "p.subtitle{margin:0 0 18px;color:#9ca3af;font-size:0.95rem;}";
  html += ".card{background:#020617;box-shadow:0 20px 40px rgba(0,0,0,0.7);";
  html += "border:1px solid rgba(148,163,184,0.35);}";
  html += ".card-header h2{color:#f9fafb;}";
  html += ".card-header p{color:#9ca3af;}";
  html += ".chip{background:rgba(15,23,42,0.9);border:1px solid rgba(148,163,184,0.5);color:#e5e7eb;}";
  html += ".btn{background:linear-gradient(135deg,#f97316,#ec4899);color:#fff;";
  html += "box-shadow:0 10px 30px rgba(248,113,22,0.5);transition:transform .1s,box-shadow .1s,filter .1s;}";
  html += ".btn:hover{transform:translateY(-1px);box-shadow:0 16px 40px rgba(236,72,153,0.6);filter:brightness(1.1);}";
  html += "footer{color:#9ca3af;}";
  html += "}";
  html += "</style></head><body><div class='wrap'>";

  html += "<h1>DIY Device Hub</h1>";
  html += "<p class='subtitle'>ESP32 Wi-Fi island – inline views of each device UI.</p>";

  html += "<div class='grid'>";
  for (size_t i = 0; i < DEVICE_COUNT; i++) {
    html += "<div class='card'>";
    html += "  <div class='card-header'>";
    html += "    <h2>" + String(devices[i].name) + "</h2>";
    html += "    <p>" + String(devices[i].description) + "</p>";
    html += "    <span class='chip'>" + String(devices[i].url) + "</span>";
    html += "  </div>";
    html += "  <div class='card-body'>";
    html += "    <iframe class='device-frame' src='" + String(devices[i].url) + "' loading='lazy'></iframe>";
    html += "  </div>";
    html += "  <div class='btn-row'>";
    html += "    <a class='btn' href='" + String(devices[i].url) + "' target='_blank' rel='noopener'>Open full page</a>";
    html += "  </div>";
    html += "</div>";
  }
  html += "</div>"; // grid

  html += "<footer>";
  html += "ESP32 DevKit Hub · AP-only · SSID: " + String(AP_SSID);
  html += " · AP IP: " + apIP.toString();
  html += "</footer>";

  html += "</div></body></html>";

  return html;
}


// ----------------- HTTP HANDLERS -----------------
void handleRoot() {
  server.send(200, "text/html", generateMainPage());
}

void handleNotFound() {
  String msg = "404 Not Found\n\n";
  msg += "URI: " + server.uri() + "\n";
  msg += "Method: " + String((server.method() == HTTP_GET) ? "GET" : "POST") + "\n";
  server.send(404, "text/plain", msg);
}

void configureRoutes() {
  if (routesConfigured) return;

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  routesConfigured = true;
}

void startServerIfNeeded() {
  if (serverStarted) return;

  configureRoutes();
  server.begin();
  serverStarted = true;
  Serial.println("HTTP server started on port 80");
}

// ================== SETUP & LOOP ==================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("ESP32 DIY Device Hub (AP-only) starting...");

  // AP-only mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ap_ip, ap_gateway, ap_mask);

  if (WiFi.softAP(AP_SSID, AP_PASS)) {
    Serial.print("AP started. SSID: ");
    Serial.print(AP_SSID);
    Serial.print("  PASS: ");
    Serial.println(AP_PASS);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Failed to start AP!");
  }

  startServerIfNeeded();
}

void loop() {
  server.handleClient();

  // Debug: show number of connected stations occasionally
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    lastPrint = millis();
    Serial.print("Stations connected: ");
    Serial.println(WiFi.softAPgetStationNum());
  }
}
