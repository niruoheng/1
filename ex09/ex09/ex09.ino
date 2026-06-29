#include <WiFi.h>
#include <WebServer.h>

// ====================== 配置区 ======================
const char* ssid     = "iPhone 15 pro";      // 替换为你的WiFi名称
const char* password = "20060929";      // 替换为你的WiFi密码
const int TOUCH_PIN  = T0;                 // 触摸引脚 T0 对应 GPIO4
// ===================================================

WebServer server(80);  // Web服务运行在80端口

// 前端网页：包含实时数值显示 + AJAX定时刷新
const char* indexPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>触摸传感器实时仪表盘</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: "Microsoft Yahei", sans-serif;
            background: #f0f2f5;
            height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        .dashboard {
            background: #fff;
            width: 360px;
            padding: 40px 30px;
            border-radius: 16px;
            box-shadow: 0 4px 20px rgba(0,0,0,0.08);
            text-align: center;
        }
        .title {
            font-size: 20px;
            color: #333;
            margin-bottom: 30px;
        }
        .value-box {
            font-size: 72px;
            font-weight: bold;
            color: #1677ff;
            margin: 20px 0;
            font-family: "Courier New", monospace;
        }
        .unit {
            font-size: 16px;
            color: #999;
            margin-top: -10px;
            margin-bottom: 30px;
        }
        .tip {
            font-size: 14px;
            color: #666;
            line-height: 1.6;
        }
    </style>
</head>
<body>
    <div class="dashboard">
        <div class="title">ESP32 触摸传感器监测面板</div>
        <div class="value-box" id="touchValue">--</div>
        <div class="unit">采样数值</div>
        <div class="tip">
            手指靠近引脚时数值变小<br>
            手指松开后数值恢复
        </div>
    </div>

    <script>
        // 定时从ESP32获取最新触摸数值
        function refreshValue() {
            fetch("/getTouch")
                .then(res => res.text())
                .then(val => {
                    document.getElementById("touchValue").textContent = val;
                })
                .catch(err => console.log("获取数据失败:", err));
        }

        // 页面加载先执行一次，之后每300毫秒刷新一次
        refreshValue();
        setInterval(refreshValue, 300);
    </script>
</body>
</html>
)HTML";

// 处理首页请求，返回仪表盘网页
void handleRoot() {
    server.send(200, "text/html; charset=UTF-8", indexPage);
}

// 处理数据请求，返回当前触摸传感器的数值
void handleGetTouch() {
    int touchVal = touchRead(TOUCH_PIN);
    server.send(200, "text/plain", String(touchVal));
}

void setup() {
    Serial.begin(115200);
    
    // 连接WiFi
    Serial.println();
    Serial.print("正在连接WiFi: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi连接成功!");
    Serial.print("仪表盘访问地址: http://");
    Serial.println(WiFi.localIP());
    
    // 注册网页路由
    server.on("/", handleRoot);
    server.on("/getTouch", handleGetTouch);
    
    // 启动Web服务器
    server.begin();
    Serial.println("Web仪表盘服务已启动");
}

void loop() {
    // 处理所有客户端请求
    server.handleClient();
}
