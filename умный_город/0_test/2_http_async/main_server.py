import asyncio, re, json

post_int = re.compile(r'^POST \/int(?:.|\n)*state=(\d+)')
post_json = re.compile(r'^POST \/json(?:.|\n)*\r\n\r\n(.*)')
get = re.compile(r'^GET ')

async def handler(reader, writer):
    data = await reader.read(1024)
    data = data.decode()
    print(data)

    header = 'HTTP/1.1 200 OK\r\n\r\n'
    if get.match(data):
        message = 'woWie, it w0rks!'
        writer.write((header + message).encode())
        await writer.drain()
    elif post_int.match(data):
        result = post_int.search(data)
        value = result.group(1)
        print(f'Resulting value is: {value}')
        writer.write(header.encode())
        await writer.drain()
    elif post_json.match(data):
        result = post_json.search(data)
        value = result.group(1)
        color = json.loads(value)
        print(f"R has {color['R']}, G has {color['G']}, and B has {color['B']}")
        writer.write(header.encode())
        await writer.drain()
    else:
        writer.write(header.encode())
        await writer.drain()
    writer.close()
    await writer.wait_closed()

async def main(address, port):
    server = await asyncio.start_server(handler, address, port)
    async with server:
        await server.serve_forever()

asyncio.run(main('192.168.0.20', 7001))