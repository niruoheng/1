#include <WiFi.h>
#include <WebServer.h>

// ========== WiFi与硬件配置 ==========
const char* wifi_ssid = "iPhone 15 pro";
const char* wifi_passwd = "20060929";

// LED控制引脚，板载LED默认GPIO2
const int led_ctrl_pin = 2;

// Web服务实例，端口80
WebServer web_server(80);

// 前端网页内容
const char* web_page = R"HTML(
<!DOCTYPE html>
<html lang="zh">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Web调光控制台</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: "Microsoft Yahei", sans-serif;
            background: #eceff1;
            display: flex;
            justify-content: center;
            padding-top: 60px;
        }
        .card {
            background: #ffffff;
            width: 420px;
            padding: 35px 30px;
            border-radius: 12px;
            box-shadow: 0 2px 12px rgba(0,0,0,0.08);
        }
        .card h2 {
            color: #455a64;
            text-align: center;
            margin-bottom: 30px;
        }
        .slider-wrap {
            padding: 10px 0;
        }
        #rangeBar {
            width: 100%;
            height: 18px;
            outline: none;
            cursor: pointer;
        }
        .num-show {
            text-align: center;
            margin-top: 18px;
            font-size: 26px;
            font-weight: 700;
            color: #1976d2;
        }
    </style>
</head>
<body>
    <div class="card">
        <h2>LED亮度无级调节</h2>
        <div class="slider-wrap">
            <input type="range" id="rangeBar" min="0" max="255" value="0">
            <div class="num-show">当前亮度：<span id="showNum">0</span></div>
        </div>
    </div>

    <script>
        const bar = document.getElementById('rangeBar');
        const num = document.getElementById('showNum');

        bar.oninput = function() {
            let val = this.value;
            num.innerText = val;
            // 向ESP32发送亮度参数
            fetch(`/set?brightness=${val}`)
                .catch(e => console.log("请求异常:", e));
        }
    </script>
</body>
</html>
)HTML";

// 首页响应：返回网页
void handle_home() {
    web_server.send(200, "text/html", web_page);
}

// 亮度设置接口
void handle_set() {
    if (web_server.hasArg("brightness")) {
        int val = web_server.arg("brightness").toInt();
        
        // 亮度值边界约束
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        
        // 输出PWM调节LED亮度
        analogWrite(led_ctrl_pin, val);
        
        Serial.print("当前亮度已调整为：");
        Serial.println(val);
        web_server.send(200, "text/plain", "OK");
    } else {
        web_server.send(400, "text/plain", "参数错误");
    }
}

void setup() {
    Serial.begin(115200);
    
    pinMode(led_ctrl_pin, OUTPUT);
    analogWrite(led_ctrl_pin, 0); // 上电默认熄灭
    
    // WiFi连接流程
    Serial.println();
    Serial.print("正在接入WiFi：");
    Serial.println(wifi_ssid);
    
    WiFi.begin(wifi_ssid, wifi_passwd);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("-");
    }
    
    Serial.println();
    Serial.println("WiFi连接完成");
    Serial.print("设备IP地址：");
    Serial.println(WiFi.localIP());
    
    // 注册URL路由
    web_server.on("/", handle_home);
    web_server.on("/set", handle_set);
    
    web_server.begin();
    Serial.println("Web服务已开启");
}

void loop() {
    // 循环处理客户端请求
    web_server.handleClient();
}
