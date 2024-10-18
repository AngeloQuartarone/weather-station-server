import paho.mqtt.client as mqtt

# Funzione di callback per quando il client si connette al broker
def on_connect(client, userdata, flags, rc):
    print("Connesso con codice di stato: " + str(rc))
    client.subscribe("data")  # Sottoscrivi al topic "data"

# Funzione di callback per quando un messaggio viene ricevuto
def on_message(client, userdata, msg):
    data = msg.payload.decode()  # Decodifica il messaggio
    print(f"Messaggio ricevuto: {data}")  # Stampa il messaggio sulla console
    
    # Salva il messaggio su un file
    with open("dati_mqtt.txt", "a") as file:
        file.write(data + "\n")  # Aggiungi il messaggio al file

# Creazione del client MQTT
client = mqtt.Client()

# Imposta le funzioni di callback
client.on_connect = on_connect
client.on_message = on_message

# Connessione al broker
broker_address = "192.168.1.29"  # Sostituisci con l'indirizzo del tuo broker
client.username_pw_set("mqttuser", "mqttpassword")  # Aggiungi username e password se necessari
client.connect(broker_address, 1883, 60)  # Connessione al broker

# Loop principale
client.loop_start()  # Inizia il loop del client

try:
    while True:
        pass  # Mantiene il programma in esecuzione
except KeyboardInterrupt:
    print("Interrotto dal programma...")
    client.loop_stop()  # Ferma il loop del client
    client.disconnect()  # Disconnetti dal broker
