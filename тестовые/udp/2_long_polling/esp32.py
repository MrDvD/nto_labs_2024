import asyncio

class Datagram(asyncio.DatagramProtocol):
    def __init__(self, message):
        self.message = message
        self.is_closed = asyncio.Future()

    def connection_made(self, transport):
        # pre-made handler-functions
        self.transport = transport
        self.transport.sendto(self.message.encode())
        self.is_closed.set_result(True)

    def error_received(self, exc):
        print('[ERROR]:', exc)
    
    def connection_lost(self, exc):
        print("[CONNECTION_CLOSED]", exc)
    
    async def wait_until_closed(self):
        # this method is self-made
        if await self.is_closed:
            self.transport.close()

async def esp32(addr, port_from, port_to, message):
    loop = asyncio.get_running_loop()
    transport, protocol = await loop.create_datagram_endpoint(
        lambda: Datagram(message),
        local_addr=(addr, port_from),
        remote_addr=("192.168.141.242", port_to)
    )
    await protocol.wait_until_closed() # 'protocol' grants access to class methods

message = '1 2 3'
asyncio.run(esp32('192.168.141.242', 6999, 7001, message))