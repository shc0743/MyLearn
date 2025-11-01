#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>

// 引脚定义
#define TOUCH_PIN       PA8
#define LED_PIN         PC13   // 板载LED
#define POWER_PIN       PA11   // 模块电源控制
#define OUTPUT_PIN      PA12   // 输出控制
#define ENROLL_PIN      PB10   // 新录入指纹按钮（低电平触发）
#define CLEAR_PIN       PB11   // 清除所有指纹按钮（低电平触发）

// 函数声明
void unlockDoor(bool bUnlock = true);
bool enrollFingerprint(int id = -1);
bool clearAllFingerprints();
bool checkButton(int pin, bool longPress = false);

// 指纹模块串口
HardwareSerial mySerial(PA10, PA9); // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// 状态变量
unsigned long clearPressTime = 0;
bool clearButtonPressed = false;

void setup() {
  // 初始化引脚
  pinMode(TOUCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(ENROLL_PIN, INPUT_PULLUP);  // 内部上拉，低电平触发
  pinMode(CLEAR_PIN, INPUT_PULLUP);   // 内部上拉，低电平触发
  
  digitalWrite(LED_PIN, HIGH);   // 初始熄灭
  digitalWrite(OUTPUT_PIN, LOW); // 输出初始关闭
  digitalWrite(POWER_PIN, LOW);  // 模块电源关闭
  
  // LED状态指示：快闪2次表示启动
  blinkLED(2, 200);
  delay(1000);
}

void loop() {
  // 检测录入按钮（短按触发）
  if (checkButton(ENROLL_PIN, false)) {
    enableFingerprintModule(true);
    blinkLED(3, 100); // 提示进入录入模式
    
    // 执行指纹录入（自动分配ID）
    if (enrollFingerprint()) {
      blinkLED(5, 100); // 录入成功
    } else {
      blinkLED(2, 500); // 录入失败
    }
    
    enableFingerprintModule(false);
  }
  
  // 检测清除按钮（长按5秒触发）
  if (checkButton(CLEAR_PIN, true)) {
    enableFingerprintModule(true);
    blinkLED(10, 50); // 紧急闪烁提示
    
    if (clearAllFingerprints()) {
      blinkLED(30, 300); // 清除成功
    } else {
      blinkLED(20, 500); // 清除失败
    }
    
    enableFingerprintModule(false);
  }
  
  // 检测触摸信号（正常开锁）
  if (digitalRead(TOUCH_PIN) == HIGH) {
    enableFingerprintModule(true);
    delay(50); // 消抖
    
    // 执行指纹识别
    int r = 0;
    fingerprintSearch(&r);
    
    if (r) {
      unlockDoor(true);
    }
    
    // 等待触摸释放
    while(digitalRead(TOUCH_PIN) == HIGH) delay(10);
    
    
    if (r) {
      delay(2000);
      unlockDoor(false);
    }
    delay(200);
    enableFingerprintModule(false);
  }
  
  delay(50); // 缩短主循环延迟
}

// 录入新指纹
// 录入新指纹（改进版，多次采样）
bool enrollFingerprint(int id) {
  if (id == -1) {
    // 自动寻找空闲ID
    for (id = 1; id < 127; id++) {
      if (finger.loadModel(id) != FINGERPRINT_OK) {
        break; // 找到空闲ID
      }
    }
  }
  
  int samples = 0;
  const int requiredSamples = 6; // 需要6次有效采样
  
  blinkLED(2, 200); // 提示开始录入
  
  while (samples < requiredSamples) {
    // 提示放手指
    blinkLED(samples + 1, 300); // 闪烁次数表示当前第几次
    
    // 获取指纹图像
    int p = finger.getImage();
    if (p != FINGERPRINT_OK) {
      if (p == FINGERPRINT_NOFINGER) {
        continue; // 没检测到手指，继续等待
      } else {
        blinkLED(3, 100); // 获取图像失败
        return false;
      }
    }
    
    // 转换图像特征
    p = finger.image2Tz(samples + 1); // 第1~6次特征
    if (p != FINGERPRINT_OK) {
      blinkLED(2, 500); // 特征提取失败
      return false;
    }
    
    samples++;
    
    // 提示抬起手指（除了最后一次）
    if (samples < requiredSamples) {
      blinkLED(1, 500); // 单次长闪提示抬起
      delay(1000);
      
      // 等待手指抬起
      while (finger.getImage() != FINGERPRINT_NOFINGER) {
        delay(100);
      }
      delay(500); // 抬起后稍作等待
    }
  }
  
  // 创建指纹模型
  if (finger.createModel() == FINGERPRINT_OK) {
    if (finger.storeModel(id) == FINGERPRINT_OK) {
      return true;
    }
  }
  
  return false;
}

// 清除所有指纹
bool clearAllFingerprints() {
  return (finger.emptyDatabase() == FINGERPRINT_OK);
}

// 指纹模块电源控制
void enableFingerprintModule(bool enable) {
  if (enable) {
    digitalWrite(POWER_PIN, HIGH);
    delay(500);
    mySerial.begin(57600);
    finger.begin(57600);
    
    if (!finger.verifyPassword()) {
      blinkLED(8, 50);
    }
  } else {
    digitalWrite(POWER_PIN, LOW);
    mySerial.end();
  }
}

void fingerprintSearch(int* result) {
  *result = 0;
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    blinkLED(1, 300);
    return;
  }
  
  blinkLED(2, 200);
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    blinkLED(3, 200);
    return;
  }
  
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    *result = 1;
  } else if (p == FINGERPRINT_NOTFOUND) {
    blinkLED(4, 100);
  } else {
    blinkLED(3, 200);
  }
}

// 开锁函数
void unlockDoor(bool bUnlock) {
  digitalWrite(OUTPUT_PIN, bUnlock ? HIGH : LOW);
  digitalWrite(LED_PIN, bUnlock ? LOW : HIGH);
}

// LED闪烁函数
void blinkLED(int times, int duration) {
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(duration);
    digitalWrite(LED_PIN, HIGH);
    if(i < times - 1) delay(duration);
  }
}

// 改进的按钮检测逻辑
bool checkButton(int pin, bool longPress) {
  static unsigned long pressTime[2] = {0, 0};
  static bool lastState[2] = {HIGH, HIGH};
  int index = (pin == ENROLL_PIN) ? 0 : 1;
  
  bool currentState = digitalRead(pin);
  
  // 状态变化检测
  if (currentState != lastState[index]) {
    delay(50); // 消抖
    currentState = digitalRead(pin);
    
    if (currentState == LOW && lastState[index] == HIGH) {
      // 按钮按下
      pressTime[index] = millis();
      lastState[index] = LOW;
      return !longPress; // 短按立即返回
    }
    else if (currentState == HIGH && lastState[index] == LOW) {
      // 按钮释放
      lastState[index] = HIGH;
    }
  }
  
  // 长按检测
  if (longPress && currentState == LOW) {
    if (millis() - pressTime[index] > 5000) {
      lastState[index] = HIGH; // 重置状态
      return true;
    }
  }
  
  return false;
}
