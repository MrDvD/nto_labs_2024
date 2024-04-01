import bluetooth

sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect(('C8:C9:A3:D1:53:1A', 1))
sock.send('Hi there')
sock.close()
# sock.sendto(cmd, (cfg['robot_blue'], 3))