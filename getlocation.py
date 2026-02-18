import cv2
import numpy as np
import sys

def find_template_location(large_image_path, small_image_path):
    large_img = cv2.imread(large_image_path)
    small_img = cv2.imread(small_image_path)
    
    if large_img is None:
        print(f"无法加载屏幕截图: {large_image_path}")
        return None
    
    if small_img is None:
        print(f"无法加载对比图: {small_image_path}")
        return None
    
    if small_img.shape[0] > large_img.shape[0] or small_img.shape[1] > large_img.shape[1]:
        print("对比图的尺寸大于屏幕截图")
        return None
    
    result = cv2.matchTemplate(large_img, small_img, cv2.TM_CCOEFF_NORMED)
    min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(result)
    
    if max_val < 0.75:
        print(f"匹配度较低: {max_val:.4f}")
        return None
    
    return {
        'x': max_loc[0],
        'y': max_loc[1],
        'width': small_img.shape[1],
        'height': small_img.shape[0],
        'confidence': max_val
    }

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("请传入参数")
        sys.exit(1)
    
    large_image_path = sys.argv[1]
    small_image_path = sys.argv[2]
    
    location = find_template_location(large_image_path, small_image_path)
    
    if location:
        center_x = location['x'] + location['width'] // 2
        center_y = location['y'] + location['height'] // 2
        
        # print(f"位置坐标: ({location['x']}, {location['y']})")
        print(f"中心坐标: ({center_x}, {center_y})")
        print(f"尺寸: {location['width']} x {location['height']}")
        print(f"匹配度: {location['confidence']:.4f}")
        
        with open('location.txt', 'w') as f:
            f.write(f"{center_x},{center_y}")
        # print("坐标已保存到 location.txt")
