//gcc data-retrieval.c -o data -lpaho-mqtt3c ./lib/circularQueue.o ./lib/linkedList.o ./lib/zambretti.o -lm




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include <math.h>
#include "./lib/circularQueue.h"
#include "./lib/linkedList.h"
#include "./lib/zambretti.h"
#include "secrets.h"
#include <unistd.h>  // sleep -> Unix
#ifdef _WIN32
#include <windows.h>  // Sleep -> Windows
#endif


void processMessage(MQTTClient_message *);
int messageArrived(void *, char *, int, MQTTClient_message *);
char *calculateWeatherForecast(float currentPressure, float oldPressure, float currentTemperature);

circularQueue *pressureQueue;




int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    pressureQueue = initQueue(36);

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        //printf("Errore di connessione al broker MQTT, codice di errore: %d\n", rc);
        MQTTClient_destroy(&client);
        return EXIT_FAILURE;
    }

    //printf("Connesso al broker MQTT con autenticazione!\n");

    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS) {
        //printf("Errore nella sottoscrizione al topic, codice di errore: %d\n", rc);
        MQTTClient_disconnect(client, TIMEOUT);
        MQTTClient_destroy(&client);
        return EXIT_FAILURE;
    }

    while (1) {
        #ifdef _WIN32
        Sleep(1000);
        #else
        sleep(1);
        #endif
    }

    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);

    deleteQueue(pressureQueue);

    return rc;
}

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    

    //printf("   message: %.*s\n", message->payloadlen, (char *)message->payload);  // Correzione qui
    processMessage(message);



    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}


void processMessage(MQTTClient_message *message) {
    char *payload = (char *)message->payload;

    char timestamp[20];
    int temperature = 0;
    int humidity = 0;
    float pressure = 0;

    char *timePtr = strstr(payload, "Time:");
    char *tempPtr = strstr(payload, "/T:");
    char *humPtr = strstr(payload, "/H:");
    char *pressPtr = strstr(payload, "/P:");

    if (timePtr) {
        timePtr += 5; 
        strncpy(timestamp, timePtr, 19);
        timestamp[19] = '\0';
    }

    if (tempPtr) {
        temperature = atoi(tempPtr + 3);
    }

    if (humPtr) {
        humidity = atoi(humPtr + 3);
    }

    if (pressPtr) {
        pressure = atof(pressPtr + 3);
        pressure = pressure / 100;
    }

    //printf("Timestamp: %s\n", timestamp);
    //printf("Temperature: %d\n", temperature);
    //printf("Humidity: %d\n", humidity);
    //printf("Pressure: %.2f\n", pressure);

    float oldPressure = 0.0;
    if (!isEmpty(pressureQueue)) {
        oldPressure = pressureQueue->arr[pressureQueue->front];
    }

    enqueue(pressureQueue, pressure);

    char *forecast = calculateWeatherForecast(pressure, oldPressure, (float)temperature);



    FILE *file = fopen("mqtt_messages.txt", "a");

    if (file == NULL) {
        //printf("Errore nell'aprire il file!\n");
        return;
    }
    
    fprintf(file, "Message: %.*s -- Forecast: %s\n", message->payloadlen, (char*) message->payload, forecast);

    fclose(file);
}


char *calculateWeatherForecast(float currentPressure, float oldPressure, float currentTemperature) {
    int trend = pressureTrend(currentPressure, oldPressure);

    float seaLevelPressure = pressureSeaLevel(currentTemperature, currentPressure);

    int zambrettiResult = caseCalculation(trend, seaLevelPressure);

    char *forecast = lookUpTable(zambrettiResult);

    return forecast;

    //printf("Weather forecast: %s\n", forecast);
}

