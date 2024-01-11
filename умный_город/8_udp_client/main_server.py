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
    def connection_made(self, transport):
        print('asdasd')
        self.transport = transport
        self.is_closed = asyncio.Future()
    
    def datagram_received(self, data, addr):
        print(f'[RECEIVED_MSG] ({addr[0]}:{addr[1]}): {data.decode()}')
    
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
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: Datagram(),
            local_addr=(self.ip, self.port)
        )
        await protocol.serve_forever()

server = Server('192.168.76.242', 7001)

obj = Activator(server)
obj.elevate()