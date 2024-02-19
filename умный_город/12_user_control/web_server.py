import flask, socket, sqlalchemy, json, pandas

app = flask.Flask('Web')

@app.route('/', methods=['GET'])
def main():
    return flask.send_file('website/distance_control.html')

@app.route('/script.js', methods=['GET'])
def script():
    return flask.send_file('website/script.js')

@app.route('/style.css', methods=['GET'])
def styles():
    return flask.send_file('website/style.css')

@app.route('/media/<img>', methods=['GET'])
def image(img):
    return flask.send_file(f'website/media/{img}')

@app.route('/fonts/<font>', methods=['GET'])
def fonts(font):
    return flask.send_file(f'website/fonts/{font}')

@app.route('/init', methods=['GET'])
def init():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto('0'.encode(), ('192.168.45.242', 7001))
    return sock.recv(100)

@app.route('/update', methods=['GET'])
def update():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto('1'.encode(), ('192.168.45.242', 7001))
    return sock.recv(100)

@app.route('/get-data',methods = ['GET'])
def getdata():
    sql_engine = sqlalchemy.create_engine('sqlite:///test.db', echo = False)
    connection = sql_engine.raw_connection()
    df = pandas.read_sql('SELECT * FROM test', con = connection)
    data = []
    for i , r in df.iterrows():
        di = {'ID':int(r['ID']), 'Clicks':int(r['Clicks'])}
        data.append(di)
    js = json.dumps(data)
    print(js)
    return js    
        


app.run(host='192.168.45.242', port=7000)