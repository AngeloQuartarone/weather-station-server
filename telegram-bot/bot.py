import os
import csv
from telegram import Update
from telegram.ext import ApplicationBuilder, CommandHandler, MessageHandler, filters, ContextTypes
from dotenv import load_dotenv

load_dotenv()  

# Ottieni il token del bot da una variabile d'ambiente
BOT_TOKEN = os.environ.get('BOT_TOKEN')

# Crea l'applicazione del bot
app = ApplicationBuilder().token(BOT_TOKEN).build()

# Funzione di avvio
async def start(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text("Welcome to my weather bot!")

# Funzione per il forecast
async def forecast(update: Update, context: ContextTypes.DEFAULT_TYPE):
    try:
        with open('../data/forecast.log', 'r') as file:
            forecast_data = file.read()
        await update.message.reply_text(forecast_data)
    except FileNotFoundError:
        await update.message.reply_text("Sorry, the forecast file is not found.")

# Funzione per leggere i dati dai sensori
async def sensor_value(update: Update, context: ContextTypes.DEFAULT_TYPE):
    try:
        with open('../data/mqtt_data.csv', 'r') as file:
            reader = csv.reader(file)
            next(reader)  # Salta l'intestazione
            last_row = None
            for last_row in reader:
                pass  # Scorri fino all'ultima riga

            if last_row:
                timestamp, temperature, humidity, pressure = last_row
                response = (f"Timestamp: {timestamp}\n"
                            f"Temperature: {temperature} °C\n"
                            f"Humidity: {humidity} %\n"
                            f"Pressure: {pressure} hPa")
                await update.message.reply_text(response)
            else:
                await update.message.reply_text("No sensor data available.")
    except FileNotFoundError:
        await update.message.reply_text("Sorry, the sensor data file is not found.")

# Funzione per gestire comandi sconosciuti
async def unknown_command(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text("Available Commands:\n/start - Welcome message\n/forecast - To get the actual forecast\n/sensor_value - To get Temperature, Humidity, and Pressure")

# Aggiungi i gestori dei comandi
app.add_handler(CommandHandler('start', start))
app.add_handler(CommandHandler('forecast', forecast))
app.add_handler(CommandHandler('sensor_value', sensor_value))  # Aggiungi il gestore per i dati del sensore
app.add_handler(MessageHandler(filters.COMMAND, unknown_command))  # Gestisce i comandi non riconosciuti
app.add_handler(MessageHandler(filters.TEXT & ~filters.COMMAND, unknown_command))  # Messaggi di testo che non sono comandi

# Avvia il polling
app.run_polling()
