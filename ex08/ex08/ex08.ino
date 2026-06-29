#include <WiFi.h>
#include <WebServer.h>

// ========== 系统参数配置 ==========
const char* wifi_ssid = "iPhone 15 pro";
const char* wifi_passwd = "20060929";
const int led_pin = 2;                // 报警指示灯引脚 D2
const int touch_pin = T0;             // 触摸检测引脚 T0 (对应GPIO4)
const int touch_threshold = 30;       // 触摸判定阈值，读数低于该值视为触发
// =================================

WebServer web_server(80);

// 系统运行状态变量
bool system_armed = false;    // 系统设防标志，默认撤防状态
bool alarm_active = false;    // 报警触发标志，默认未触发
unsigned long last_blink_tick = 0;
const int blink_interval = 100; // 报警时LED闪烁间隔，单位毫秒

// 首页请求处理：返回控制页面
void handle_index() {
  String status_label = system_armed ? "已布防" : "已撤防";
  String alarm_label = alarm_active ? " 警报触发中！" : "运行正常";
  
  String page_html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32触摸报警控制系统</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { font-family: "微软雅黑", sans-serif; text-align: center; padding-top: 60px; background: #f5f7fa; }
    .panel { width: 380px; margin: 0 auto; background: #fff; padding: 35px 25px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.06); }
    .title { font-size: 22px; color: #303133; margin-bottom: 25px; }
    .state { font-size: 20px; margin: 18px 0; color: #606266; }
    .warn { color: #f56c6c; font-weight: bold; }
    .btn { width: 120px; padding: 12px 0; font-size: 16px; margin: 0 8px; border: none; border-radius: 6px; cursor: pointer; color: #fff; }
    .btn-arm { background: #e6a23c; }
    .btn-disarm { background: #67c23a; }
  </style>
</head>
<body>
  <div class="panel">
    <div class="title">物联网触摸报警装置</div>
    <div class="state">设防状态: <span id="sys_status">)rawliteral" + status_label + R"rawliteral(</span></div>
    <div class="state warn" id="alarm_info">)rawliteral" + alarm_label + R"rawliteral(</div>
    
    <button class="btn btn-arm" onclick="location.href='/arm'">一键布防</button>
    <button class="btn btn-disarm" onclick="location.href='/disarm'">一键撤防</button>
  </div>
</body>
</html>
  )rawliteral";
  
  web_server.send(200, "text/html; charset=UTF-8", page_html);
}

// 处理布防请求
void handle_arm() {
  system_armed = true;
  alarm_active = false;
  digitalWrite(led_pin, LOW); // 复位LED状态
  Serial.println("系统进入布防状态");
  web_server.sendHeader("Location", "/");
  web_server.send(303);
}

// 处理撤防请求
void handle_disarm() {
  system_armed = false;
  alarm_active = false;
  digitalWrite(led_pin, LOW); // 关闭报警指示灯
  Serial.println("系统已撤防");
  web_server.sendHeader("Location", "/");
  web_server.send(303);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW); // 上电默认熄灭
  
  // WiFi连接流程
  Serial.print("正在连接WiFi网络...");
  WiFi.begin(wifi_ssid, wifi_passwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功");
  Serial.print("控制页面地址: http://");
  Serial.println(WiFi.localIP());
  
  // 注册页面路由
  web_server.on("/", handle_index);
  web_server.on("/arm", handle_arm);
  web_server.on("/disarm", handle_disarm);
  
  web_server.begin();
  Serial.println("Web服务已正常启动");
}

void loop() {
  web_server.handleClient();
  
  // 布防状态下持续检测触摸信号
  if (system_armed && !alarm_active) {
    int touch_val = touchRead(touch_pin);
    Serial.printf("触摸读数：%d\n", touch_val);
    
    if (touch_val < touch_threshold) {
      alarm_active = true;
      Serial.println("检测到触摸，触发报警！");
    }
  }
  
  // 报警状态下控制LED闪烁
  if (alarm_active) {
    unsigned long current_tick = millis();
    if (current_tick - last_blink_tick >= blink_interval) {
      last_blink_tick = current_tick;
      digitalWrite(led_pin, !digitalRead(led_pin)); // 翻转LED电平
    }
  }
}
