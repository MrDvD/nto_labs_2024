import asyncio, sqlalchemy, yaml, json, pandas

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
        result = int(binary, 2)
        return result
    
    def int_to_binary(self, number, bits):
        result = bin(number)[2:]
        return '0' * (bits - len(result)) + result

    def connection_made(self, transport):
        print('[SERVER_ACTIVATED]')
        self.transport = transport
        self.is_closed = asyncio.Future()
    
    async def handle_data(self, data, addr):
        data = data.decode()[:-2]
        if data == '0':
            with open('data.json') as f:
                angle = json.load(f)['angle']
            if not self.update.done():
                self.update.set_result(self.int_to_binary(angle, 9))
        elif data == '1':
            print(await self.update)
            self.transport.sendto(str(await self.update).encode(), addr)
            self.update = asyncio.Future()
        else:
            angle = self.binary_to_int(data)
            to_dump = {'angle': angle}
            with open('data.json', 'w') as f:
                json.dump(to_dump, f)
        # if data == '1':
        #     self.transport.sendto((await self.update).encode(), addr)
        #     self.update = asyncio.Future()
        # elif data == '0':
        #     # telegram bot update
        #     sql_engine = sqlalchemy.create_engine('sqlite:///users.db', echo=False)
        #     connection = sql_engine.raw_connection()
        #     df = pandas.read_sql('SELECT * FROM users', con=connection)
        #     # ПЕРЕДЕЛАТЬ ЛОГИКУ: БУДУЩЕЕ МОЖЕТ БЫТЬ УЖЕ НЕАКТУАЛЬНЫМ
        #     if not self.update.done():
        #         self.update.set_result(str(self.int_to_binary(len(df), 8)))
        # elif len(data) > 16:
        #     charge, state, users = self.binary_to_int(data[0:8]), self.binary_to_int(data[8:16]), self.binary_to_int(data[16:24])
        #     # print(charge, state, users)
        #     data = {'charge': charge, 'state': state, 'nusr': users}
        #     with open('data.json', 'w') as f:
        #         json.dump(data, f)

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