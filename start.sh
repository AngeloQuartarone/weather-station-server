#!/bin/bash

# Ottieni la directory in cui si trova questo script
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Percorsi dei file
TELEGRAM_BOT_PATH="$SCRIPT_DIR/telegram-bot/bot.py"
VENV_DIR="$SCRIPT_DIR/telegram-bot/v-env"
MAKE_DIR="$SCRIPT_DIR/mqtt-data-ret"
SERVER_BINARY="$SCRIPT_DIR/mqtt-data-ret/data-retrieval"

# Controlla se l'ambiente virtuale esiste e se ci sono i file binari
if [ ! -d "$VENV_DIR" ] || [ ! -f "$VENV_DIR/bin/activate" ]; then
    echo "Creating virtual environment..."
    python3 -m venv "$VENV_DIR"  # Crea l'ambiente virtuale
fi

# Attiva l'ambiente virtuale
source "$VENV_DIR/bin/activate"

# Installa le dipendenze se necessario
if [ -f "$SCRIPT_DIR/telegram-bot/requirements.txt" ]; then
    pip install -r "$SCRIPT_DIR/telegram-bot/requirements.txt"
else
    echo "File requirements.txt non trovato!" >&2
    exit 1
fi

if [ ! -d "$SCRIPT_DIR/data" ]; then
    mkdir -p "$SCRIPT_DIR/data"
fi

# Compila il server in C usando make
if make -C "$MAKE_DIR"; then
    echo "Server in C compilato con successo."
else
    echo "Errore nella compilazione del server in C." >&2
    exit 1
fi

# Verifica se il file binario del server esiste
if [ ! -f "$SERVER_BINARY" ]; then
    echo "Errore: il file binario del server non esiste!" >&2
    exit 1
fi

# Avvia il server in C in background usando nohup
nohup "$SERVER_BINARY" &> server_output.log 2>&1 &
SERVER_PID=$!

# Controlla se il server è in esecuzione subito dopo l'avvio
sleep 1  # Aspetta un attimo per dare tempo al server di avviarsi

if ! ps -p "$SERVER_PID" > /dev/null; then
    echo "Errore: il server in C non è in esecuzione subito dopo l'avvio." >&2
    cat server_output.log  # Mostra il log per ulteriori dettagli
    exit 1
fi

echo "Server in C avviato in background con PID $SERVER_PID."
