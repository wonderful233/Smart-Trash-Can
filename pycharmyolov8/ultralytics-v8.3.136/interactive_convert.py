# interactive_convert.py
import os
from PIL import Image
import tkinter as tk
from tkinter import filedialog, messagebox


def select_folder_and_convert():
    """选择文件夹并转换图片"""
    root = tk.Tk()
    root.withdraw()  # 隐藏主窗口

    # 选择文件夹
    folder_path = filedialog.askdirectory(title="选择包含图片的文件夹")
    if not folder_path:
        return

    # 确认
    if not messagebox.askyesno("确认", f"将转换文件夹中的RGBA图片:\n{folder_path}\n\n继续吗？"):
        return

    converted = []
    failed = []

    for filename in os.listdir(folder_path):
        if filename.lower().endswith(('.png', '.jpg', '.jpeg')):
            img_path = os.path.join(folder_path, filename)
            try:
                img = Image.open(img_path)
                if img.mode == 'RGBA':
                    rgb = Image.new('RGB', img.size, (255, 255, 255))
                    rgb.paste(img, mask=img.split()[3])

                    if filename.lower().endswith('.png'):
                        new_name = filename[:-4] + '_converted.jpg'
                    else:
                        name, ext = os.path.splitext(filename)
                        new_name = f"{name}_converted{ext}"

                    rgb.save(os.path.join(folder_path, new_name))
                    converted.append(f"{filename} -> {new_name}")
                else:
                    print(f"{filename}: {img.mode}模式，无需转换")
            except Exception as e:
                failed.append(f"{filename}: {str(e)}")

    # 显示结果
    result = f"转换完成！\n\n成功: {len(converted)}张\n失败: {len(failed)}张"

    if converted:
        result += "\n\n转换成功的图片：\n" + "\n".join(converted[:10])
        if len(converted) > 10:
            result += f"\n... 还有{len(converted) - 10}张"

    if failed:
        result += "\n\n失败的图片：\n" + "\n".join(failed)

    messagebox.showinfo("结果", result)


if __name__ == "__main__":
    select_folder_and_convert()