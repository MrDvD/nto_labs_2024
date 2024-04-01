import bluetooth

sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.bind(('74:C6:3B:38:A4:A0', 1))
sock.listen(1)
while True:
    print('asdasdasd')
    client, addr = sock.accept()
    msg = client.recv(1024)
    print(msg.decode())
    client.close()
sock.close()