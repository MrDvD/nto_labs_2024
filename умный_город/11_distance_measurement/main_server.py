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
        self.cache = 5
    
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
    
    async def handle_data(self, data, addr):
        data = data.decode()
        if len(data) == 34:
            self.cache = self.binary_to_float(data[:-2])
            self.distance.set_result(self.cache)
            print(self.cache)
            peep = '0' if self.distance.result() > 20 else '1'
            self.transport.sendto(peep.encode(), addr)
            self.distance = asyncio.Future()
        elif data == '0':
            self.transport.sendto(str(self.cache).encode(), addr)
        elif data == '1':
            self.transport.sendto(str(await self.distance).encode(), addr)
    
    def datagram_received(self, data, addr):
        loop = asyncio.get_event_loop()
        loop.create_task(self.handle_data(data, addr))
    
    def connection_lost(self, exception):
        print("[CONNECTION_CLOSED]")
    
    async def serve_forever(self):
        # this method is self-made
        if await self.is_closed:
            self.transport.close()

class Server:
    def __init__(self, ip, port):
        self.ip, self.port = ip, port
    
    async def start(self):
        loop = asyncio.get_running_loop()
        self.distance = asyncio.Future()
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: Datagram(self.distance),
            local_addr=(self.ip, self.port)
        )
        await protocol.serve_forever()

server = Server('192.168.45.242', 7001)

obj = Activator(server)
obj.elevate()