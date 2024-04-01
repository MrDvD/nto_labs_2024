import bluetooth

while True:
    try:
        sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        sock.bind(('74:C6:3B:38:A4:A0', 1))
        sock.listen(1)
        while True:
            client, addr = sock.accept()
            msg = client.recv(1024)
            print(msg.decode())
            with open('cmd') as f:
                cmd = f.read()
                client.send(cmd)
            client.close()
    except:
        print('CONNECTION RESET')
        continue