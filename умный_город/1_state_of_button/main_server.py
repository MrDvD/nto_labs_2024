import asyncio, re

state = '?'
get = re.compile('^GET ')

async def handler(reader, writer):
    data = await reader.read(1024)
    data = data.decode()
    print(data)
    global state
    if get.match(data):
        print('yes')
        header = 'HTTP/1.1 200 OK\r\n\r\n'
        writer.write((header + state).encode())
        await writer.drain()
    else:
        if state != data:
            state = data
            print(state)
    writer.close()
    await writer.wait_closed()

async def main(address, port):
    server = await asyncio.start_server(handler, address, port)
    async with server:
        await server.serve_forever()

asyncio.run(main('192.168.155.242', 7000))