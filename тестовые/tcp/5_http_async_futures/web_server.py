import flask

app = flask.Flask('webserver')

@app.route('/', methods=['GET'])
def root():
    with open('page.html', 'r') as f:
        html = f.read()
        return html

@app.route('/script.js', methods=['GET'])
def script():
    with open('script.js', 'r') as f:
        js = f.read()
        return flask.Response(js, mimetype='text/javascript')

app.run(host='192.168.0.20', port=7000)