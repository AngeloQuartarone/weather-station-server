// gcc data-retrieval.c -o data -lpaho-mqtt3c ./lib/circularQueue.o ./lib/linkedList.o ./lib/zambretti.o -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include <math.h>
#include "./lib/circularQueue.h"
#include "./lib/linkedList.h"
#include "./lib/zambretti.h"
#include "secrets.h"
#include <unistd.h> // sleep -> Unix
#ifdef _WIN32
#include <windows.h> // Sleep -> Windows
#endif

#define DATAPATH "../data/mqtt_data.csv"
#define FORECASTPATH "../data/forecast.log"
// generic path: /var/log/
// oppure leggere $home

int processMessage(MQTTClient_message *);
int messageArrived(void *, char *, int, MQTTClient_message *);
char *calculateWeatherForecast(float currentPressure, float oldPressure, float currentTemperature);

circularQueue *pressureQueue;

int main(int argc, char *argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    pressureQueue = initQueue(36);

    FILE *file = fopen(DATAPATH, "w+");
    if (file == NULL)
    {
        // Error opening the file
        return -1;
    }
    fprintf(file, "Timestamp,Temperature,Humidity,Pressure\n");
    fflush(file);
    fclose(file);

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        // printf("Errore di connessione al broker MQTT, codice di errore: %d\n", rc);
        MQTTClient_destroy(&client);
        return EXIT_FAILURE;
    }

    // printf("Connesso al broker MQTT con autenticazione!\n");

    if ((rc = MQTTClient_subscribe(client, TOPIC, QOS)) != MQTTCLIENT_SUCCESS)
    {
        // printf("Errore nella sottoscrizione al topic, codice di errore: %d\n", rc);
        MQTTClient_disconnect(client, TIMEOUT);
        MQTTClient_destroy(&client);
        return EXIT_FAILURE;
    }

    while (1)
    {
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

/**
 * MQTT message arrival callback function.
 *
 * This function is called whenever an MQTT message arrives. It processes the message
 * and logs the result.
 *
 * @param context Not used in this implementation.
 * @param topicName The name of the MQTT topic.
 * @param topicLen The length of the topic name.
 * @param message The received MQTT message.
 * @return 0 on success, -1 if processMessage fail.
 */
int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    if (strcmp(topicName, "data") != 0) {
        return -1;
    }
    // } else {
    //     printf("Topic Name is NULL\n");
    //     return -1;
    // }

    if (message == NULL) {
        return -1;
    }

    // Process the incoming message
    int ret = processMessage(message);
    if (ret == -1) {
        return -1;
    }

    MQTTClient_freeMessage(&message);
    if (topicName != NULL) {
        MQTTClient_free(topicName);
    }
    return 0;
}


/**
 * Function to process the content of an MQTT message.
 *
 * @param message The pointer to the MQTTClient_message received.
 *
 * This function extracts relevant weather data (timestamp, temperature, humidity, pressure)
 * from the message payload, and then calculates a weather forecast using the Zambretti algorithm.
 * It appends the result to a file for logging purposes.
 * @return 0 on success, -1 if fopen fail.
 */
int processMessage(MQTTClient_message *message)
{
    char *payload = (char *)message->payload;

    char timestamp[20];
    int temperature = 0;
    int humidity = 0;
    float pressure = 0;

    // Extract data from the MQTT payload
    char *timePtr = strstr(payload, "Time:");
    char *tempPtr = strstr(payload, "/T:");
    char *humPtr = strstr(payload, "/H:");
    char *pressPtr = strstr(payload, "/P:");

    if (timePtr)
    {   
        timePtr += 5;  // Salta la parte "Time:"
        strncpy(timestamp, timePtr, sizeof(timestamp) - 1);
        timestamp[sizeof(timestamp) - 1] = '\0'; // Assicurati che sia sempre terminata con null
    }

    if (tempPtr)
    {
        temperature = atoi(tempPtr + 3);
    }

    if (humPtr)
    {
        humidity = atoi(humPtr + 3);
    }

    if (pressPtr)
    {
        pressure = atof(pressPtr + 3);
        pressure = pressure / 100;
    }

    // Retrieve the previous pressure value from the queue (if any)
    float oldPressure = 0.0;
    if (!isEmpty(pressureQueue))
    {
        oldPressure = pressureQueue->arr[pressureQueue->front];
    }

    // Enqueue the new pressure reading
    enqueue(pressureQueue, pressure);

    // Calculate the weather forecast
    char *forecast = calculateWeatherForecast(pressure, oldPressure, (float)temperature);

    FILE *forecast_file = fopen(FORECASTPATH, "w+");
    if (forecast_file == NULL)
    {
        return -1;
    }
    
    fprintf(forecast_file, "%s\n", forecast);
    fflush(forecast_file);
    fclose(forecast_file);
    

    // Log the MQTT message and forecast to a file
    FILE *log_file = fopen(DATAPATH, "a");
    if (log_file == NULL)
    {
        return -1;
    }
    
    fprintf(log_file, "%s,%d,%d,%f\n", timestamp, temperature, humidity, pressure);
    fflush(log_file);
    fclose(log_file);
    


    return 0;
}

/**
 * Function to calculate a weather forecast based on pressure and temperature.
 *
 * @param currentPressure The current atmospheric pressure (hPa).
 * @param oldPressure The previous atmospheric pressure.
 * @param currentTemperature The current temperature (Celsius).
 * @return A string representing the calculated weather forecast.
 *
 * This function calculates the weather forecast using the Zambretti algorithm.
 * It first determines the pressure trend, calculates sea level pressure, and
 * then looks up the forecast based on Zambretti's result.
 * @return char* on success, NULL on prediction error; 
 */
char *calculateWeatherForecast(float currentPressure, float oldPressure, float currentTemperature)
{
    // Determine the pressure trend
    int trend = pressureTrend(currentPressure, oldPressure);

    // Calculate sea level pressure based on the current temperature
    float seaLevelPressure = pressureSeaLevel(currentTemperature, currentPressure);

    // Get the Zambretti result
    int zambrettiResult = caseCalculation(trend, seaLevelPressure);

    // Look up the weather forecast based on the Zambretti result
    char *forecast = lookUpTable(zambrettiResult);
    

    if (forecast == NULL)
    {
        return NULL;
    }
    else
    {
        return forecast;
    }
}
