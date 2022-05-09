/*************************************************************

Created using the help of two examples. ESP8266_Standalone which
is a very basic sketch used to start off the project in sprints 1
and 2. 

For sprint 3 the example OpenWeatherMapForecastDemo was used to
gather weather data from an online API. I then used this data
to suit my needs.

Each example uses its own function to connect to wifi. I left both
functions in when merging the sketches to avoid errors. It also
does not affect the main aim of the project.
 *************************************************************/

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Arduino.h>
#include <JsonListener.h>
#include <time.h>
#include "OpenWeatherMapForecast.h"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "eJuQ3KQr_8InX2tFdvdSZQ1qP8PBxZg0";

// Your WiFi credentials. FOR BLYNK
// Set password to "" for open networks.
char ssid[] = "Super fast crazy speed wifi";
char pass[] = "4Lutyens";






// initiate the client
OpenWeatherMapForecast client;

// See https://docs.thingpulse.com/how-tos/openweathermap-key/
String OPEN_WEATHER_MAP_APP_ID = "9205b41b5b2b47eb6ccb2f19dfd088bc";

String OPEN_WEATHER_MAP_LOCATION_ID = "2654675";

String OPEN_WEATHER_MAP_LANGUAGE = "en";
boolean IS_METRIC = false;
uint8_t MAX_FORECASTS = 15;

/**
 * WiFi Settings FOR FORECAST
 */
const char* ESP_HOST_NAME = "esp-" + ESP.getFlashChipId();
const char* WIFI_SSID     = "Super fast crazy speed wifi";
const char* WIFI_PASSWORD = "4Lutyens";

// initiate the WifiClient
WiFiClient wifiClient;



/**
 * Helping funtions
 */
void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());
  Serial.println();
}

//Set pin variables to control pumps
bool lockPump = false;
int pin0Value;
int pin3Value;

int IN2 = 2;
int IN14 = 14;
int IN12 = 12;

BLYNK_WRITE(V0){
  //Store the value of pin V0 as an int
  pin0Value = param.asInt();
  }

BLYNK_WRITE(V1){
  
  
  }

BLYNK_WRITE(V3){
  //Store the value of pin V3 as an int
  pin3Value = param.asInt();
  
  }

BLYNK_WRITE(V4){
  
  
  }



void callForecast(){
  
  OpenWeatherMapForecastData data[MAX_FORECASTS];
  client.setMetric(IS_METRIC);
  client.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
  uint8_t allowedHours[] = {0,12};
  client.setAllowedHours(allowedHours, 2);
  uint8_t foundForecasts = client.updateForecastsById(data, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID, MAX_FORECASTS);
  Serial.printf("Found %d forecasts in this call\n", foundForecasts);
  Serial.println("------------------------------------");
  time_t time;
  for (uint8_t i = 0; i < foundForecasts; i++) {
    Serial.printf("---\nForecast number: %d\n", i);
    // {"dt":1527066000, uint32_t observationTime;
    time = data[i].observationTime;
    Serial.printf("observationTime: %d, full date: %s", data[i].observationTime, ctime(&time));
    // "main":{
    //   "temp":17.35, float temp;
    Serial.printf("temp: %f\n", data[i].temp);
    //   "feels_like": 16.99, float feelsLike;
    Serial.printf("feels-like temp: %f\n", data[i].feelsLike);
    //   "temp_min":16.89, float tempMin;
    Serial.printf("tempMin: %f\n", data[i].tempMin);
    //   "temp_max":17.35, float tempMax;
    Serial.printf("tempMax: %f\n", data[i].tempMax);
    //   "pressure":970.8, float pressure;
    Serial.printf("pressure: %f\n", data[i].pressure);
    //   "sea_level":1030.62, float pressureSeaLevel;
    Serial.printf("pressureSeaLevel: %f\n", data[i].pressureSeaLevel);
    //   "grnd_level":970.8, float pressureGroundLevel;
    Serial.printf("pressureGroundLevel: %f\n", data[i].pressureGroundLevel);
    //   "humidity":97, uint8_t humidity;
    Serial.printf("humidity: %d\n", data[i].humidity);
    //   "temp_kf":0.46
    // },"weather":[{
    //   "id":802, uint16_t weatherId;
    Serial.printf("weatherId: %d\n", data[i].weatherId);
    //   "main":"Clouds", String main;
    Serial.printf("main: %s\n", data[i].main.c_str());
    //   "description":"scattered clouds", String description;
    Serial.printf("description: %s\n", data[i].description.c_str());
    //   "icon":"03d" String icon; String iconMeteoCon;
    Serial.printf("icon: %s\n", data[i].icon.c_str());
    Serial.printf("iconMeteoCon: %s\n", data[i].iconMeteoCon.c_str());
    // }],"clouds":{"all":44}, uint8_t clouds;
    Serial.printf("clouds: %d\n", data[i].clouds);
    // "wind":{
    //   "speed":1.77, float windSpeed;
    Serial.printf("windSpeed: %f\n", data[i].windSpeed);
    //   "deg":207.501 float windDeg;
    Serial.printf("windDeg: %f\n", data[i].windDeg);
    // rain: {3h: 0.055}, float rain;
    Serial.printf("rain: %f\n", data[i].rain);
    // },"sys":{"pod":"d"}
    // dt_txt: "2018-05-23 09:00:00"   String observationTimeText;
    Serial.printf("observationTimeText: %s\n", data[i].observationTimeText.c_str());

    //Every time the weather forecast is called, the pump unlocks
    lockPump = false;

    //If the weatherID is the following it means rain is forecast so the pump will lock
    if(data[0].weatherId == 200 || 201 || 202 || 232 || 302 || 310 || 311 || 312 || 313 || 314 || 321 || 500 || 501 || 502 || 503 || 504 || 511 || 520 || 521 || 522 || 531 || 600 || 601 || 602 || 611 || 612 || 613 || 615 || 616 || 620 || 621 || 622){
      lockPump = true;

      //Change V1 to 1. This will be used by blynk to send a notification to the users phone
      if (lockPump = true){
        Blynk.virtualWrite(V1, 1);
        }
    }

    //V3 will be set to 1 at 5.30am every monday and friday
    if((pin3Value == 1) && (i == 6)){

      //Add up the expected rainfall for the week
      float expectedRainfall = (data[0].rain + data[1].rain + data[2].rain + data[3].rain + data[4].rain + data[5].rain + data[6].rain);
      Serial.println(expectedRainfall);
      if(expectedRainfall >= 25){
        //Send a notification to the users phone via blynk app
        Blynk.virtualWrite(V4, 1);
        delay(10000);
        Blynk.virtualWrite(V4, 0);
        }

      //Water pumps depending on how much rainfall they receive
      if(expectedRainfall >= 20 && expectedRainfall < 25){
        digitalWrite(IN14, HIGH);
        digitalWrite(IN12, HIGH);
        delay(15000);
        digitalWrite(IN14, LOW);
        digitalWrite(IN12, LOW);
      }
      if(expectedRainfall >= 15 && expectedRainfall < 20){
        digitalWrite(IN14, HIGH);
        digitalWrite(IN12, HIGH);
        delay(30000);
        digitalWrite(IN14, LOW);
        digitalWrite(IN12, LOW);
      }
      if(expectedRainfall >= 10 && expectedRainfall < 15){
        digitalWrite(IN14, HIGH);
        digitalWrite(IN12, HIGH);
        delay(45000);
        digitalWrite(IN14, LOW);
        digitalWrite(IN12, LOW);
      }
      if(expectedRainfall >= 5 && expectedRainfall < 10){
        digitalWrite(IN14, HIGH);
        digitalWrite(IN12, HIGH);
        delay(60000);
        digitalWrite(IN14, LOW);
        digitalWrite(IN12, LOW);
      }
      if(expectedRainfall < 5){
        digitalWrite(IN14, HIGH);
        digitalWrite(IN12, HIGH);
        delay(75000);
        digitalWrite(IN14, LOW);
        digitalWrite(IN12, LOW);
        
        }
      //Reset pin value on V3
      Blynk.virtualWrite(V3, 0);  
      
      }
    

    
  }

  Serial.println();
  Serial.println("---------------------------------------------------/\n");

  
}



int sensorPin = A0; 
int outputValue;



void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  connectWifi();


  callForecast();
  Serial.println();
  Serial.println("\n\nNext Loop-Step: " + String(millis()) + ":");

  
pinMode(IN2, OUTPUT);
pinMode(IN14, OUTPUT);
pinMode(IN12, OUTPUT);


}

void loop()
{
  
  
  Blynk.run();

//at 6am everyday V0 will be set to 1 via blynk app
  
  if(pin0Value == 1){
    callForecast();
    Blynk.virtualWrite(V0, 0);
    
    
    }

  //If the pump is locked, no code is run

  if(lockPump = true){
      
  } else{
      outputValue = analogRead(sensorPin);
      Serial.println(outputValue);

      //Change the speed at which the pump operates using PWM
      if(outputValue == 1024){
        analogWrite(IN2, 0);
      } else if (1020 < outputValue && outputValue < 1024) {
        analogWrite(IN2, 64);
      } else if (1015 < outputValue && outputValue < 1021) {
        analogWrite(IN2, 127);
      } else if (1010 < outputValue && outputValue < 1016){
        analogWrite(IN2, 191);
      } else {
        analogWrite(IN2, 255);  
      } 
        
  }

  


  
  
  delay(1000);
}
