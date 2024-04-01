import asyncio

async def esp32(address, port, message):
    reader, writer = await asyncio.open_connection(address, port)
    data = message.encode()

    writer.write(data)
    await writer.drain()
    writer.close()
    await writer.wait_closed()

while True:
    message = input()
    asyncio.run(esp32('192.168.0.20', 7001, message))