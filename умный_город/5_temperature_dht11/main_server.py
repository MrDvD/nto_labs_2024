import asyncio, re, json

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
        self.state = [100, 5, 255]
        self.post = re.compile(r'^POST')
        self.body = re.compile(r'({.*)$')
    
    async def start(self):
        server = await asyncio.start_server(self.on_connect, self.ip, self.port)
        async with server:
            print(f'Listening on [{self.port}]')
            await server.serve_forever()
    
    def decorate(self):
        result = ''
        for num in map(lambda x: bin(x)[2:], self.state):
            result += '0' * (8 - len(num)) + num
        return result

    async def on_connect(self, reader, writer):
        data = await reader.read(1024)
        message = data.decode()
        print(f'[{self.port}] Data has been received:\n{message}')
        if message == '1':
            # если в коде функции нет await, то её нельзя делать асинхронной
            result = self.decorate()
            print(result)
            writer.write((result + '\n').encode())
            await writer.drain()
        elif self.post.match(message):
            # лучше сначала присылать response, потом обрабатывать
            # (так в выводе спустя время появится искомая ошибка)
            writer.write((self.header).encode())
            await writer.drain()
            body = self.body.search(message)
            body = body.group(1)
            body = json.loads(body)
            for key in body.keys():
                self.state[int(key) - 1] = int(body[key])
        writer.close()
        await writer.wait_closed()

server = Server('192.168.6.242', 7000)

obj = Activator(server)
obj.elevate()
