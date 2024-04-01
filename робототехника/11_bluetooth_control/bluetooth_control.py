import yaml, bluetooth
from pynput import keyboard

with open('config.yaml') as f:
    cfg = yaml.load(f, Loader=yaml.FullLoader)

prev = ''

def send_to_robot(cmd):
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((cfg['robot_blue'], 1))
    sock.send(cmd)
    sock.close()
    # sock.sendto(cmd, (cfg['robot_blue'], 3))

def on_press(key):
    global prev
    try:
        if str(key) == "'w'" and prev != "'w'":
            send_to_robot('MOV')
        elif str(key) == "'s'" and prev != "'s'":
            send_to_robot('STO')
    except:
        pass

with keyboard.Listener(on_press=on_press) as listener:
    listener.join()