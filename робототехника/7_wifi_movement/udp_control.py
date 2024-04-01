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
        if str(key) == "'w'" and prev != "'w'":
            send_to_robot('MOV F')
            prev = "'w'"
        elif str(key) == "'s'" and prev != "'s'":
            send_to_robot('MOV B')
            prev = "'s'"
        elif str(key) == "'d'" and prev != "'d'":
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
        if str(key) == "'w'":
            send_to_robot('MOV B')
            prev = ''
        elif str(key) == "'s'":
            send_to_robot('MOV F')
            prev = ''
        elif str(key) == "'d'":
            send_to_robot('MOV L')
            prev = ''
        elif str(key) == "'a'":
            send_to_robot('MOV R')
            prev = ''
    except:
        pass

with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()
