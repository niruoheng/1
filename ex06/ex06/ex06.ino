// 引脚定义
const int ledFirstPin = 2;  // 第一路LED控制引脚
const int ledSecondPin = 4;  // 第二路LED控制引脚

// 呼吸灯运行参数
int lightLevel = 0;     // LED亮度值，范围0~255
int fadeDelta = 1;       // 每周期亮度增减步长
unsigned long lastFadeTick = 0;
const int fadePeriod = 10; // 亮度更新周期(ms)，数值越小呼吸速度越快

void setup() {
  Serial.begin(115200);
  pinMode(ledFirstPin, OUTPUT);
  pinMode(ledSecondPin, OUTPUT);
  
  Serial.println("ESP32 双通道PWM警车双闪启动");
}

void loop() {
  // 非阻塞式亮度渐变
  if (millis() - lastFadeTick >= fadePeriod) {
    lastFadeTick = millis();
    
    // 更新第一路LED的亮度
    lightLevel += fadeDelta;
    
    // 亮度到达边界时反转变化方向
    if (lightLevel <= 0 || lightLevel >= 255) {
      fadeDelta = -fadeDelta;
      lightLevel = constrain(lightLevel, 0, 255); // 限制亮度范围，防止溢出
    }
    
    // 核心逻辑：两路LED亮度完全反相
    // 第一路灯从0增至255时，第二路灯同步从255减至0
    analogWrite(ledFirstPin, lightLevel);
    analogWrite(ledSecondPin, 255 - lightLevel);
  }
}
