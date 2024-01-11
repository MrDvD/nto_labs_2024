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
        self.init = re.compile(r'^GET /init')
        self.update = re.compile(r'^GET /update')
        self.header = 'HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n'
        self.state = 'Not Updated'
    
    async def start(self):
        server = await asyncio.start_server(self.on_connect, self.ip, self.port)
        # таким синтаксисом Future создаётся в текущем loop-потоке
        # метод __init__ синхронный, поэтому loop-поток отсутствует
        self.future = asyncio.Future()
        async with server:
            print(f'Listening on [{self.port}]')
            await server.serve_forever()

    async def on_connect(self, reader, writer):
        data = await reader.read(1024)
        message = data.decode()
        print(f'[{self.port}] Data has been received: {message}')

        if self.init.match(message):
            # возвращает текущее состояние новому клиенту
            writer.write((self.header + self.state).encode())
            await writer.drain()
        elif self.update.match(message):
            # ожидание обновлённого значения
            update = await self.future
            writer.write((self.header + update).encode())
            await writer.drain()
            # очистка буфера
            self.future = asyncio.Future()
        else:
            self.state = message
            # возвращает обновлённое значение ожидающим клиентам
            self.future.set_result(message)

        writer.close()
        await writer.wait_closed()

server = Server('192.168.0.20', 7001)

obj = Activator(server)
obj.elevate()