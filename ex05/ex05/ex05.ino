// ESP32 触摸换挡呼吸灯实验程序

// 硬件引脚定义
const int ledPwmPin = 2;      // LED的PWM输出引脚（板载LED）
const int touchSensorPin = 4; // 触摸传感器引脚，对应T0通道

// 触摸与档位变量
int gearLevel = 1;            // 当前呼吸档位，共1/2/3档
bool lastTouchStatus = false; // 上一次的触摸状态
unsigned long lastTriggerMs = 0;
const unsigned long debounceMs = 200; // 触摸消抖时长

// 呼吸灯运行参数
int ledBright = 0;
int fadeDelta = 1;
unsigned long lastFadeMs = 0;

// 档位对应速度表：数值为亮度更新间隔，单位ms，值越小呼吸越快
const int gearSpeed[] = {30, 15, 5}; // 1档最慢，3档最快

void setup() {
  Serial.begin(115200);
  pinMode(ledPwmPin, OUTPUT);

  Serial.println("多档位触摸呼吸灯初始化完成");
  Serial.print("当前运行档位：");
  Serial.println(gearLevel);
}

void loop() {
  // ========== 触摸检测与档位切换 ==========
  int rawTouchVal = touchRead(touchSensorPin);
  bool touchActive = (rawTouchVal < 400); // 低于阈值判定为触摸

  // 消抖 + 下降沿检测（仅触摸瞬间触发一次）
  if (touchActive && !lastTouchStatus && (millis() - lastTriggerMs > debounceMs)) {
    // 循环切换档位：1→2→3→1
    gearLevel = gearLevel % 3 + 1;
    
    Serial.print("有效触摸触发，切换至档位：");
    Serial.println(gearLevel);
    lastTriggerMs = millis();
  }
  lastTouchStatus = touchActive;

  // ========== 非阻塞式呼吸灯逻辑 ==========
  int updateInterval = gearSpeed[gearLevel - 1];

  if (millis() - lastFadeMs >= updateInterval) {
    lastFadeMs = millis();

    ledBright += fadeDelta;

    // 到达亮度边界时反转渐变方向
    if (ledBright <= 0 || ledBright >= 255) {
      fadeDelta = -fadeDelta;
      // 边界值修正，避免亮度溢出
      ledBright = ledBright < 0 ? 0 : 255;
    }

    analogWrite(ledPwmPin, ledBright);
  }
}
