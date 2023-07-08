import socket
import sys
import time
import argparse
from vchat import Video_Server, Video_Client
from achat import Audio_Server, Audio_Client

parser = argparse.ArgumentParser()
print("本机的无线局域网ipv4地址:"+socket.gethostbyname_ex(socket.gethostname())[2][1])
flag = int(input("选择1:视频通话，2:仅语音通话："))
choise = input('你想要远程视频通话吗(y/n):')
if (choise == 'y'):
    IP = input("输入对方的IP地址:")
    PORT = 10086
    VERSION = 4
    SHOWME = True
    LEVEL = int(input("选择流畅度（1，2，3）:"))
else:
    parser.add_argument('--host', type=str, default='127.0.0.1')#主机地址，127.0.0.1发送到本机
    parser.add_argument('--port', type=int, default=10086)#传入端口，随便传一个，但是要服务器端要和客户端一致
    parser.add_argument('--noself', type=bool, default=False)
    parser.add_argument('--level', type=int, default=3)
    parser.add_argument('-v', '--version', type=int, default=4)
    args = parser.parse_args()
    IP = args.host
    PORT = args.port
    VERSION = args.version
    SHOWME = not args.noself
    LEVEL = args.level

if __name__ == '__main__':
    vclient = Video_Client(IP, PORT, SHOWME, LEVEL, VERSION)
    vserver = Video_Server(PORT, VERSION)
    aclient = Audio_Client(IP, PORT+1, VERSION)
    aserver = Audio_Server(PORT+1, VERSION)
    if (flag == 1):
        vclient.start()
        vserver.start()
    aclient.start()
    aserver.start()
    while True:
        time.sleep(1)
        if (not vserver.is_alive() or not vclient.is_alive()) and flag == 1:
            print("Video connection lost...")
            sys.exit(0)
        if not aserver.is_alive() or not aclient.is_alive():
            print("Audio connection lost...")
            sys.exit(0)