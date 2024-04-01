import asyncio, yaml

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

class Server:
    def __init__(self, ip, port):
        self.ip, self.port = ip, port
    
    async def start(self):
        server = await asyncio.start_server(self.on_connect, self.ip, self.port)
        self.future = asyncio.Future()
        async with server:
            print(f'Listening on [{self.port}]')
            await server.serve_forever()
    
    async def get(self, reader, size=5120):
        data = await reader.read(size)
        return data.decode()
    
    async def send(self, message, writer):
        writer.write(message.encode())
        await writer.drain()
    
    async def close(self, obj):
        obj.close()
        await obj.wait_closed()

    async def on_connect(self, reader, writer):
        message = self.get(reader)
        print(f'[{self.port}] Data has been received: {message}')

        if self.init.match(message):
            # возвращает текущее состояние новому клиенту
            self.send(self.header + self.state, writer)
        elif self.update.match(message):
            # ожидание обновлённого значения
            update = await self.future
            self.send(self.header + update, writer)
            # очистка буфера
            self.future = asyncio.Future()
        else:
            self.state = message
            # возвращает обновлённое значение ожидающим клиентам
            self.future.set_result(message)
        self.close(writer)

server = Server(cfg['server_ip'], cfg['server_port'])

obj = Activator(server)
obj.elevate()