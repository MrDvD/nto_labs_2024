import telebot, json, pandas, sqlalchemy
from telebot import types

bot = telebot.TeleBot('7045541803:AAGJ3axeJEENXltI8TLBuLQ_qlHJKsXN0wY')

@bot.message_handler(commands=['start'])
def start(message):
    markup = types.InlineKeyboardMarkup(row_width = 2)

    but1 = types.InlineKeyboardButton('Battery state', callback_data='state')
    but2 = types.InlineKeyboardButton('Connect', callback_data='adusr')

    markup.add(but1, but2)
    bot.send_message(message.chat.id, "Choose", reply_markup=markup)



@bot.callback_query_handler(func=lambda call:True)
def answer(callback):
    markup2 = types.InlineKeyboardMarkup(row_width = 2)
    markup3 = types.InlineKeyboardMarkup(row_width = 2)

    but1 = types.InlineKeyboardButton('Battery state', callback_data='state')
    but2 = types.InlineKeyboardButton('Connect', callback_data='adusr')
    but3 = types.InlineKeyboardButton('Disconnect', callback_data='rmusr')

    markup2.add(but1, but2)
    markup3.add(but1, but3)

    usr_id = callback.message.chat.id

    if callback.message:
        if callback.data == 'state':
            with open('data.json') as f:
                jlist = json.load(f)
                charge = jlist['charge']
                if jlist['state'] == 0:
                    status = 'Discharged'
                elif jlist['state'] == 1:
                    status = 'Charging'
                elif jlist['state'] == 2:
                    status = 'Discharging'
                elif jlist['state'] == 3:
                    status = 'Charged'
            try:
                bot.edit_message_text(chat_id=callback.message.chat.id, text=f"Current charge: {charge}%\nCurrent state: {status}", message_id=callback.message.message_id, reply_markup= markup2)
            except Exception as e:
                bot.edit_message_text(chat_id=callback.message.chat.id, text=f"Current charge: {charge}% \nCurrent state: {status}", message_id=callback.message.message_id, reply_markup= markup2)
        
        elif callback.data == 'adusr':
            with open('data.json') as f:
                jlist = json.loads(f.read())
                sql_engine = sqlalchemy.create_engine('sqlite:///users.db', echo = False)
                connection = sql_engine.raw_connection()
                dict = {'ID':[]}
                df = pandas.DataFrame(dict)
                try:
                   df.to_sql('users', connection, index=False, if_exists='fail')
                except:
                    pass
                df = pandas.read_sql('SELECT * FROM users', con = connection)
                if not(callback.message.from_user.id in df.ID.tolist()):
                    
                    print(usr_id)
                    new_dict = {'ID':[usr_id]}
                    row = pandas.DataFrame(new_dict)
                    new = pandas.concat([df, row])
                    new.to_sql('users', connection, index=False, if_exists='replace')
                    num = len(new)
                    print(new, '---------con-----------')
                    try:
                        bot.edit_message_text( text=f'You\'ve been connected\n{num} people currently connected', chat_id=callback.message.chat.id, message_id=callback.message.message_id, reply_markup=markup2)
                    except Exception as e:
                        bot.edit_message_text( text=f'You\'ve been connected \n{num} people currently connected', chat_id=callback.message.chat.id, message_id=callback.message.message_id, reply_markup=markup2)

                else:
                    num = len(df)
                    try:
                        bot.edit_message_text( text=f'You\'re already connected\n{num} people currently connected', chat_id=callback.message.chat.id, message_id=callback.message.message_id, reply_markup=markup3)
                    except Exception as e:
                        bot.edit_message_text( text=f'You\'re already connected \n{num} people currently connected', chat_id=callback.message.chat.id, message_id=callback.message.message_id, reply_markup=markup3)


        elif callback.data == 'rmusr':
            sql_engine = sqlalchemy.create_engine('sqlite:///users.db', echo = False)
            connection = sql_engine.raw_connection()
            df = pandas.read_sql('SELECT * FROM users', con = connection)
            df = df.drop(df[df['ID'] == usr_id].index)
            df.to_sql('users', connection, index=False, if_exists='replace')
            num = len(df)
            print(df, '----------dis---------')
            try:
                bot.edit_message_text( text=f'You\'ve been disconnected\n{num} people currently connected', chat_id=callback.message.chat.id, message_id=callback.message.message_id, reply_markup=markup2)
            except Exception as e:
                bot.edit_message_text( text=f'You\'ve been disconnected \n{num} people currently connected', chat_id=callback.message.chat.id, message_id=callback.message.message_id, reply_markup=markup2)






bot.infinity_polling()
