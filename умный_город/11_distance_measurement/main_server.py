import asyncio

class Activator:
    def __init__(self, *servers):
        self.servers = list(servers)
    
    async def gather(self):
        start = list(map(lambda x: x.start(), self.servers))
        return await asyncio.gather(*start)

    def elevate(self):
        asyncio.run(self.gather())

class Datagram(asyncio.DatagramProtocol):
    def __init__(self, distance):
        super().__init__()
        self.distance = distance
    
    def binary_to_float(self, binary):
        sign = -1 if int(binary[0]) else 1
        pow_sign = -1 if int(binary[1]) else 1
        power = int(binary[2:8], 2)
        mantissa = int(binary[8:], 2)
        return round(sign * mantissa * 10 ** (pow_sign * power), 1)

    def connection_made(self, transport):
        print('[SERVER_ACTIVATED]')
        self.transport = transport
        self.is_closed = asyncio.Future()
    
    def datagram_received(self, data, addr):
        self.distance = self.binary_to_float(data.decode())
        print(f'[RECEIVED_MSG] ({addr[0]}:{addr[1]}): {self.distance}')
        peep = '0' if self.distance > 20 else '1'
        self.transport.sendto(peep.encode(), addr)
    
    def connection_lost(self, exception):
        print("[CONNECTION_CLOSED]")
    
    async def serve_forever(self):
        # this method is self-made
        if await self.is_closed:
            self.transport.close()

class Server:
    def __init__(self, ip, port):
        self.ip, self.port = ip, port
        self.distance = ''
    
    async def start(self):
        loop = asyncio.get_running_loop()
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: Datagram(self.distance),
            local_addr=(self.ip, self.port)
        )
        await protocol.serve_forever()

server = Server('192.168.76.242', 7001)

obj = Activator(server)
obj.elevate()