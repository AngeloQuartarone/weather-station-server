CREATE DATABASE data;
USE data;

CREATE TABLE weather_data (
    timestamp TIMESTAMP PRIMARY KEY,
    pressure FLOAT,
    temperature FLOAT,
    humidity FLOAT,
    forecast VARCHAR(50)
);
