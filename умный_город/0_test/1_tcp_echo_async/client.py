import asyncio

async def client(address, port, message):
    reader, writer = await asyncio.open_connection(address, port)
    data = message.encode()

    writer.write(data)
    await writer.drain()

    response = await reader.read(1024)
    echo = response.decode()
    print(f'Echo has been received: {echo}')

    writer.close()
    await writer.wait_closed()

message = input()
asyncio.run(client('192.168.0.20', 7001, message))