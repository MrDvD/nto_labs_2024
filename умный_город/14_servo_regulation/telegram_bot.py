import telebot, json, pandas, sqlalchemy, yaml, socket
from telebot import types

with open('config.yaml') as f:
    cfg = yaml.load(f, Loader=yaml.FullLoader)

bot = telebot.TeleBot(cfg['telegram_api'])
battery_state = {0: 'Discharged', 1: 'Charging', 2: 'Discharging', 3: 'Charged'}

@bot.message_handler(commands=['start'])
def start(message):
    markup = types.InlineKeyboardMarkup(row_width=2)

    button1 = types.InlineKeyboardButton('Servo angle', callback_data='state')
    button2 = types.InlineKeyboardButton('Enter angle', callback_data='angle')

    markup.add(button1, button2)
    bot.send_message(message.chat.id, "Choose", reply_markup=markup)

@bot.callback_query_handler(func=lambda call:True)
def answer(callback):
    markup_con = types.InlineKeyboardMarkup(row_width=2)

    button1 = types.InlineKeyboardButton('Servo angle', callback_data='state')
    button2 = types.InlineKeyboardButton('Enter angle', callback_data='angle')

    markup_con.add(button1, button2)

    if callback.message:
        if callback.data == 'state':
            with open('data.json') as f:
                jlist = json.load(f)
            angle = jlist['angle']
            new_text = f'Current angle: {angle}'
            markup = markup_con
            if callback.message.text != new_text:
                bot.edit_message_text(text=new_text,
                                    chat_id=callback.message.chat.id,
                                    message_id=callback.message.message_id,
                                    reply_markup=markup)
        elif callback.data == 'angle':
            msg = bot.edit_message_text(text='Angle:',
                                  chat_id=callback.message.chat.id,
                                  message_id=callback.message.message_id)
            bot.register_next_step_handler(msg, enter_angle)
            
def enter_angle(message):
    markup = types.InlineKeyboardMarkup(row_width=2)

    button1 = types.InlineKeyboardButton('Servo angle', callback_data='state')
    button2 = types.InlineKeyboardButton('Enter angle', callback_data='angle')

    markup.add(button1, button2)

    angle = message.text
    if angle.isdigit() and int(angle) <=180 and int(angle)>=0:
        with open('data.json') as f:
            jlist = json.load(f)
            jlist['angle'] = int(angle)
        with open('data.json', 'w') as f:
            json.dump(jlist, f)
        new_text = 'Angle was changed'
        bot.send_message(text=new_text,
                            chat_id=message.chat.id,
                            reply_markup=markup)
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.sendto('0\r\n'.encode(), (cfg['server_ip'], cfg['server_port']))
    else:
        msg = bot.send_message(text = 'Wrong format', chat_id=message.chat.id)
        bot.register_next_step_handler(msg, enter_angle)



bot.infinity_polling()