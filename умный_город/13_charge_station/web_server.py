import flask, socket, yaml

with open('config.yaml') as f:
    cfg = yaml.load(f, Loader=yaml.FullLoader)

app = flask.Flask('Web', template_folder='')

@app.route('/')
def main():
    return flask.send_file('website/main.html')
    
@app.route('/charge')
def charge():
    return flask.send_file('website/charge.html')

@app.route('/<js>.js')
def scripts(js):
    conx = {'ip': cfg['server_ip'], 'web_port': cfg['web_port']}
    return flask.render_template(f'website/{js}.js', context=conx)

@app.route('/<css>.css')
def styles(css):
    return flask.send_file(f'website/{css}.css')

@app.route('/media/<img>')
def image(img):
    return flask.send_file(f'website/media/{img}')

@app.route('/fonts/<font>')
def fonts(font):
    return flask.send_file(f'website/fonts/{font}')

@app.route('/init')
def init():
    return flask.send_file('data.json')

@app.route('/update')
def update():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto('1'.encode(), (cfg['server_ip'], cfg['server_port']))
    if sock.recv(100).decode() == '1':
        return flask.send_file('data.json')

app.run(host=cfg['server_ip'], port=cfg['web_port'])
