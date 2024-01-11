from flask import Flask
from jinja2 import Template
import flask, requests, json

app = Flask('webserver')

@app.route('/')
def main():
    return 'Main Page!'

@app.route('/yay', methods=['GET'])
def secondary():
    return 'Yay Page :)'

@app.route('/get', methods=['GET'])
def getrequest():
    value = requests.get('http://192.168.0.20:7001').content
    return f'Responce: {value.decode()}'

@app.route('/post/<int:state>', methods=['GET'])
def dynamicroute(state):
    requests.post('http://192.168.0.20:7001/int', data={'state': state, 'hotel': 'trivago'})
    return f'State {state} has been successfully sent!'

@app.route('/render/<text>', methods=['GET'])
def jinjaformat(text):
    with open('page.html', 'r') as f:
        page = f.read()
        template = Template(page)
        return template.render(value=text)

@app.route('/color', methods=['GET'])
def sendjson():
    color = flask.request.args.to_dict()
    payload = json.dumps(color)
    requests.post('http://192.168.0.20:7001/json', data=payload)
    return f'Loaded JSON: {payload}'

app.run(host='192.168.0.20', port=7000)