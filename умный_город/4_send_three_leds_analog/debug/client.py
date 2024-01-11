import asyncio, time

idx = 0

async def web(address, port):
    print('webwebweb')
    global idx
    reader, writer = await asyncio.open_connection(address, port)
    data = f'POST\r\n\r\n{{"1": 0, "2": {idx}, "3": 255}}'

    writer.write(data.encode())
    await writer.drain()

    idx = (idx + 1) % 256

    response = await reader.read(1024)

    writer.close()
    await writer.wait_closed()

async def esp(address, port):
    print('espespesp')
    global idx
    reader, writer = await asyncio.open_connection(address, port)

    writer.write('1'.encode())
    await writer.drain()
    print('sent')

    response = await reader.read(1024)
    echo = response.decode()
    print(echo)

    writer.close()
    await writer.wait_closed()

async def main():
    while True:
        await asyncio.gather(web('192.168.0.20', 7001), esp('192.168.0.20', 7001))
        time.sleep(1)

# while True:
#     asyncio.run(web('192.168.0.20', 7001))
#     asyncio.run(esp('192.168.0.20', 7001))
#     time.sleep(1)
asyncio.run(main())