import asyncio, re

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
        self.header = 'HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n'
        self.state = 1
        self.post = re.compile(r'^POST ')
    
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
            print('worked')
            print((str(self.state) + '\n').encode())
            writer.write((str(self.state) + '\n').encode())
            await writer.drain()
            writer.close()
            await writer.wait_closed()
        elif self.post.match(message):
            self.state = message[-1]

server = Server('192.168.119.242', 7000)

obj = Activator(server)
obj.elevate()