#include <ESP8266WiFi.h> //provides the ability to connect to the WiFi
#include <ESP8266HTTPClient.h> //affords HTTP functionality to your sketch
#include <ArduinoJson.h> //provides the ability to parse and construct JSON objects

const char* ssid = "University of Washington";
const char* pass = "";
// const char* latitude = "47.608013";
// const char* longtitude = "-122.335167";
// const char* date = "2019-04-11";

// Here we create a new data type definition, a box to hold other data types for each name:value pair coming in from the service, 
// we will create a slot in our structure to hold our data
typedef struct {
  String rise;
  String set;
  String day;
} Sun;    //then we give our new data structure a name so we can use it in our code

typedef struct {
  String number;
  String name;
} Space;    //then we give our new data structure a name so we can use it in our code

Sun conditions; //we have created a Sun type, but not an instance of that type, so we create the variable 'conditions' of type Sun
Space people; //we have created a Space type, but not an instance of that type, so we create the variable 'people' of type Space

// Setup routine runs once when you press reset or uploading the code to the arduino
void setup() {
  Serial.begin(115200); // initialize serial communications at 115200 bps
  delay(10); // wait for 10 ms
  // Prints the results to the serial monitor
  Serial.print("This board is running: ");  //Prints that the board is running
  Serial.println(F(__FILE__));
  Serial.print("Compiled: "); //Prints that the program was compiled on this date and time
  Serial.println(F(__DATE__ " " __TIME__));
  
  Serial.print("Connecting to "); Serial.println(ssid); //Prints that the arduino is connected to the WiFi

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  //while arduino is connected to wifi print . to serial monitor
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(); 
  Serial.println("WiFi connected"); //prints that we are connected to the wifi

  getConditions(); //calles the geConditions function to get sunrise, sunset and day length info

  //prints a series of text describing the time information about sunrise, sunset and day length
  Serial.println();
  Serial.println("Below is the time that the sunrise and sunset will happen in Seattle on 2019-04-11.");
  Serial.println("The Sunrise will happen at " + conditions.rise + " UTC .");
  Serial.println("The Sunset will happen at " + conditions.set + " UTC .");
  Serial.println("The day will last for " + conditions.day + " hrs.");
  Serial.println();

  getISS(); //calles the getISS function to get information about who is in space and which craft they are in
}

void loop() {
  //if we put any get...() here, it would ping the endpoint over and over . . . DOS attack?
}

void getConditions() { 
  HTTPClient theClient; //instantiate a new HTTPClient object
  Serial.println("Making getConditions HTTP request");
//  theClient.begin("https://api.Sunrise-Sunset.org/json?lat=" + latitude + "&lng=" + longtitude + "&date=" + date);
  theClient.begin("http://api.sunrise-sunset.org/json?lat=47.608013&lng=-122.335167&date=2019-04-11");  //return as .json object
  int httpCode = theClient.GET();

  //checks wether got an error while trying to access the website/API url
  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Received getConditions HTTP payload.");
      String payload = theClient.getString();
      DynamicJsonBuffer jsonBuffer;
      Serial.println("Parsing getConditions...");
      JsonObject& root = jsonBuffer.parseObject(payload);

      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed in getConditions().");
        return;
      }
      //collects values from JSON keys and stores them as strings because the slots in Sun are strings
      conditions.rise = root["results"]["sunrise"].as<String>(); 
      conditions.set = root["results"]["sunset"].as<String>();
      conditions.day = root["results"]["day_length"].as<String>();
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getConditions().");//prints the statement in case of failure connecting to the end point
  }
}

void getISS() { 
  HTTPClient theClient; //instantiate a new HTTPClient object
  Serial.println("Making getISS HTTP request");
  theClient.begin("http://api.open-notify.org/astros.json");  //return as .json object
  int httpCode = theClient.GET();

  //checks wether got an error while trying to access the website/API url
  if (httpCode > 0) {
    if (httpCode == 200) {
      Serial.println("Received getISS HTTP payload.");
      String payload = theClient.getString();
      DynamicJsonBuffer jsonBuffer;
      Serial.println("Parsing getISS...");
      JsonObject& root = jsonBuffer.parseObject(payload);

      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed in getISS().");
        return;
      }

      //collects values from JSON keys and stores them as strings because the slots in Space are strings
      locations.number = root["number"].as<String>();
      Serial.println();
      Serial.println("Below is the number of people in space and which craft they are in.");
      Serial.println("Number: " + people.number);
      int arraySize = root["number"].as<int>(); //collects and stores arraySize value as an int for later use to print information in an array
      
      //Used to print all the text inside the people array
      //Uses the arraySize variable to know how many times the loop will run and print which name/craft based on i number (avoids printing duplicates)
      for (int i = 0; i < arraySize; i++) {
        people.name = root["people"][i].as<String>();
        Serial.println(people.name);
      }
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getISS().");//prints the statement in case of failure connecting to the end point
  }
}
