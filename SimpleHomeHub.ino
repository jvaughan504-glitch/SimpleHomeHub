#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// ================== WiFi CONFIG ==================
const char* WIFI_SSID = "BELL728";
const char* WIFI_PASS = "9134EC94D365";

// ================== DEVICE LIST ==================
struct DeviceInfo {
  const char* name;
  const char* url;
  const char* description;
};

DeviceInfo devices[] = {
  { "Bedroom Vent", "http://192.168.1.50", "Smart Heating Vent" },
  { "Fireplace",          "http://192.168.1.51", "Smart Fireplace" },
  { "Light",      "http://192.168.1.60", "Smart Light" },
  // Add more here as you build them
};

const size_t DEVICE_COUNT = sizeof(devices) / sizeof(devices[0]);

// ================== WEB SERVER ==================
WebServer server(80);
bool serverStarted = false;

// ----------------- HTML PAGE -----------------
String generateMainPage() {
  String html;

  html  = "<!doctype html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>DIY Device Hub</title><style>";

  // LIGHT MODE
  html += "@media (prefers-color-scheme: light){";
  html += "body{margin:0;font-family:system-ui,-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;";
  html += "background:#f5f5f5;color:#111;min-height:100vh;display:flex;justify-content:center;}";
  html += ".wrap{max-width:900px;width:100%;padding:20px;box-sizing:border-box;}";
  html += "h1{margin:0 0 8px;font-size:1.8rem;}";
  html += "p.subtitle{margin:0 0 20px;color:#555;font-size:0.95rem;}";
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(240px,1fr));gap:16px;}";
  html += ".card{background:#fff;border-radius:16px;padding:16px 14px;box-shadow:0 10px 25px rgba(0,0,0,0.08);";
  html += "display:flex;flex-direction:column;gap:8px;}";
  html += ".card h2{margin:0;font-size:1.1rem;}";
  html += ".card p{margin:0;font-size:0.9rem;color:#555;}";
  html += ".btn-row{margin-top:10px;display:flex;gap:8px;flex-wrap:wrap;}";
  html += ".btn{flex:1;text-align:center;text-decoration:none;font-size:0.9rem;font-weight:500;";
  html += "padding:8px 10px;border-radius:999px;border:none;cursor:pointer;";
  html += "background:#111;color:#fff;transition:transform .1s,box-shadow .1s,background .1s;}";
  html += ".btn:hover{transform:translateY(-1px);box-shadow:0 6px 14px rgba(0,0,0,0.25);background:#222;}";
  html += ".chip{display:inline-block;padding:2px 8px;border-radius:999px;background:#eee;";
  html += "font-size:0.75rem;color:#666;margin-top:4px;}";
  html += "footer{margin-top:24px;font-size:0.75rem;color:#777;text-align:center;}";
  html += "}";

  // DARK MODE
  html += "@media (prefers-color-scheme: dark){";
  html += "body{margin:0;font-family:system-ui,-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;";
  html += "background:#050509;color:#f5f5f5;min-height:100vh;display:flex;justify-content:center;}";
  html += ".wrap{max-width:900px;width:100%;padding:20px;box-sizing:border-box;}";
  html += "h1{margin:0 0 8px;font-size:1.8rem;}";
  html += "p.subtitle{margin:0 0 20px;color:#aaa;font-size:0.95rem;}";
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(240px,1fr));gap:16px;}";
  html += ".card{background:radial-gradient(circle at top left,#202033,#101018);border-radius:16px;";
  html += "padding:16px 14px;box-shadow:0 10px 30px rgba(0,0,0,0.7);display:flex;flex-direction:column;gap:8px;}";
  html += ".card h2{margin:0;font-size:1.1rem;color:#fff;}";
  html += ".card p{margin:0;font-size:0.9rem;color:#ccc;}";
  html += ".btn-row{margin-top:10px;display:flex;gap:8px;flex-wrap:wrap;}";
  html += ".btn{flex:1;text-align:center;text-decoration:none;font-size:0.9rem;font-weight:500;";
  html += "padding:8px 10px;border-radius:999px;border:none;cursor:pointer;";
  html += "background:linear-gradient(135deg,#f97316,#ec4899);color:#fff;";
  html += "box-shadow:0 10px 30px rgba(248,113,22,0.5);transition:transform .1s,box-shadow .1s,filter .1s;}";
  html += ".btn:hover{transform:translateY(-1px);box-shadow:0 16px 40px rgba(236,72,153,0.6);filter:brightness(1.1);}";
  html += ".chip{display:inline-block;padding:2px 8px;border-radius:999px;background:rgba(255,255,255,0.06);";
  html += "font-size:0.75rem;color:#aaa;margin-top:4px;border:1px solid rgba(255,255,255,0.06);}";
  html += "footer{margin-top:24px;font-size:0.75rem;color:#888;text-align:center;}";
  html += "}";
  html += "</style></head><body><div class='wrap'>";

  html += "<h1>DIY Device Hub</h1>";
  html += "<p class='subtitle'>Central landing page for your ESP32 toys &amp; home automation projects.</p>";

  html += "<div class='grid'>";
  for (size_t i = 0; i < DEVICE_COUNT; i++) {
    html += "<div class='card'>";
    html += "<h2>" + String(devices[i].name) + "</h2>";
    html += "<p>" + String(devices[i].description) + "</p>";
    html += "<span class='chip'>" + String(devices[i].url) + "</span>";
    html += "<div class='btn-row'>";
    html += "<a class='btn' href='" + String(devices[i].url) + "' target='_blank' rel='noopener'>Open</a>";
    html += "</div></div>";
  }
  html += "</div>"; // grid

  html += "<footer>ESP32 DevKit Hub &middot; v1.0</footer>";
  html += "</div></body></html>";

  return html;
}

// ----------------- HTTP HANDLERS -----------------
void handleRoot() {
  String html = generateMainPage();
  server.send(200, "text/html", html);
}

void handleNotFound() {
  String msg = "404 Not Found\n\n";
  msg += "URI: " + server.uri() + "\n";
  msg += "Method: " + String((server.method() == HTTP_GET) ? "GET" : "POST") + "\n";
  server.send(404, "text/plain", msg);
}

bool connectToWiFi(uint8_t maxAttempts = 30) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to WiFi");
  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("Failed to connect to WiFi. Check SSID/password and coverage.");
  return false;
}

// ================== SETUP & LOOP ==================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("ESP32 DIY Device Hub starting...");

  if (!connectToWiFi()) {
    return;
  }

  // Optional: mDNS so you can use http://esp32-hub.local
  if (MDNS.begin("esp32-hub")) {
    Serial.println("mDNS responder started: http://esp32-hub.local");
  } else {
    Serial.println("Error starting mDNS");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started on port 80");
  serverStarted = true;
}

void loop() {
  if (!serverStarted) {
    delay(1000);
    return;
  }
  server.handleClient();
}
