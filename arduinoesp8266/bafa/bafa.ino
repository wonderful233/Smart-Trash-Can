#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ------------------ 配置项 ------------------
const char* ssid = "";  //WIFI名称
const char* password = "";  //WIFI密码
const char* mqttServer = "bemfa.com";
const int mqttPort = 9501;
const char* uid = "";
const char* topic_bin1 = "garbagebin01";  // 设备主题（接收指令+发送数据）

// ------------------ 全局对象与变量 ------------------
WiFiClient espClient;
PubSubClient client(espClient);
char data_buf[128];  // 串口数据缓冲区

// ------------------ 处理云端指令 ------------------
void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.print("收到云端指令: ");
    Serial.println(message);

    // 转发指令给STM32（保持原有指令格式）
    Serial.println(message);  // 直接发送指令字符串（如"open1"）
}

// ------------------ 连接WiFi ------------------
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("连接到: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi连接成功");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
}

// ------------------ MQTT重连 ------------------
void reconnect() {
    while (!client.connected()) {
        Serial.print("尝试连接MQTT服务器...");
        if (client.connect(uid)) {
            Serial.println("连接成功！");
            client.subscribe(topic_bin1);
            Serial.print("已订阅主题: ");
            Serial.println(topic_bin1);
        } else {
            Serial.print("连接失败，rc=");
            Serial.print(client.state());
            Serial.println(" 5秒后重试...");
            delay(5000);
        }
    }
}

// ------------------ 接收STM32数据并上传 ------------------
void receiveFromSTM32() {
    static String stm32Data = "";
    while (Serial.available() > 0) {
        char c = Serial.read();
        // 按STM32发送的换行符分割数据（假设STM32用\r\n结尾）
        if (c == '\n') {
            // 移除可能的回车符
            stm32Data.trim();
            if (stm32Data.length() > 0) {
                Serial.print("收到STM32数据: ");
                Serial.println(stm32Data);
                
                // 上传数据到云端
                if (client.publish(topic_bin1, stm32Data.c_str())) {
                    Serial.println("数据已上传到云端");
                } else {
                    Serial.println("数据上传失败");
                }
                stm32Data = "";  // 清空缓冲区
            }
        } else {
            stm32Data += c;  // 拼接数据
        }
    }
}

// ------------------ 初始化 ------------------
void setup() {
    Serial.begin(115200);  // 与STM32通信的串口波特率
    setup_wifi();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

// ------------------ 主循环 ------------------
void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();  // 处理MQTT消息
    
    receiveFromSTM32();  // 检查并处理STM32发送的数据
}
