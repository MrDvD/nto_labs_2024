from pynput import keyboard

prev = ''

def send_to_robot(cmd):
    with open('cmd', 'w') as f:
        f.write(cmd)

def on_press(key):
    global prev
    try:
        if str(key) == "'w'" and prev != "'w'":
            send_to_robot('MOV')
        elif str(key) == "'s'" and prev != "'s'":
            send_to_robot('STO')
    except:
        pass

def on_release(key):
    global prev
    try:
        if str(key) == "'w'":
            send_to_robot('MOV')
            prev = ''
        elif str(key) == "'s'":
            send_to_robot('STO')
            prev = ''
    except:
        pass

with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()