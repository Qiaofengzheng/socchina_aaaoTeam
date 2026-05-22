import mediapipe as mp
import cv2
import numpy as np
import serial
import time
import threading

# 加载预训练的人脸检测模型（OpenCV Haar级联分类器）
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

def get_angle(v1, v2):
    """
    计算两个向量之间的夹角（角度制）
    Args:
        v1, v2: 输入向量
    Returns:
        angle: 两向量夹角（0-180度）
    """
    # 计算向量点积
    dot_product = np.dot(v1, v2)
    # 计算向量模长
    norm_v1 = np.sqrt(np.sum(v1 * v1))
    norm_v2 = np.sqrt(np.sum(v2 * v2))
    # 计算余弦值并限制在[-1,1]范围内防止数值误差
    cos_angle = dot_product / (norm_v1 * norm_v2)
    cos_angle = np.clip(cos_angle, -1.0, 1.0)
    # 反余弦得到弧度，转换为角度
    angle = np.arccos(cos_angle) / 3.14 * 180
    return angle


def get_str_guester(up_fingers, list_lms):
    """
    基于手指关键点识别手势类型
    Args:
        up_fingers: 伸出的手指索引列表
        list_lms: 21个手部关键点坐标
    Returns:
        gesture: 识别出的手势字符串
    """
    # 握拳识别：没有伸出的手指
    if len(up_fingers) == 0:
        return "fist"  # 用"fist"表示握拳

    # 数字1识别：只有食指伸出
    if len(up_fingers) == 1 and up_fingers[0] == 8:
        # 计算食指关节角度
        v1 = list_lms[6] - list_lms[7]  # 食指近端指向
        v2 = list_lms[8] - list_lms[7]  # 食指远端指向
        angle = get_angle(v1, v2)
        # 角度过小可能是误检，忽略该手势
        if angle < 160:
            return " "  # 忽略9手势
        else:
            return "1"

    # 数字2识别：食指和中指伸出
    elif len(up_fingers) == 2 and up_fingers[0] == 8 and up_fingers[1] == 12:
        return "2"

    # 数字3和7识别：大拇指、食指、中指伸出
    elif len(up_fingers) == 3 and up_fingers[0] == 4 and up_fingers[1] == 8 and up_fingers[2] == 12:
        # 计算三指指尖之间的平均距离来区分3和7
        tips = [4, 8, 12]  # 大拇指、食指、中指指尖索引
        tip_distances = []

        # 计算所有指尖对之间的距离
        for i in range(len(tips)):
            for j in range(i + 1, len(tips)):
                # 欧几里得距离
                dist = np.sqrt((list_lms[tips[i]][0] - list_lms[tips[j]][0]) ** 2 +
                               (list_lms[tips[i]][1] - list_lms[tips[j]][1]) ** 2)
                tip_distances.append(dist)

        # 计算平均指尖距离
        avg_tip_dist = np.mean(tip_distances) if tip_distances else 0

        # 基于指尖距离阈值区分3和7
        # 三指聚拢(平均距离小) → 数字7，三指分开 → 数字3
        if avg_tip_dist < 60:  # 阈值可根据实际测试调整
            return "7"
        else:
            return "3"

    # 数字4识别：除小指外四指伸出
    elif len(up_fingers) == 4 and up_fingers[0] == 8 and up_fingers[1] == 12 and up_fingers[2] == 16 and up_fingers[
        3] == 20:
        return "4"

    # 数字5识别：五指全部伸出
    elif len(up_fingers) == 5:
        return "5"

    # 数字6识别：大拇指和小指伸出（"打电话"手势）
    elif len(up_fingers) == 2 and up_fingers[0] == 4 and up_fingers[1] == 20:
        return "6"

    else:
        return " "  # 其他未定义手势返回空


def init_serial(port, baudrate, max_retries=3):
    """
    初始化串口连接，支持重试机制
    Args:
        port: 串口名称（如'COM9'）
        baudrate: 波特率
        max_retries: 最大重试次数
    Returns:
        ser: 串口对象或None（失败时）
    """
    for i in range(max_retries):
        try:
            ser = serial.Serial(port, baudrate, timeout=0.5)
            if ser.isOpen():
                print(f"Successfully opened serial port {port}")
                return ser
        except Exception as e:
            print(f"Attempt {i + 1} failed: {e}")
            time.sleep(1)
    print(f"Failed to open serial port {port} after {max_retries} attempts")
    return None


def read_from_serial(ser, buffer):
    """
    串口数据读取线程函数
    Args:
        ser: 串口对象
        buffer: 数据缓冲区列表
    """
    while True:
        try:
            # 检查串口是否有可读数据
            if ser.in_waiting > 0:
                # 读取一行数据并解码
                data = ser.readline().decode('utf-8').strip()
                if data:
                    buffer.append(data)  # 添加到缓冲区
        except Exception as e:
            print(f"Serial read error: {e}")
            time.sleep(0.1)


if __name__ == "__main__":
    """
    主程序：多模态边缘智能稳像云台系统控制核心
    系统架构：
    - COM22: 手势控制串口（9600bps）
    - COM20: 人脸跟踪串口（115200bps）  
    - COM9:  MCU通信串口（115200bps）
    """
    # ========== 硬件初始化 ==========

    # 初始化三个串口连接
    serial_hand = init_serial('COM22', 9600)  # 手势控制
    serial_face = init_serial('COM20', 115200)  # 人脸跟踪
    serial_mcu = init_serial('COM9', 115200)  # MCU通信

    # 检查所有串口是否成功打开
    if not all([serial_hand, serial_face, serial_mcu]):
        print("Critical: Failed to initialize serial ports")
        exit(1)

    # ========== 多线程串口读取 ==========

    # 创建MCU数据缓冲区及读取线程
    mcu_buffer = []
    mcu_thread = threading.Thread(target=read_from_serial, args=(serial_mcu, mcu_buffer))
    mcu_thread.daemon = True  # 设置为守护线程
    mcu_thread.start()

    # ========== 视觉系统初始化 ==========

    # 初始化摄像头
    cap = cv2.VideoCapture(0)
    # 设置摄像头参数
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)  # 分辨率宽度
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)  # 分辨率高度
    cap.set(cv2.CAP_PROP_FPS, 30)  # 帧率

    # 初始化MediaPipe手势识别
    mpHands = mp.solutions.hands
    # 配置手势识别参数
    hands = mpHands.Hands(
        max_num_hands=1,  # 最大检测手数
        min_detection_confidence=0.7,  # 检测置信度阈值
        min_tracking_confidence=0.5  # 跟踪置信度阈值
    )
    mpDraw = mp.solutions.drawing_utils  # 绘制工具

    # ========== 系统状态变量 ==========

    # 手势识别稳定性检测
    coord_transmission_mode = False  # 坐标传输模式标志
    hand_stable_count = 0  # 手势稳定计数
    STABLE_THRESHOLD = 3  # 稳定阈值
    last_hand_flag = " "  # 上一帧手势
    current_hand_flag = " "  # 当前手势

    # 人脸跟踪稳定性滤波
    FACE_STABLE_THRESHOLD = 5  # 人脸坐标稳定窗口大小
    face_stable_coords = []  # 人脸坐标历史缓冲区

    # 数据发送控制
    last_sent_time = time.time()  # 上次发送时间
    last_hand_sent = ""  # 上次发送的手势
    last_face_sent = None  # 上次发送的人脸坐标
    last_time = time.time()  # 上一帧时间
    COORD_CHANGE_THRESHOLD = 5  # 坐标变化阈值（减少频繁发送）

    # 手势发送控制
    hand_send_count = 0  # 当前手势发送计数
    MAX_HAND_SEND_COUNT = 4  # 最大发送次数（确保指令可靠传输）
    current_sending_gesture = " "  # 当前正在发送的手势
    is_sending_gesture = False  # 是否正在发送手势
    last_gesture_change_time = 0  # 上次手势变化时间
    GESTURE_CHANGE_COOLDOWN = 0.5  # 手势切换冷却时间（防抖动）

    # MCU通信控制
    mcu_5_received = False  # 是否收到MCU的'5'指令
    mcu_5_send_count = 0  # '5'指令发送计数
    MCU_5_SEND_MAX = 4  # '5'指令最大发送次数

    # ========== 主循环 ==========

    while True:
        try:
            # 读取摄像头帧
            success, img = cap.read()
            if not success:
                continue  # 读取失败时跳过本帧

            # ========== MCU数据处理 ==========

            if mcu_buffer:
                data = mcu_buffer.pop(0)  # 从缓冲区取出数据
                if data == '5':
                    print("Received '5' from MCU - Voice activation detected")
                    mcu_5_received = True
                    mcu_5_send_count = 0
                    coord_transmission_mode = False  # 语音激活时先不开启坐标传输

            # ========== 图像预处理 ==========

            img = cv2.flip(img, 1)  # 水平翻转（镜像显示）
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)  # 转为灰度图（人脸检测用）
            imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # 转为RGB（MediaPipe用）

            # ========== 人脸检测与跟踪 ==========

            current_face_coord = None
            # 使用Haar级联分类器检测人脸
            faces = face_cascade.detectMultiScale(
                gray,
                scaleFactor=1.1,  # 图像缩放比例
                minNeighbors=5,  # 最小邻居数
                minSize=(50, 50)  # 最小检测尺寸
            )

            if len(faces) > 0:
                # 选择最大的人脸（假设为主要目标）
                max_face = max(faces, key=lambda rect: rect[2] * rect[3])
                x, y, w, h = max_face

                # 计算人脸中心坐标
                center_x = x + w // 2
                center_y = y + h // 2

                # 将像素坐标转换为云台角度（基于图像中心）
                # 角度映射：图像中心对应94°，边缘对应0°或180°
                angle_y = int(94 - (center_x - 320) * (86 / 320))  # 水平角度
                angle_x = int(94 - (center_y - 240) * (94 / 240))  # 垂直角度

                # 人脸坐标稳定性滤波（移动平均）
                face_stable_coords.append((angle_x, angle_y))
                if len(face_stable_coords) > FACE_STABLE_THRESHOLD:
                    face_stable_coords.pop(0)  # 保持固定窗口大小

                # 当有足够的历史数据时计算平均坐标
                if len(face_stable_coords) == FACE_STABLE_THRESHOLD:
                    avg_x = sum(coord[0] for coord in face_stable_coords) / FACE_STABLE_THRESHOLD
                    avg_y = sum(coord[1] for coord in face_stable_coords) / FACE_STABLE_THRESHOLD
                    current_face_coord = (int(avg_x), int(avg_y))  # 稳定后的人脸坐标

                # 在图像上绘制人脸矩形框
                cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)

            # ========== 手势识别 ==========

            hand_flag = " "  # 当前有效手势
            results = hands.process(imgRGB)  # 处理图像进行手势识别

            if results.multi_hand_landmarks:
                # 获取第一只手的 landmarks
                hand = results.multi_hand_landmarks[0]
                # 在图像上绘制手部关键点和连接线
                mpDraw.draw_landmarks(img, hand, mpHands.HAND_CONNECTIONS)

                # 提取21个手部关键点的像素坐标
                list_lms = []
                for i in range(21):
                    pos_x = hand.landmark[i].x * img.shape[1]  # 归一化坐标转像素坐标
                    pos_y = hand.landmark[i].y * img.shape[0]
                    list_lms.append([int(pos_x), int(pos_y)])

                list_lms = np.array(list_lms)

                # 计算手部凸包（用于判断手指是否伸出）
                hull_index = [0, 1, 2, 3, 6, 10, 14, 19, 18, 17, 10]  # 手掌轮廓关键点
                hull = cv2.convexHull(list_lms[hull_index, :])

                # 检测伸出的手指
                up_fingers = []
                finger_tips = [4, 8, 12, 16, 20]  # 指尖关键点索引（拇指、食指、中指、无名指、小指）
                for i in finger_tips:
                    pt = (int(list_lms[i][0]), int(list_lms[i][1]))
                    # 如果指尖在凸包外部，则认为该手指伸出
                    if cv2.pointPolygonTest(hull, pt, True) < 0:
                        up_fingers.append(i)

                # 手势识别
                current_hand_flag = get_str_guester(up_fingers, list_lms)

                # ========== 手势可视化调试 ==========

                # 对于3/7手势，显示指尖距离信息（调试用）
                if len(up_fingers) == 3 and set(up_fingers) == {4, 8, 12}:
                    tips = [4, 8, 12]  # 大拇指、食指、中指
                    total_dist = 0
                    count = 0
                    # 计算所有指尖对之间的距离并绘制连线
                    for i in range(len(tips)):
                        for j in range(i + 1, len(tips)):
                            x1, y1 = list_lms[tips[i]]
                            x2, y2 = list_lms[tips[j]]
                            dist = np.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)
                            cv2.line(img, (x1, y1), (x2, y2), (0, 255, 255), 2)  # 黄色连线
                            total_dist += dist
                            count += 1
                    # 显示平均指尖距离
                    if count > 0:
                        avg_dist = total_dist / count
                        cv2.putText(img, f"Tip Dist: {avg_dist:.1f}", (50, 80),
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)

                # ========== 手势稳定性检测 ==========

                # 连续多帧手势相同才认为是稳定手势
                if current_hand_flag == last_hand_flag:
                    hand_stable_count += 1
                else:
                    hand_stable_count = 0  # 手势变化时重置计数
                    last_hand_flag = current_hand_flag

                # 达到稳定阈值，确认当前手势
                if hand_stable_count >= STABLE_THRESHOLD:
                    hand_flag = current_hand_flag
                    # 在图像上显示识别到的手势
                    cv2.putText(img, hand_flag, (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 255), 2)

            # ========== 多模态控制逻辑 ==========

            current_time = time.time()

            # 手势"5"：开启人脸跟踪模式
            if hand_flag == "5":
                if not mcu_5_received:  # 只有当不是由MCU触发的5时才处理（避免冲突）
                    coord_transmission_mode = True
                    if current_sending_gesture != "5":
                        hand_send_count = 0
                        current_sending_gesture = "5"
                        is_sending_gesture = True
                        last_gesture_change_time = current_time

            # 手势"fist"：停止所有跟踪
            elif hand_flag == "fist":
                coord_transmission_mode = False
                mcu_5_received = False  # 重置MCU触发标志
                if current_sending_gesture != "fist":
                    hand_send_count = 0
                    current_sending_gesture = "fist"
                    is_sending_gesture = True
                    last_gesture_change_time = current_time

            # 其他数字手势：执行相应功能
            elif hand_flag in ["1", "2", "3", "4", "6", "7"]:
                coord_transmission_mode = False
                mcu_5_received = False  # 重置MCU触发标志
                # 防抖动：手势变化需要超过冷却时间
                if current_sending_gesture != hand_flag and (
                        current_time - last_gesture_change_time) > GESTURE_CHANGE_COOLDOWN:
                    hand_send_count = 0
                    current_sending_gesture = hand_flag
                    is_sending_gesture = True
                    last_gesture_change_time = current_time

            else:
                # 无有效手势
                current_sending_gesture = " "
                is_sending_gesture = False

            # ========== 串口数据发送 ==========

            # 控制发送频率：每100ms发送一次
            if current_time - last_sent_time > 0.1:

                # 1. 处理MCU触发的'5'指令转发
                if mcu_5_received and mcu_5_send_count < MCU_5_SEND_MAX:
                    try:
                        serial_hand.write("5\n".encode())  # 转发到手势串口
                        mcu_5_send_count += 1
                        print(f"Sent 5 via COM22: {mcu_5_send_count}/{MCU_5_SEND_MAX}")
                        # 发送完指定次数后开启坐标传输
                        if mcu_5_send_count >= MCU_5_SEND_MAX:
                            coord_transmission_mode = True
                    except serial.SerialException as e:
                        print(f"COM22 write error: {e}")

                # 2. 发送手势指令
                if is_sending_gesture and hand_send_count < MAX_HAND_SEND_COUNT and current_sending_gesture in ["1",
                                                                                                                "2",
                                                                                                                "3",
                                                                                                                "4",
                                                                                                                "5",
                                                                                                                "6",
                                                                                                                "7",
                                                                                                                "fist"]:
                    try:
                        # 握拳时发送停止指令"0"，其他手势发送对应数字
                        if current_sending_gesture == "fist":
                            serial_hand.write("0\n".encode())  # 停止指令
                        else:
                            serial_hand.write(f"{current_sending_gesture}\n".encode())  # 数字指令

                        hand_send_count += 1
                        print(
                            f"Sent hand via COM22: {current_sending_gesture} ({hand_send_count}/{MAX_HAND_SEND_COUNT})")
                        # 达到最大发送次数后停止发送
                        if hand_send_count >= MAX_HAND_SEND_COUNT:
                            is_sending_gesture = False
                    except serial.SerialException as e:
                        print(f"COM22 write error: {e}")

                # 3. 发送人脸跟踪坐标
                if coord_transmission_mode and current_face_coord:
                    # 只在坐标变化超过阈值时才发送（减少通信负载）
                    if last_face_sent is None or \
                            abs(current_face_coord[0] - last_face_sent[0]) > COORD_CHANGE_THRESHOLD or \
                            abs(current_face_coord[1] - last_face_sent[1]) > COORD_CHANGE_THRESHOLD:
                        try:
                            # 发送格式："角度X,角度Y"
                            serial_face.write(f"{current_face_coord[0]},{current_face_coord[1]}\n".encode())
                            print(f"Sent face via COM20: {current_face_coord}")
                            last_face_sent = current_face_coord
                        except serial.SerialException as e:
                            print(f"COM20 write error: {e}")

                last_sent_time = current_time  # 更新最后发送时间

            # ========== 界面显示 ==========

            # 显示系统状态信息
            mode_text = f"COM22(Hand): {current_sending_gesture} ({hand_send_count}/{MAX_HAND_SEND_COUNT}) | COM20(Face): {'ON' if coord_transmission_mode else 'OFF'} | COM9(MCU): Active"
            if mcu_5_received:
                mode_text += f" | MCU-5: {mcu_5_send_count}/{MCU_5_SEND_MAX}"
            cv2.putText(img, mode_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)

            # 计算并显示帧率
            fps = 1 / (current_time - last_time)
            cv2.putText(img, f"FPS: {fps:.1f}", (10, img.shape[0] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1)
            last_time = current_time

            # 显示处理后的图像
            cv2.imshow("Gesture and Face Detection", img)

            # 按'q'键退出
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        except Exception as e:
            # 异常处理：重新初始化资源
            print(f"Error in main loop: {e}")
            cap.release()
            cap = cv2.VideoCapture(0)  # 重新初始化摄像头

            # 重新初始化断开的串口
            if not serial_hand.isOpen():
                serial_hand = init_serial('COM22', 9600)
            if not serial_face.isOpen():
                serial_face = init_serial('COM20', 115200)
            if not serial_mcu.isOpen():
                serial_mcu = init_serial('COM9', 115200)

    # ========== 资源清理 ==========

    # 释放摄像头
    cap.release()
    # 关闭所有OpenCV窗口
    cv2.destroyAllWindows()
    # 关闭所有串口连接
    for ser in [serial_hand, serial_face, serial_mcu]:
        if ser.isOpen():
            ser.close()