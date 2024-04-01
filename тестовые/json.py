import json

data = {'temp': 36.7, 'time': '20:03'}
packed = json.dumps(data)
print(packed, type(packed))
unpacked = json.loads(packed)
print(unpacked, type(unpacked))
print(unpacked['time'], 'time')