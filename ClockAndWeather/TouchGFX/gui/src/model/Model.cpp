#include "clock_weather_app.h"
#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include <gui/common/enums.hpp>
#include <gui/common/structs.hpp>
#include "json_parsers.h"
#include "weather_data.h"

#include <stdio.h>
#include <string.h>


#ifdef SIMULATOR
#include <stdlib.h>     /* srand, rand */
#endif // SIMULATOR

const char* encryption_status[]={"Open", "WEP", "WPA", "WPA2", "WPA-WPA2", "WPA2-TKIP", "Unknown"};

extern net_wifi_scan_results_t  APs[];
extern osMessageQId ClockAndWeatherEvent;
extern char* jsondata;

osMessageQId GUIEvent = 0;
uint8_t password_ascii[64];
uint8_t ssid_ascii[32]; 
uint8_t encryption_ascii[30];
uint32_t prev_request = 0;

Model::Model() : modelListener(0), metric(true), hour24(true), tickCnt(0), numberOfWifiAccesPoints(0), getWifiData(true), connectingToWifi(false), isWifiConected(false), inSettings(false)
{
  /* GUI Queue */
  osMessageQDef(GUI_Queue, 10, uint16_t);
  GUIEvent = osMessageCreate (osMessageQ(GUI_Queue), NULL);
  
  for(int i = 0; i < getCitiesNbr(); i++)
  { 
    cityWeather_C[i].isLoaded = false;
    cityWeather_F[i].isLoaded = false;
  }
  
}

void Model::tick()
{
  osEvent event;
  
  event = osMessageGet(GUIEvent, 0 );
  
  if( event.status == osEventMessage )
  {
    /*Update WIFI Done*/
    switch (event.value.v)
    {
    case UPDATE_WIFI_AP_DONE:
      prev_request = osKernelSysTick();
      updateWifi();
      updateWifiSignalStrength();
      break;
      
      /*Connection to WIFI Error*/
    case CONNECTING_TO_WIFI_ERROR:
      modelListener->wifiConnection(false);
      break;
      
      /*Connection to WIFI OK*/
    case CONNECTING_TO_WIFI_OK:
      setIsWifiConected(true);
      osMessagePut ( ClockAndWeatherEvent, GET_DATE_AND_TIME , 0);
      
      break;
      
      /*Get Date and Time*/
    case GET_DATE_AND_TIME_DONE:
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_TUNIS_C , 0);
      break;
      
    case GET_DATE_AND_TIME_ERROR:
      modelListener->wifiConnection(false);
      break;
           
      /*TUNIS*/
    case GET_WEATHER_FORECAST_TUNIS_C_DONE:
      extractWeatherData(jsondata, &cityWeather_C[CITY_WEATHER::TUNIS]);
      cityWeather_C[CITY_WEATHER::TUNIS].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_C[CITY_WEATHER::TUNIS]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_TUNIS_F , 0);
      break;
      
    case GET_WEATHER_FORECAST_TUNIS_C_ERROR:
            modelListener->wifiConnection(false);
      break;
      
    case GET_WEATHER_FORECAST_TUNIS_F_DONE:
      extractWeatherData(jsondata, &cityWeather_F[CITY_WEATHER::TUNIS]);
      cityWeather_F[CITY_WEATHER::TUNIS].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_F[CITY_WEATHER::TUNIS]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_PARIS_C , 0);
      break;
      
    case GET_WEATHER_FORECAST_TUNIS_F_ERROR:
            modelListener->wifiConnection(false);
      break;
      
      /*PARIS*/
    case GET_WEATHER_FORECAST_PARIS_C_DONE:
      extractWeatherData(jsondata, &cityWeather_C[CITY_WEATHER::PARIS]);
      cityWeather_C[CITY_WEATHER::PARIS].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_C[CITY_WEATHER::PARIS]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_PARIS_F , 0);
      break;
      
      case GET_WEATHER_FORECAST_PARIS_C_ERROR:
              modelListener->wifiConnection(false);
      break;
      
    case GET_WEATHER_FORECAST_PARIS_F_DONE:
      extractWeatherData(jsondata, &cityWeather_F[CITY_WEATHER::PARIS]);
      cityWeather_F[CITY_WEATHER::PARIS].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_F[CITY_WEATHER::PARIS]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_NEW_YORK_C , 0);
      break;
      
    case GET_WEATHER_FORECAST_PARIS_F_ERROR:
            modelListener->wifiConnection(false);
      break;
      
      /* NEW YORK*/
    case GET_WEATHER_FORECAST_NEW_YORK_C_DONE:
      extractWeatherData(jsondata, &cityWeather_C[CITY_WEATHER::NEW_YORK]);
      cityWeather_C[CITY_WEATHER::NEW_YORK].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_C[CITY_WEATHER::NEW_YORK]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_NEW_YORK_F , 0);
      break;
      
      case GET_WEATHER_FORECAST_NEW_YORK_C_ERROR:
              modelListener->wifiConnection(false);
      break;
      
    case GET_WEATHER_FORECAST_NEW_YORK_F_DONE:
      extractWeatherData(jsondata, &cityWeather_F[CITY_WEATHER::NEW_YORK]);
      cityWeather_F[CITY_WEATHER::NEW_YORK].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_F[CITY_WEATHER::NEW_YORK]);
      
      if(getCitiesNbr() > 3)
      {
        osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_CUSTOM_CITY_1_C , 0);
      }
      else
      {
        modelListener->updateWifiIcon(currentConnectedWifi.wifi.signalStrength);
        modelListener->wifiConnection(true);
      }
      break;
      
    case GET_WEATHER_FORECAST_NEW_YORK_F_ERROR:
            modelListener->wifiConnection(false);
      break;
      
       /*Custom City_1*/
    case GET_WEATHER_FORECAST_CUSTOM_CITY_1_C_DONE:
      extractWeatherData(jsondata, &cityWeather_C[CITY_WEATHER::CUSTOM_CITY_1]);
      cityWeather_C[CITY_WEATHER::CUSTOM_CITY_1].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_C[CITY_WEATHER::CUSTOM_CITY_1]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_CUSTOM_CITY_1_F , 0);
      break;
      
      case GET_WEATHER_FORECAST_CUSTOM_CITY_1_C_ERROR:
              modelListener->wifiConnection(false);
      break;
      
    case GET_WEATHER_FORECAST_CUSTOM_CITY_1_F_DONE:
      extractWeatherData(jsondata, &cityWeather_F[CITY_WEATHER::CUSTOM_CITY_1]);
      cityWeather_F[CITY_WEATHER::CUSTOM_CITY_1].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_F[CITY_WEATHER::CUSTOM_CITY_1]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_CUSTOM_CITY_2_C , 0);
      break;
      
    case GET_WEATHER_FORECAST_CUSTOM_CITY_1_F_ERROR:
            modelListener->wifiConnection(false);
      break;
      
       /*Custom City_2*/
    case GET_WEATHER_FORECAST_CUSTOM_CITY_2_C_DONE:
      extractWeatherData(jsondata, &cityWeather_C[CITY_WEATHER::CUSTOM_CITY_2]);
      cityWeather_C[CITY_WEATHER::CUSTOM_CITY_2].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_C[CITY_WEATHER::CUSTOM_CITY_2]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_CUSTOM_CITY_2_F , 0);
      break;
      
      case GET_WEATHER_FORECAST_CUSTOM_CITY_2_C_ERROR:
              modelListener->wifiConnection(false);
      break;
      
    case GET_WEATHER_FORECAST_CUSTOM_CITY_2_F_DONE:
      extractWeatherData(jsondata, &cityWeather_F[CITY_WEATHER::CUSTOM_CITY_2]);
      cityWeather_F[CITY_WEATHER::CUSTOM_CITY_2].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_F[CITY_WEATHER::CUSTOM_CITY_2]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_CUSTOM_CITY_3_C , 0);
      break;
      
    case GET_WEATHER_FORECAST_CUSTOM_CITY_2_F_ERROR:
            modelListener->wifiConnection(false);
      break;
      
      /*Custom City_3*/
    case GET_WEATHER_FORECAST_CUSTOM_CITY_3_C_DONE:
      extractWeatherData(jsondata, &cityWeather_C[CITY_WEATHER::CUSTOM_CITY_3]);
      cityWeather_C[CITY_WEATHER::CUSTOM_CITY_3].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_C[CITY_WEATHER::CUSTOM_CITY_3]);
      osMessagePut ( ClockAndWeatherEvent, GET_WEATHER_FORECAST_CUSTOM_CITY_3_F , 0);
      break;
      
      case GET_WEATHER_FORECAST_CUSTOM_CITY_3_C_ERROR:
              modelListener->wifiConnection(false);
      break;
      
    case GET_WEATHER_FORECAST_CUSTOM_CITY_3_F_DONE:
      extractWeatherData(jsondata, &cityWeather_F[CITY_WEATHER::CUSTOM_CITY_3]);
      cityWeather_F[CITY_WEATHER::CUSTOM_CITY_3].isLoaded = true;
      modelListener->updateWeatherView(&cityWeather_F[CITY_WEATHER::CUSTOM_CITY_3]);
//      setIsWifiConected(true);
      modelListener->updateWifiIcon(currentConnectedWifi.wifi.signalStrength);
      modelListener->wifiConnection(true);
      break;
      
    case GET_WEATHER_FORECAST_CUSTOM_CITY_3_F_ERROR:
            modelListener->wifiConnection(false);
      break;
      
    }
  }
  
  if (getWifiData)
  {
//    if(!wifiIsBusy)
    {
      osMessagePut ( ClockAndWeatherEvent, UPDATE_WIFI_AP , 0);
    }
    getWifiData = false;
  }
  
  if (inSettings)
   {
     if((prev_request != 0) && (osKernelSysTick() - prev_request >= 10000) && !getIsWifiConected())
    {
      osMessagePut ( ClockAndWeatherEvent, UPDATE_WIFI_AP , 0);
      prev_request = osKernelSysTick();
    }
    
   }
  
  if (connectingToWifi)
  {
    osMessagePut ( ClockAndWeatherEvent, CONNECTING_TO_WIFI , 0);
    connectingToWifi = false;
  }
  
  if (tickCnt == 3750)
  {
    //CalendarUpdate(getUTC_Date_Time());
    updateWeather(0);
//    getWifiData = true;
    tickCnt = 0;
  }
  else
  {
    tickCnt++;
  }
  
}

void Model::updateWeather(int CityVal)
{

  struct weatherData weather;
  
  modelListener->updateWeatherView(&weather);
}

struct tm *time_s;
void Model::getWeather(weatherData *weather, int CityVal)
{ 
  if (metric)
  {
    switch (CityVal)
    {
    case CITY_WEATHER::TUNIS:
      if (cityWeather_C[CITY_WEATHER::TUNIS].isLoaded == false)
      {
        extractWeatherData(TUNIS_WEATHER_METRIC, weather);
      }
      else
      {
        *weather = cityWeather_C[CITY_WEATHER::TUNIS];
      }
      break;
      
    case CITY_WEATHER::PARIS:
      if (cityWeather_C[CITY_WEATHER::PARIS].isLoaded == false)
      {
        extractWeatherData(PARIS_WEATHER_METRIC, weather);
      }
      else
      {
        *weather = cityWeather_C[CITY_WEATHER::PARIS];
      }
      break;
      
    case CITY_WEATHER::NEW_YORK:
      if (cityWeather_C[CITY_WEATHER::NEW_YORK].isLoaded == false)
      {
        extractWeatherData(NEW_YORK_WEATHER_METRIC, weather);
      }
      else
      {
        *weather = cityWeather_C[CITY_WEATHER::NEW_YORK];
      }
      break;
      
    case CITY_WEATHER::CUSTOM_CITY_1:
      if (cityWeather_C[CITY_WEATHER::CUSTOM_CITY_1].isLoaded == false)
      {
        extractWeatherData(CUSTOM_CITY_1_WEATHER_METRIC, weather);
        sprintf(weather->city, "%s", getCityName(0));
      }
      else
      {
        *weather = cityWeather_C[CITY_WEATHER::CUSTOM_CITY_1];
      }
      break;
      
    case CITY_WEATHER::CUSTOM_CITY_2:
      if (cityWeather_C[CITY_WEATHER::CUSTOM_CITY_2].isLoaded == false)
      {
        extractWeatherData(CUSTOM_CITY_2_WEATHER_METRIC, weather);
        sprintf(weather->city, "%s", getCityName(1));
      }
      else
      {
        *weather = cityWeather_C[CITY_WEATHER::CUSTOM_CITY_2];
      }
      break; 
      
    case CITY_WEATHER::CUSTOM_CITY_3:
      if (cityWeather_C[CITY_WEATHER::CUSTOM_CITY_3].isLoaded == false)
      {
        extractWeatherData(CUSTOM_CITY_3_WEATHER_METRIC, weather);
        sprintf(weather->city, "%s", getCityName(2));
      }
      else
      {
        *weather = cityWeather_C[CITY_WEATHER::CUSTOM_CITY_3];
      }
      break; 
    }
  }
  else
  {
    switch (CityVal)
    {
    case CITY_WEATHER::TUNIS:
      if (cityWeather_F[CITY_WEATHER::TUNIS].isLoaded == false)
      {
        extractWeatherData(TUNIS_WEATHER_IMPERIAL, weather);
      }
      else
      {
        *weather = cityWeather_F[CITY_WEATHER::TUNIS];
      }
      break;
      
    case CITY_WEATHER::PARIS:
      if (cityWeather_F[CITY_WEATHER::PARIS].isLoaded == false)
      {
        extractWeatherData(PARIS_WEATHER_IMPERIAL, weather);
      }
      else
      {
        *weather = cityWeather_F[CITY_WEATHER::PARIS];
      }
      break;
      
    case CITY_WEATHER::NEW_YORK:
      if (cityWeather_F[CITY_WEATHER::NEW_YORK].isLoaded == false)
      {
        extractWeatherData(NEW_YORK_WEATHER_IMPERIAL, weather);
      }
      else
      {
        *weather = cityWeather_F[CITY_WEATHER::NEW_YORK];
      }
      break;
      
    case CITY_WEATHER::CUSTOM_CITY_1:
      if (cityWeather_F[CITY_WEATHER::CUSTOM_CITY_1].isLoaded == false)
      {
        extractWeatherData(CUSTOM_CITY_1_WEATHER_IMPERIAL, weather);
        sprintf(weather->city, "%s", getCityName(0));
      }
      else
      {
        *weather = cityWeather_F[CITY_WEATHER::CUSTOM_CITY_1];
      }
      break;
      
    case CITY_WEATHER::CUSTOM_CITY_2:
      if (cityWeather_F[CITY_WEATHER::CUSTOM_CITY_2].isLoaded == false)
      {
        extractWeatherData(CUSTOM_CITY_2_WEATHER_IMPERIAL, weather);
        sprintf(weather->city, "%s", getCityName(1));
      }
      else
      {
        *weather = cityWeather_F[CITY_WEATHER::CUSTOM_CITY_2];
      }
      break;
      
    case CITY_WEATHER::CUSTOM_CITY_3:
      if (cityWeather_F[CITY_WEATHER::CUSTOM_CITY_3].isLoaded == false)
      {
        extractWeatherData(CUSTOM_CITY_3_WEATHER_IMPERIAL, weather);
        sprintf(weather->city, "%s", getCityName(2));
      }
      else
      {
        *weather = cityWeather_F[CITY_WEATHER::CUSTOM_CITY_3];
      }
      break;
    }
  }
  sprintf(weather->dates[0], "%s", getCity_Date_Time(weather->timezone));
  weather->timezone = 0;
}

static inline int rssi_to_strength(int16_t RSSI)
{
  if(RSSI >= -50)
  {
    return 4;
  }
  if(RSSI >= -60)
  {
    return 3;
  }
  if(RSSI >= -70)
  {
    return 2;
  }
  if(RSSI >= -100)
  {
    return 1;
  }
    return 0;
}

void Model::updateWifi()
{
  for (int cnt = 0; cnt < MAX_LISTED_AP; cnt++)
  {
    if (APs[cnt].ssid.value[0] == 0) break;
    wifiAccessPoints[cnt].id = cnt;
    touchgfx::Unicode::snprintf(wifiAccessPoints[cnt].ssid, 32, (const char*)APs[cnt].ssid.value);
    touchgfx::Unicode::snprintf(wifiAccessPoints[cnt].encryption, 30,net_wifi_security_to_string(APs[cnt].security));
    wifiAccessPoints[cnt].signalStrength = rssi_to_strength(APs[cnt].rssi);

    if ((APs[cnt].security != NET_WIFI_SM_OPEN) && (APs[cnt].security != NET_WIFI_SM_UNKNOWN))
     {
       wifiAccessPoints[cnt].encrypted = true;
     }
    numberOfWifiAccesPoints=cnt+1;
  }
    
  modelListener->updateWiFiInformaion(wifiAccessPoints, numberOfWifiAccesPoints);

}

void Model::getWifiAccessPoints(wifiData &ap, int id)
{
  memcpy(&ap, &wifiAccessPoints[id], sizeof(ap));
}

void Model::setWifiPassword(touchgfx::Unicode::UnicodeChar *password, int id)
{   
  
#ifdef SIMULATOR
  tickCnt = 0;
  connectingToWifi = true;
#endif // SIMULATOR
  
  touchgfx::Unicode::snprintf(currentConnectedWifi.password, 64, "%s", password);
  touchgfx::Unicode::snprintf(currentConnectedWifi.wifi.ssid, 32, "%s", wifiAccessPoints[id].ssid);
  touchgfx::Unicode::snprintf(currentConnectedWifi.wifi.encryption, 30, "%s", wifiAccessPoints[id].encryption);
  currentConnectedWifi.wifi.signalStrength = wifiAccessPoints[id].signalStrength;
  
  touchgfx::Unicode::toUTF8(password, password_ascii, 64);
  touchgfx::Unicode::toUTF8(wifiAccessPoints[id].ssid, ssid_ascii, 32);
  touchgfx::Unicode::toUTF8(wifiAccessPoints[id].encryption, encryption_ascii, 30);
  
  connectingToWifi = true;
  
}

void Model::setMetric(bool isMetric)
{ 
  metric = isMetric; 
}

bool Model::getMetric()
{
  return metric;
}

void Model::setClockFormat(bool isHour24)
{
  hour24 = isHour24;
}

bool Model::getClockFormat()
{
  return hour24;
}

void Model::setInSettings(bool inSettings)
{
  this->inSettings = inSettings;
}

void Model::setIsWifiConected(bool isWifiConected)
{
  this->isWifiConected = isWifiConected;
}

bool Model::getIsWifiConected()
{
  return isWifiConected;
}

int Model::getCitiesNbr(void)
{
  return custom_cities_nbr() + 3;
}

char* Model::getCityName(int CityVal)
{
  return CityName(CityVal);
}

void Model::updateWifiSignalStrength(void)
{  
  if(getIsWifiConected())
  {
    for(int i = 0; i < numberOfWifiAccesPoints; i++)
    {
      if(touchgfx::Unicode::strncmp(currentConnectedWifi.wifi.ssid, wifiAccessPoints[i].ssid, 32) == 0)
      {
        currentConnectedWifi.wifi.signalStrength = wifiAccessPoints[i].signalStrength;
        modelListener->updateWifiIcon(currentConnectedWifi.wifi.signalStrength);
        break;
      }
    }
  }
}

void Model::returnToMenuLanucher()
{
    HAL_NVIC_SystemReset();
}
