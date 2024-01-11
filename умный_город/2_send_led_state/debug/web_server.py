import flask

app = flask.Flask('website')

@app.route('/')
def main():
    with open('page.html', 'r') as f:
        html = f.read()
        return html

@app.route('/script.js')
def script():
    with open('script.js', 'r') as f:
        js = f.read()
        return flask.Response(js, mimetype='text/javascript')

@app.route('/styles.css')
def style():
    with open('styles.css', 'r') as f:
        css = f.read()
        return flask.Response(css, mimetype='text/css')

# @app.route('/', methods=['POST'])
# def ajaj():
#     with open('styles.css', 'r') as f:
#         css = f.read()
#         return 1

app.run(host='192.168.119.242', port=7001)