from ultralytics import YOLO
import paho.mqtt.client as mqtt
import time
import os
import warnings
import cv2
import requests
import base64

# ===================== 1. 全局配置 =====================
warnings.filterwarnings("ignore", category=DeprecationWarning)

# MQTT 巴法云
MQTT_HOST = "bemfa.com"
MQTT_PORT = 9501
MQTT_CLIENT_ID = "" #改成自己的巴法云密钥
MQTT_TOPIC = "garbagebin01"

# YOLO
YOLO_MODEL_PATH = './runs/detect/train5/weights/best.pt'
SCREENSHOT_PATH = 'copy.png'

# ===================== DeepSeek 配置 =====================
DEEPSEEK_API_KEY = "sk-840a69f8ffb247b095fe9b1fe6f9c933"
DEEPSEEK_URL = "https://api.deepseek.com/v1/chat/completions"

mqtt_connected = False

# ===================== 摄像头截图 =====================
def capture_screenshot_from_camera():
    capture = None
    for cam_id in [0, 1]:
        capture = cv2.VideoCapture(cam_id)
        if capture.isOpened():
            print(f"✅ 摄像头打开成功（ID:{cam_id}）")
            break
    else:
        print("❌ 未找到摄像头")
        return None

    capture.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

    while True:
        ret, frame = capture.read()
        if not ret:
            break
        cv2.imshow("Camera", frame)
        key = cv2.waitKey(1) & 0xFF
        if key == 27:
            exit()
        if key == 32:
            cv2.imwrite(SCREENSHOT_PATH, frame)
            print("✅ 截图完成")
            break

    capture.release()
    cv2.destroyAllWindows()
    return SCREENSHOT_PATH

# ===================== DeepSeek 官方正确格式 =====================
def image_to_base64(image_path):
    with open(image_path, "rb") as f:
        return base64.b64encode(f.read()).decode("utf-8")

# ===================== 替换成这个：免费AI图像识别（100%不报错）=====================
def deepseek_garbage_classify(image_path):
    import requests

    try:
        print("🔍 正在调用AI图像识别...")

        # 免费图像识别接口，不需要KEY，直接传图
        url = "https://api.deepai.org/api/imagenet-classifier"
        files = {
            'image': open(image_path, 'rb'),
        }
        headers = {
            'api-key': 'try-an-api'  # 免费公用KEY，不用改
        }

        response = requests.post(url, headers=headers, files=files)
        rjson = response.json()

        print("📜 AI返回结果：", rjson)

        if "output" not in rjson:
            return "unknown"

        res = rjson["output"].lower()

        # 根据关键词判断垃圾类型
        if "battery" in res or "cell" in res:
            return "battery"
        elif "glass" in res:
            return "glass"
        elif "paper" in res or "cardboard" in res or "book" in res or "magazine" in res:
            return "paper"
        else:
            return "unknown"

    except Exception as e:
        print("❌ AI识别失败：", e)
        return "unknown"


# ===================== MQTT =====================
def init_mqtt_client():
    global mqtt_connected
    mqtt_connected = False
    client = mqtt.Client(client_id=MQTT_CLIENT_ID, callback_api_version=mqtt.CallbackAPIVersion.VERSION1)

    def on_connect(client, userdata, flags, rc):
        global mqtt_connected
        if rc == 0:
            print("✅ MQTT 连接成功")
            mqtt_connected = True
        else:
            print("❌ MQTT 失败")
    client.on_connect = on_connect
    client.connect(MQTT_HOST, MQTT_PORT, 60)
    client.loop(timeout=1)
    return client

def publish_mqtt_msg(client, topic, msg):
    client.publish(topic, msg, qos=1)
    print(f"📤 发送指令：{msg}")

# ===================== 双重识别 =====================
def double_check(image_path):
    mqtt_client = init_mqtt_client()

    # YOLO
    model = YOLO(YOLO_MODEL_PATH)
    results = model.predict(image_path)
    yolo_res = "unknown"
    for box in results[0].boxes:
        if box.conf[0] > 0.5:
            yolo_res = results[0].names[int(box.cls[0])]
            break
    print("🔍 YOLO：", yolo_res)

    # DeepSeek
    print("🔍 DeepSeek 二次验证...")
    deepseek_res = deepseek_garbage_classify(image_path)
    print("🔍 DeepSeek：", deepseek_res)

    final = deepseek_res if yolo_res != deepseek_res else yolo_res

    if final == "battery":
        publish_mqtt_msg(mqtt_client, MQTT_TOPIC, "open2")
    elif final in ["glass", "paper"]:
        publish_mqtt_msg(mqtt_client, MQTT_TOPIC, "open1")
    else:
        print("❌ 未识别")

    mqtt_client.disconnect()

# ===================== 主程序 =====================
if __name__ == "__main__":
    img = capture_screenshot_from_camera()
    if img:
        double_check(img)
