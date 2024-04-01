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
        self.transport = transport
        self.is_closed = asyncio.Future()
    
    async def handle_1(self):
        print('start 1')
        await asyncio.sleep(5)
        print('end 1')
    
    async def handle_2(self):
        print('start 2')
        await asyncio.sleep(5)
        print('end 2')
    
    async def handle_3(self):
        print('start 3')
        await asyncio.sleep(5)
        print('end 3')

    async def handle_data(self, data, addr):
        data = data.decode()
        print(f'[RECEIVED_MSG] ({addr[0]}:{addr[1]}): {data}')
        cmds = data.split()
        to_do = list()
        if '1' in cmds:
            to_do.append(self.handle_1())
        if '2' in cmds:
            to_do.append(self.handle_2())
        if '3' in cmds:
            to_do.append(self.handle_3())
        await asyncio.gather(*to_do)
    
    def datagram_received(self, data, addr):
        loop = asyncio.get_running_loop()
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
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: Datagram(),
            local_addr=(self.ip, self.port)
        )
        await protocol.serve_forever()

server = Server('192.168.141.242', 7001)

obj = Activator(server)
obj.elevate()