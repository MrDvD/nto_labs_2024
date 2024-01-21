import flask

app = flask.Flask('Example')

@app.route('/', methods=['GET'])
def main():
    with open('website/distance_control.html', 'r') as f:
        html = f.read()
        return html

@app.route('/script.js', methods=['GET'])
def script():
    with open('website/script.js', 'r') as f:
        js = f.read()
        return js

@app.route('/style.css', methods=['GET'])
def styles():
    with open('website/style.css', 'r') as f:
        css = f.read()
        return flask.Response(css, mimetype='text/css')

app.run(host='192.168.0.20', port=7000)