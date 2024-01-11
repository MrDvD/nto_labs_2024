import asyncio

async def handler(reader, writer):
    data = await reader.read(1024)
    message = data.decode()
    print(f'Data has been received: {message}')

    writer.write(data)
    await writer.drain()

    writer.close()
    await writer.wait_closed()

async def main(address, port):
    server = await asyncio.start_server(handler, address, port)
    async with server:
        await server.serve_forever()

asyncio.run(main('192.168.0.20', 7001))