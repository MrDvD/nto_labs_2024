import asyncio, sqlalchemy
import pandas as pd

server_ip, server_port = '192.168.0.20', 7001

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
        if data == '1':
            self.transport.sendto(str(await self.update).encode(), addr)
            self.update = asyncio.Future()
        else:
            user_id, clicks = self.binary_to_int(data[0:8]), self.binary_to_int(data[8:])
            sql_engine = sqlalchemy.create_engine('sqlite:///test.db', echo = False)
            connection = sql_engine.raw_connection()
            df = pd.read_sql('SELECT * FROM test', con = connection)
            if not(user_id in df.ID.tolist()):
                row_dict = {'ID':[user_id], 'Clicks':[clicks]}
                row = pd.DataFrame(row_dict)
                new = pd.concat([df, row]) 
                new.to_sql('test', connection, index=False, if_exists='replace')
            else:
                df.loc[df['ID'] == user_id, 'Clicks'] += clicks
                df.to_sql('test', connection, index=False, if_exists='replace')
            # df = pd.read_sql('SELECT * FROM test', con = connection)
            # print(df)
            self.update.set_result(1)

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
        try:
            # creates db if it didn't exist
            dict = {'ID':[], 'Clicks':[]}
            df = pd.DataFrame(dict)
            sql_engine = sqlalchemy.create_engine('sqlite:///test.db', echo = False)
            connection = sql_engine.raw_connection()
            df.to_sql('test', connection, index=False, if_exists='fail')
        except:
            pass
    
    async def start(self):
        loop = asyncio.get_running_loop()
        self.update = asyncio.Future()
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: Datagram(self.update),
            local_addr=(self.ip, self.port)
        )
        await protocol.serve_forever()

server = Server(server_ip, server_port)

obj = Activator(server)
obj.elevate()