import flask, socket

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

app.run(host='192.168.42.233', port=7000)