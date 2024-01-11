import asyncio

class Activator:
    def __init__(self, *servers):
        self.servers = list(servers)
    
    async def gather(self):
        start = list(map(lambda x: x.start(), self.servers))
        return await asyncio.gather(*start)

    def elevate(self):
        asyncio.run(self.gather())

class Server:
    def __init__(self, ip, port):
        self.ip, self.port = ip, port
        self.idx = 0
    
    async def start(self):
        server = await asyncio.start_server(self.on_connect, self.ip, self.port)
        async with server:
            print(f'Listening on [{self.port}]')
            await server.serve_forever()

    async def on_connect(self, reader, writer):
        data = await reader.read(1024)
        message = data.decode()
        print(f'[{self.port}] Data has been received:\n{message}')
        if message == '1':
            self.idx = (self.idx + 1) % 256
            print(self.idx)
            writer.write((str(self.idx) + '\n').encode())
            await writer.drain()
        writer.close()
        await writer.wait_closed()

server = Server('192.168.6.242', 7000)

obj = Activator(server)
obj.elevate()
