import cv2

from ultralytics import YOLO
# 加载预训练的 YOLOv8n 模型
model = YOLO('./runs/detect/train5/weights/best.pt')
#定义图像文件的路径

# capture = cv2.VideoCapture(0) # 打开笔记本内置摄像头
# #capture = cv2.VideoCapture(1, cv2.CAP_DSHOW) # 打开笔记本外置摄像头
# while (capture.isOpened()): # 笔记本内置摄像头被打开后
#     retval, image = capture.read() # 从摄像头中实时读取视频
#     image_Gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
#     #cv2.imshow("Video", image) # 在窗口中显示读取到的视频
#     cv2.imshow("Video_Gray", image)  # 在窗口中显示灰度视频
#     key = cv2.waitKey(1) # 窗口的图像刷新时间为1毫秒
#     if key == 32: # 如果按下空格键
#         image = cv2.imwrite("copy.png", image)  # 保存按下空格键时摄像头视频中的图像,注意是截图
#         cv2.imshow('img', image)  # 显示按下空格键时摄像头视频中的图像，新窗口名img
#         cv2.waitKey()  # 刷新图像
#         break
#
#
# capture.release() # 关闭笔记本内置摄像头
# cv2.destroyAllWindows() # 销毁显示摄像头视频的窗口
#
# source = 'copy.jpg' #更改为自己的图片路径
# # 运行推理，并附加参数
# model.predict(source, save=True)


# import cv2
# from ultralytics import YOLO
#
# 加载自己训练的 YOLOv8 模型
# model = YOLO('./runs/detect/train5/weights/best.pt')
#
# # 打开摄像头
# cap = cv2.VideoCapture(0)
#
# # 检查摄像头是否成功打开
# if not cap.isOpened():
#     print("无法打开摄像头")
#     exit()
#
# while True:
#     # 从摄像头读取一帧图像
#     ret, frame = cap.read()
#     if not ret:
#         print("无法从摄像头读取图像")
#         break
#
#     # 运行推理
#     results = model.predict(frame)
#
#     # 在图像上绘制识别结果
#     annotated_frame = results[0].plot()
#
#     # 显示图像
#     cv2.imshow('YOLOv8 Real-Time Object Detection', annotated_frame)
#
#     # 按下 'q' 键退出循环
#     if cv2.waitKey(1) & 0xFF == ord('q'):
#         break
#
# # 释放摄像头资源
# cap.release()
# # 销毁所有窗口
# cv2.destroyAllWindows()
#
# # 定义图像文件的路径
source = 'paper_2600.jpg'  # 更改为自己的图片路径
# # 运行推理，并附加参数
model.predict(source, save=True)


