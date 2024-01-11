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
    
    async def start(self):
        server = await asyncio.start_server(self.on_connect, self.ip, self.port)
        async with server:
            print(f'Listening on [{self.port}]')
            await server.serve_forever()

    async def on_connect(self, reader, writer):
        data = await reader.read(1024)
        message = data.decode()
        print(f'[{self.port}] Data has been received: {message}')

        writer.write(data)
        await writer.drain()
        # ОБЯЗАТЕЛЬНО поставь это в конец
        # (неважно, что записи не происходило)
        writer.close()
        await writer.wait_closed()

server1 = Server('192.168.0.20', 7001)
server2 = Server('192.168.0.20', 7002)
server3 = Server('192.168.0.20', 7003)

obj = Activator(server1, server2, server3)
obj.elevate()