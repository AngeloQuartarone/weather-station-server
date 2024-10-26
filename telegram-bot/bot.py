import os
import mysql.connector
from telegram import Update
from telegram.ext import ApplicationBuilder, CommandHandler, MessageHandler, filters, ContextTypes
from dotenv import load_dotenv

load_dotenv()

# Ottieni il token del bot e i dettagli del database dalle variabili d'ambiente
BOT_TOKEN = os.environ.get('BOT_TOKEN')
DB_HOST = os.environ.get('DB_HOST', 'localhost')
DB_USER = os.environ.get('DB_USER')
DB_PASSWORD = os.environ.get('DB_PASSWORD')
DB_NAME = os.environ.get('DB_NAME')

# Crea l'applicazione del bot
app = ApplicationBuilder().token(BOT_TOKEN).build()

# Funzione per connettersi al database
def get_db_connection():
    return mysql.connector.connect(
        host=DB_HOST,
        user=DB_USER,
        password=DB_PASSWORD,
        database=DB_NAME
    )

# Funzione di avvio
async def start(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text("Welcome to my weather bot!")

# Funzione per il forecast
async def forecast(update: Update, context: ContextTypes.DEFAULT_TYPE):
    try:
        db = get_db_connection()
        cursor = db.cursor()
        cursor.execute("SELECT forecast FROM weather_data ORDER BY timestamp DESC LIMIT 1")
        result = cursor.fetchone()
        cursor.close()
        db.close()
        
        if result:
            await update.message.reply_text(result[0])
        else:
            await update.message.reply_text("No forecast data available.")
    except mysql.connector.Error as err:
        await update.message.reply_text(f"Database error: {err}")

# Funzione per leggere i dati dai sensori
async def sensor_value(update: Update, context: ContextTypes.DEFAULT_TYPE):
    try:
        db = get_db_connection()
        cursor = db.cursor()
        
        # Usa timestamp per ottenere l'ultima riga
        cursor.execute("SELECT timestamp, temperature, humidity, pressure FROM weather_data ORDER BY timestamp DESC LIMIT 1")
        result = cursor.fetchone()
        cursor.close()
        db.close()
        
        if result:
            timestamp, temperature, humidity, pressure = result
            response = (f"Timestamp: {timestamp}\n"
                        f"Temperature: {temperature} °C\n"
                        f"Humidity: {humidity} %\n"
                        f"Pressure: {pressure} hPa")
            await update.message.reply_text(response)
        else:
            await update.message.reply_text("No sensor data available.")
    except mysql.connector.Error as err:
        await update.message.reply_text(f"Database error: {err}")

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
