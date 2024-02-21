import asyncio, sqlalchemy, yaml, json
import pandas as pd

with open('config.yaml') as f:
    cfg = yaml.load(f, Loader=yaml.FullLoader)

class Activator:
    def __init__(self, *servers):
        self.servers = list(servers)
    
    async def gather(self):
        start = list(map(lambda x: x.start(), self.servers))
        return await asyncio.gather(*start)

    def elevate(self):
        asyncio.run(self.gather())

class Datagram(asyncio.DatagramProtocol):
    def __init__(self, update):
        super().__init__()
        self.update = update
    
    def binary_to_int(self, binary):
        sign = -1 if int(binary[0]) else 1
        result = int(binary[1:], 2)
        return sign * result

    def connection_made(self, transport):
        print('[SERVER_ACTIVATED]')
        self.transport = transport
        self.is_closed = asyncio.Future()
    
    async def handle_data(self, data, addr):
        data = data.decode()
        print(data)
        if data == '1':
            self.transport.sendto((await self.update).encode(), addr)
            self.update = asyncio.Future()
        else:
            charge, state, users = self.binary_to_int(data[0:8]), self.binary_to_int(data[8:16]), self.binary_to_int(data[16:24])
            print(charge, state, users)
            data = {'charge': charge, 'state': state, 'nusr': users}
            with open('data.json', 'w') as f:
                json.dump(data, f)
            if not self.update.done():
                self.update.set_result('1')

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
        self.update = asyncio.Future()
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: Datagram(self.update),
            local_addr=(self.ip, self.port)
        )
        await protocol.serve_forever()

server = Server(cfg['server_ip'], cfg['server_port'])

obj = Activator(server)
obj.elevate()