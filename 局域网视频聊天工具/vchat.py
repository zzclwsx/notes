from socket import *
import threading
import cv2
import sys
import struct
import pickle
import time
import zlib
import numpy as np
#服务器类，发送本机相机信息
print(gethostname())
class Video_Server(threading.Thread):#继承多线程类，重载run函数实现多线程
    def __init__(self, port, version) :
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.ADDR = ('', port)

        # 创建套接字，输入参数决定套接字网络协议
        if version == 4:
            self.sock = socket(AF_INET, SOCK_STREAM)  #创建IPv4
        else:
            self.sock = socket(AF_INET6, SOCK_STREAM)  #创建IPv6
    def __del__(self):
        self.sock.close()#关闭连接
        cv2.destroyAllWindows()#关闭窗口
    def run(self):#重写run函数
        print("VEDIO server starts...")

        self.sock.bind(self.ADDR)#把地址和端口跟socket绑定
        self.sock.listen(1)#侦听传入连接，允许1个设备连接
        conn, addr = self.sock.accept()#接受客户端的请求，创建一个新的套接字
        print("remote VEDIO client success connected...")
        data = "".encode("utf-8")#把传到服务器的二进制码转换为utf-8编码
        payload_size = struct.calcsize("L")
        cv2.namedWindow('Remote', cv2.WINDOW_NORMAL)#创建窗口
        while True:
            while len(data) < payload_size:
                data += conn.recv(81920)#服务器接受最多81920字节数据
            packed_size = data[:payload_size]#压缩后的编码长度信息
            data = data[payload_size:]#data就是client里面压缩的zdata信息
            msg_size = struct.unpack("L", packed_size)[0]#解压长度信息，存到msg_size 变量中
            while len(data) < msg_size:
                data += conn.recv(81920)#接收图片数据
            zframe_data = data[:msg_size]#切片取出后面的图片信息，zframe_data是压缩形式的图片信息
            data = data[msg_size:]
            frame_data = zlib.decompress(zframe_data)#解压缩
            frame = pickle.loads(frame_data)#从字节对象读取被封装的图片的信息
            #cv2.putText(frame, "MY_FRIEND", (50, 100), cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 50, 50), 2, cv2.LINE_AA)
            cv2.imshow('Remote', frame)
            if cv2.waitKey(1) & 0xFF == 27:#按esc退出
                break

class Video_Client(threading.Thread):
    def __init__(self, ip, port, showme, level, version):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.ADDR = (ip, port)
        self.showme = showme
        if level == 0:
            self.interval = 0
        elif level == 1:
            self.interval = 1
        elif level == 2:
            self.interval = 2
        else:
            self.interval = 3#决定要丢弃几帧。
        self.fx = 1 / (self.interval + 1)
        if self.fx < 0.3:
            self.fx = 0.3
        if version == 4:
            self.sock = socket(AF_INET, SOCK_STREAM)
        else:
            self.sock = socket(AF_INET6, SOCK_STREAM)
        self.cap = cv2.VideoCapture(0)#从摄像头获取读取视频
        print("VEDIO client starts...")
    def __del__(self) :
        self.sock.close()
        self.cap.release()
        if self.showme:
            try:
                cv2.destroyAllWindows()
            except:
                pass
    def run(self):
        while True:
            try:
                self.sock.connect(self.ADDR)#发送连接请求到服务端
                break
            except:
                time.sleep(3)
                continue
        if self.showme:
            cv2.namedWindow('You', cv2.WINDOW_NORMAL)#创建窗口
        print("VEDIO client connected...")
        while self.cap.isOpened():
            ret, frame = self.cap.read()#从摄像头获取一帧图像， 后续显示和传输。
            if self.showme:
                #cv2.putText(frame,"MYSELF",(50,100),cv2.FONT_HERSHEY_SIMPLEX,2,(255,50,50),2,cv2.LINE_AA)
                cv2.imshow('You', frame)
                if cv2.waitKey(1) & 0xFF == 27:#按esc退出
                    self.showme = False
                    cv2.destroyWindow('You')
            sframe = cv2.resize(frame, (0, 0), fx=self.fx, fy=self.fx)
            data = pickle.dumps(sframe)#将图片封装成字节对象
            zdata = zlib.compress(data, zlib.Z_BEST_COMPRESSION)#压缩图像
            try:
                self.sock.sendall(struct.pack("L", len(zdata)) + zdata)#发送到服务器端，因为要压缩和解压缩，所以连同压缩后的编码长度一起传给服务器
            except:#               将字节解释为压缩的二进制数据
                break
            for i in range(self.interval):
                self.cap.read()#读取并且丢弃帧数，提高效率
