import flask, socket, sqlalchemy, json, pandas

app = flask.Flask('Web', template_folder='')
server_ip, web_port, server_port = '192.168.0.20', 7000, 7001

def db_to_json():
    sql_engine = sqlalchemy.create_engine('sqlite:///test.db', echo = False)
    connection = sql_engine.raw_connection()
    df = pandas.read_sql('SELECT * FROM test', con = connection)
    data = list()
    for i , r in df.iterrows():
        di = {'ID': int(r['ID']), 'Clicks': int(r['Clicks'])}
        data.append(di)
    return json.dumps(data)

@app.route('/', methods=['GET'])
def main():
    return flask.send_file('website/user_control.html')

@app.route('/script.js', methods=['GET'])
def script():
    conx = {'ip': server_ip, 'web_port': web_port}
    return flask.render_template('website/script.js', context=conx)

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
    return db_to_json()

@app.route('/update', methods=['GET'])
def update():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto('1'.encode(), (server_ip, server_port))
    if sock.recv(100).decode() == '1':
        return db_to_json()

app.run(host=server_ip, port=web_port)
