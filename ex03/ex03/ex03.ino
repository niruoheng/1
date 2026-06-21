// 定义LED引脚，ESP32板载LED默认连接在GPIO2
const int ledPin = 2;

// 摩斯码基础时间单位（毫秒），改这个数值可以整体调整闪烁快慢
const int unit = 200;
// 短信号（点）持续时长
const int dotTime = unit;
// 长信号（划）持续时长 = 3倍基础单位
const int dashTime = 3 * unit;
// 同字母内，两个信号之间的熄灭间隔
const int gapInChar = unit;
// 字母与字母之间的熄灭间隔
const int gapBetweenChar = 3 * unit;
// 整轮SOS发送完毕后的长停顿
const int gapEnd = 7 * unit;

void setup() {
  // 初始化串口通信，波特率115200，和实验教程保持一致
  Serial.begin(115200);
  // 将LED引脚设置为输出模式
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // ---------- 发送字母 S（三短闪） ----------
  Serial.println("发送 S: · · ·");
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);  // 点亮LED
    delay(dotTime);              // 保持短亮
    digitalWrite(ledPin, LOW);   // 熄灭LED
    delay(gapInChar);            // 信号间短间隔
  }
  // 补全字母间的总间隔（循环末尾已经有1个单位间隔，这里补剩下的2个单位）
  delay(gapBetweenChar - gapInChar);

  // ---------- 发送字母 O（三长闪） ----------
  Serial.println("发送 O: — — —");
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);  // 点亮LED
    delay(dashTime);             // 保持长亮
    digitalWrite(ledPin, LOW);   // 熄灭LED
    delay(gapInChar);            // 信号间短间隔
  }
  // 补全字母间的总间隔
  delay(gapBetweenChar - gapInChar);

  // ---------- 再次发送字母 S（三短闪） ----------
  Serial.println("发送 S: · · ·");
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);  // 点亮LED
    delay(dotTime);              // 保持短亮
    digitalWrite(ledPin, LOW);   // 熄灭LED
    delay(gapInChar);            // 信号间短间隔
  }

  // ---------- 整轮结束，长停顿后循环 ----------
  Serial.println("--- SOS发送完成，等待下一轮 ---");
  delay(gapEnd);
}
