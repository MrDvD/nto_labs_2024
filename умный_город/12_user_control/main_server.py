import asyncio
import pandas as pd
import sqlalchemy

class Activator:
    def __init__(self, *servers):
        self.servers = list(servers)
    
    async def gather(self):
        start = list(map(lambda x: x.start(), self.servers))
        return await asyncio.gather(*start)

    def elevate(self):
        asyncio.run(self.gather())

class Datagram(asyncio.DatagramProtocol):
    def __init__(self, distance):
        super().__init__()
        self.distance = distance
        self.cache = 5
    
    def binary_to_float(self, binary):
        sign = -1 if int(binary[0]) else 1
        pow_sign = -1 if int(binary[1]) else 1
        power = int(binary[2:8], 2)
        mantissa = int(binary[8:], 2)
        return int(sign * mantissa * 10 ** (pow_sign * power))

    def connection_made(self, transport):
        print('[SERVER_ACTIVATED]')
        self.transport = transport
        self.is_closed = asyncio.Future()
    
    def datagram_received(self, data, addr):
        data = data.decode()
        user_id, clicks = self.binary_to_float(data[0:32]), self.binary_to_float(data[32:])
        sql_engine = sqlalchemy.create_engine('sqlite:///test.db', echo = False)
        connection = sql_engine.raw_connection()
        df = pd.read_sql('SELECT * FROM test', con = connection)
        print(df)
        if not(user_id in df.ID.tolist()):
            dict = {'ID':[user_id], 'Clicks':[clicks]}
            df2 = pd.DataFrame(dict)
            df3 = pd.concat([df, df2]) 
            df3.to_sql('test', connection, index=False, if_exists='replace')
        else:
            df.loc[df['ID'] == user_id, 'Clicks'] += clicks
            df.to_sql('test', connection, index=False, if_exists='replace')
        df = pd.read_sql('SELECT * FROM test', con = connection)
        print(df)
    
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
            dict = {'ID':[], 'Clicks':[]}
            df = pd.DataFrame(dict)
            sql_engine = sqlalchemy.create_engine('sqlite:///test.db', echo = False)
            connection = sql_engine.raw_connection()
            df.to_sql('test', connection, index=False, if_exists='fail')
        except:
            pass
    
    async def start(self):
        loop = asyncio.get_running_loop()
        self.distance = asyncio.Future()
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: Datagram(self.distance),
            local_addr=(self.ip, self.port)
        )
        await protocol.serve_forever()

server = Server('192.168.45.242', 7001)

obj = Activator(server)
obj.elevate()