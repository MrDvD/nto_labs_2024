import flask, yaml, json, logging, logging.config

with open('config.yaml') as f:
    cfg = yaml.load(f, Loader=yaml.FullLoader)

with open('log_config.yaml') as f:
    log_cfg = yaml.load(f, Loader=yaml.FullLoader)
    
logging.config.dictConfig(log_cfg)
logger = logging.getLogger('web')

app = flask.Flask('Web', template_folder='')
app.secret_key = b'n98sd9c8#$$^bwq78d'

@app.route('/')
def main():
    print(flask.request.remote_addr)
    logger.debug('Incoming / query', extra={'addr': flask.request.remote_addr})
    print('sssss')
    if 'user' in flask.session:
        logger.info('Redirecting to /account', extra={'addr': flask.request.remote_addr})
        return flask.redirect('account')
    logger.info('Requiring account info', extra={'addr': flask.request.remote_addr})
    return flask.send_file('website/main.html')

@app.route('/<js>.js')
def scripts(js):
    conx = {'ip': cfg['server_ip'], 'web_port': cfg['web_port']}
    logger.debug(f'Sending {js}.js script', extra={'addr': flask.request.remote_addr})
    return flask.render_template(f'website/{js}.js', context=conx)

@app.route('/<css>.css')
def styles(css):
    logger.debug(f'Sending {css}.css style', extra={'addr': flask.request.remote_addr})
    return flask.send_file(f'website/{css}.css')

@app.route('/media/<img>')
def image(img):
    logger.debug(f'Sending {img} media', extra={'addr': flask.request.remote_addr})
    return flask.send_file(f'website/media/{img}')

@app.route('/fonts/<font>')
def fonts(font):
    logger.debug(f'Sending {font} font', extra={'addr': flask.request.remote_addr})
    return flask.send_file(f'website/fonts/{font}')

@app.route('/login', methods=['POST'])
def login():
    logger.debug(f'Incoming /login query', extra={'addr': flask.request.remote_addr})
    with open('database.json') as f:
        database = json.load(f)
    user = flask.request.get_json()['user']
    passwd = flask.request.get_json()['pass']
    for obj in database:
        if obj['user'] == user and obj['pass'] == passwd:
            logger.info('Got a matching login-password pair in database', extra={'addr': flask.request.remote_addr})
            flask.session['user'] = user
            return '', 302
    logger.info('No matching login-password pair in database', extra={'addr': flask.request.remote_addr})
    return 'Wrong login/password', 401

@app.route('/logout')
def logout():
    logger.debug('Incoming /logout query', extra={'addr': flask.request.remote_addr})
    flask.session.pop('user', None)
    return flask.redirect(flask.url_for('main'))

@app.route('/account')
def account():
    logger.debug('Incoming /account query', extra={'addr': flask.request.remote_addr})
    if 'user' in flask.session:
        logger.info('Granting access to /account')
        conx = {'user': flask.session['user']}
        return flask.render_template('website/account.html', context=conx)
    logger.info('Blocking access to /account', extra={'addr': flask.request.remote_addr})
    return flask.redirect(flask.url_for('main'))

app.run(host=cfg['server_ip'], port=cfg['web_port'])