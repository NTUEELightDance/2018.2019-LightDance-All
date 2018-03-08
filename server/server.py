import socketserver
import socket
import threading
import time
import sys

sys.path.append('../editor')
import translate
import json
import subprocess
import platform

HOST = ''
PORT = 33117
TIME_BASE = 0
START_DELAY = 5
MUSIC_DELAY = 0

Ready = False
Data = None

Status = ['.'] * 7

Shutdown = False

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):
    def handle(self):
        def send(x):
            self.request.sendall(bytes(x, 'ascii') + b'\0')
        def read():
            return str(self.request.recv(1024).strip(), 'ascii')
        cur_thread = threading.current_thread()

        try:
            send('N')
            num = int(read())
            if num < 1 or num > translate.N_DANCER:
                raise Exception('Board number {} out of range'.format(num))
            # print('Board No: {}'.format(num))
            send('O')
            operation = read()
            # print('Operation: {}'.format(operation))

            if operation[0] == 'T': #Time calibration
                if not Ready:
                    send('X')
                    return
                tm1 = time.time() - TIME_BASE
                send(str(tm1))
                res = read()
                if res[0] != 'S':
                    raise Exception('Calibration Step 1 Failed')
                tm2 = time.time() - TIME_BASE
                send(str(tm2))
                res = read()
                if res[0] != 'S':
                    raise Exception('Calibration Step 2 Failed')
                print('[ {} ] Calibration Successful! Time = {}, Delay = {}'.format(num, (tm1+tm2)/2, tm2-tm1))
                Status[num-1] = '^'

            elif operation[0] == 'D': #Data transfer
                dt = Data[num-1]
                Status[num-1] = 'D'
                js = json.dumps(dt, separators=(',', ':'))
                send(str(len(js)))
                send(js)
                res = read()
                if res[0] != 'S':
                    raise Exception('Send data failed')
                print('[ {} ] Data Transfer Successful! Len = {} bytes'.format(num, len(js)))
                Status[num-1] = 'R'

        except Exception as e:
            print(e)

class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

def report():
    while not Shutdown:
        print('[ ' + ' '.join(Status) + ' ]')
        time.sleep(1)

if __name__ == "__main__":
    Data = translate.translate('../editor/test.in')

    socketserver.TCPServer.allow_reuse_address = True
    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
    ip, port = server.server_address

    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True
    server_thread.start()
    print("Server loop running in thread:", server_thread.name)

    print_interval = 1

    report_thread = threading.Thread(target=report)
    report_thread.start()

    proc = False

    try:
        while True:
            Ready = False
            s = input()
            Ready = True
            TIME_BASE = time.time() + START_DELAY
            audio_played = False
            lst_time = -START_DELAY - print_interval
            while True:
                time.sleep(0.002)
                tm = time.time() - TIME_BASE
                if tm - lst_time >= print_interval:
                    lst_time += print_interval
                    print('Time : {}'.format(lst_time))
                if tm >= MUSIC_DELAY and not audio_played:
                    audio_played = True
                    print('Play!')
                    if platform.system() == 'Windows':
                        import winsound
                        winsound.PlaySound('..\\music\\disconnected_02.wav', winsound.SND_ASYNC)
                        #proc = subprocess.Popen(['powershell', '-c', '(New-Object Media.SoundPlayer \'..\\music\\disconnected_02.wav\').PlaySync();'])
                    elif platform.system() == 'Linux':
                        proc = subprocess.Popen(['aplay', '../music/disconnected_02.wav'])
                    else:
                        proc = subprocess.Popen(['afplay', '../music/disconnected_02.wav'])
    except KeyboardInterrupt:
        print('Shutdown')

    if proc is not False:
        proc.kill()
    if platform.system() == 'Windows':
        winsound.PlaySound(None, winsound.SND_PURGE)
    Shutdown = True
    server.shutdown()
    server.server_close()

