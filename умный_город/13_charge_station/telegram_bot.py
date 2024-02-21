import telebot, json, pandas, sqlalchemy, yaml
from telebot import types

with open('config.yaml') as f:
    cfg = yaml.load(f, Loader=yaml.FullLoader)

bot = telebot.TeleBot(cfg['telegram_api'])
battery_state = {0: 'Discharged', 1: 'Charging', 2: 'Discharging', 3: 'Charged'}

try:
    # creates db if it didn't exist
    sql_engine = sqlalchemy.create_engine('sqlite:///test.db', echo = False)
    connection = sql_engine.raw_connection()
    df = pandas.DataFrame({'ID': []})
    df.to_sql('test', connection, index=False, if_exists='fail')
except:
    pass

@bot.message_handler(commands=['start'])
def start(message):
    markup = types.InlineKeyboardMarkup(row_width=2)

    button1 = types.InlineKeyboardButton('Battery state', callback_data='state')
    button2 = types.InlineKeyboardButton('Connect', callback_data='con')

    markup.add(button1, button2)
    bot.send_message(message.chat.id, "Choose", reply_markup=markup)

@bot.callback_query_handler(func=lambda call:True)
def answer(callback):
    markup_con = types.InlineKeyboardMarkup(row_width=2)
    markup_dis = types.InlineKeyboardMarkup(row_width=2)

    button1 = types.InlineKeyboardButton('Battery state', callback_data='state')
    button2 = types.InlineKeyboardButton('Connect', callback_data='con')
    button3 = types.InlineKeyboardButton('Disconnect', callback_data='dis')

    markup_con.add(button1, button2)
    markup_dis.add(button1, button3)

    user_id = callback.message.chat.id

    if callback.message:
        if callback.data == 'state':
            with open('data.json') as f:
                jlist = json.load(f)
            charge, status = jlist['charge'], battery_state[jlist['state']]
            new_text = f'Current charge: {charge}%\nCurrent state: {status}'
            markup = markup_con
        else:
            sql_engine = sqlalchemy.create_engine('sqlite:///users.db', echo=False)
            connection = sql_engine.raw_connection()
            df = pandas.read_sql('SELECT * FROM users', con=connection)
            if callback.data == 'con':
                if callback.message.from_user.id not in df.ID.tolist():
                    # print(user_id)
                    row = pandas.DataFrame({'ID': [user_id]})
                    df = pandas.concat([df, row])
                    df.to_sql('users', connection, index=False, if_exists='replace')
                    new_text = 'You\'ve been connected\n{} people currently connected'
                    markup = markup_con
                else:
                    new_text = 'You\'re already connected\n{} people currently connected'
                    markup = markup_dis
            else:
                df = df.drop(df[df['ID'] == user_id].index)
                df.to_sql('users', connection, index=False, if_exists='replace')
                # print(df, '----------dis---------')
                new_text = 'You\'ve been disconnected\n{} people currently connected'
                markup = markup_con
            count = len(df)
            new_text = new_text.format(count)
        if callback.message.text != new_text:
            bot.edit_message_text(text=new_text,
                                  chat_id=callback.message.chat.id,
                                  message_id=callback.message.message_id,
                                  reply_markup=markup)

bot.infinity_polling()