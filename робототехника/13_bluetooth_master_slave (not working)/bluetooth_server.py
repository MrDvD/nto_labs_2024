import bluetooth

sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.bind(('', bluetooth.PORT_ANY))
sock.listen(3)
while True:
   client, addr = sock.accept()
   print(addr)
   msg = client.recv(1024)
   print(msg)
   client.close()