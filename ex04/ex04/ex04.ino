#define SENSOR_PIN 4
#define LIGHT_PIN  2

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int touchThreshold = 700;
const int debounceTime = 50; 

bool lightStatus = false; // false对应熄灭，true对应点亮

void setup() {
  Serial.begin(115200);
  // 绑定LED引脚并配置PWM参数
  ledcAttach(LIGHT_PIN, pwmFreq, pwmResolution);
  ledcWrite(LIGHT_PIN, 255); // 上电默认熄灭
  Serial.println("触摸自锁开关程序初始化完成");
  Serial.println("使用说明：触摸一次切换LED亮灭状态");
}

void loop() {
  int touchValue = touchRead(SENSOR_PIN);
  Serial.printf("当前触摸读数：%d\n", touchValue);

  // 检测到触摸信号
  if (touchValue < touchThreshold) {
    delay(debounceTime); // 软件防抖延时
    // 防抖后二次确认触摸状态
    if (touchRead(SENSOR_PIN) < touchThreshold) {
      
      // 翻转LED状态
      lightStatus = !lightStatus;
      ledcWrite(LIGHT_PIN, lightStatus ? 0 : 255); 
      Serial.printf("状态已切换，LED当前：%s\n", lightStatus ? "点亮" : "熄灭");

      // 等待手指松开，防止单次触摸重复触发
      while (touchRead(SENSOR_PIN) < touchThreshold) {
        delay(10);
      }
    }
  }

  delay(50);
}
