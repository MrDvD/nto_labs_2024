import yaml, socket
from pynput import keyboard

with open('config.yaml') as f:
    cfg = yaml.load(f, Loader=yaml.FullLoader)

prev = ''

def send_to_robot(cmd):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        # sending to iotik
        sock.sendto(cmd.encode(), (cfg['robot_ip'], cfg['robot_port']))

def on_press(key):
    global prev
    try:
        if str(key) == "'d'" and prev != "'d'":
            send_to_robot('MOV R')
            prev = "'d'"
        elif str(key) == "'a'" and prev != "'a'":
            send_to_robot('MOV L')
            prev = "'a'"
        elif str(key) == "'e'" and prev != "'e'":
            send_to_robot('MOV E')
            prev = "'e'"
    except:
        pass

def on_release(key):
    global prev
    try:
        if str(key) == "'d'":
            send_to_robot('MOV S')
            prev = ''
        elif str(key) == "'a'":
            send_to_robot('MOV S')
            prev = ''
    except:
        pass

with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()
