#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "pump-automation.firebaseio.com"                     //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "nvjyJOZqPrcYb6m89OdH02k84frLkqYOmB3lW6Xa"       //Your Firebase Database Secret goes here
#define WIFI_SSID "CD"                                               //your WiFi SSID for which yout NodeMCU connects
#define WIFI_PASSWORD "12345678"                                      //Password of your wifi network 

#define Relay1 15 //D8
int val1, val2, val3, b1, b2, b3;

// defines pins numbers
const int trigPin = 2;  //D4 Top tank
const int echoPin = 0;  //D3
const int trigPin1 = 5;  //D1 Bottom tank
const int echoPin1 = 4;  //D2

#define LED1 14 //D5
#define LED2 12 //D6
#define LED3 13 //D7

// defines variables for sensor 1
long duration;
float distance;
int percentage;
float maxlevel, tankheight, EmptySpace, ActualDist, Temp;

// defines variables for sensor 2
long duration1;
float distance1;
int percentage1;
float maxlevel1, tankheight1, EmptySpace1, ActualDist1, Temp1;

void setup() {

  pinMode(Relay1, OUTPUT);

  pinMode(LED1, INPUT);
  pinMode(LED2, INPUT);
  pinMode(LED3, INPUT);



  digitalWrite(Relay1, LOW);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600); // Starts the serial communication

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setInt("S1", 0);
}

void firebasereconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {

  b1 = digitalRead(LED1);
  b2 = digitalRead(LED2);
  b3 = digitalRead(LED3);

  if (Firebase.failed())
  {
    Serial.print("setting number failed:");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }
  val2 = Firebase.getString("S2").toInt();
  val3 = Firebase.getString("S3").toInt();                                      //Reading the value of the varialble Status from the firebase

  if (val2 == 1 && val3 == 1)                                                        // If, the Status is 1, turn on the Relay1
  {
    digitalWrite(Relay1, HIGH);
    Serial.println("PUMP  ON");
    Firebase.setInt("R1", 0);
    Firebase.setInt("R2", 0);
  }
  else if (val2 == 1 && val3 == 0)                                                 // If, the Status is 0, turn Off the Relay1
  {
    digitalWrite(Relay1, LOW);
    Serial.println("PUMP  OFF");
    Firebase.setInt("R2", 1);
    Firebase.setInt("R1", 0);
  }

  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = (duration * 0.034) / 2;

  tankheight = 19;
  maxlevel = 0.80 * tankheight;;
  EmptySpace = tankheight - maxlevel;
  Temp = distance - EmptySpace;
  ActualDist = maxlevel - Temp;
  percentage = (ActualDist / maxlevel * 100);

  Firebase.setInt("topt", percentage);

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("Percent Top: ");
  Serial.println(percentage);
  delay(200);

  // Clears the trigPin
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoPin1, HIGH);

  // Calculating the distance
  distance1 = (duration1 * 0.034) / 2;

  tankheight1 = 19;
  maxlevel1 = 0.80 * tankheight1;;
  EmptySpace1 = tankheight1 - maxlevel1;
  Temp1 = distance1 - EmptySpace1;
  ActualDist1 = maxlevel1 - Temp1;
  percentage1 = (ActualDist1 / maxlevel1 * 100);

  Firebase.setInt("gndt", percentage1);

  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance1);
  Serial.print("Percent ground: ");
  Serial.println(percentage1);
  delay(200);

  val1 = Firebase.getString("S1").toInt();

  if (val1 == 1 && val2 == 0)
  {

    if (percentage >= 80)
    {
      digitalWrite(Relay1, LOW);
      Serial.println("PUMP  OFF");
      Firebase.setInt("R2", 1);
      Firebase.setInt("R1", 0);
    }

    if (percentage < 80 && percentage1 < 50)
    {
      digitalWrite(Relay1, LOW);
      Serial.println("PUMP  OFF");
      Firebase.setInt("R2", 1);
      Firebase.setInt("R1", 0);
    }

    if (percentage <= 20 && percentage1 >= 50)
    {
      digitalWrite(Relay1, HIGH);
      Serial.println("PUMP  ON");
      Firebase.setInt("R1", 0);
      Firebase.setInt("R2", 0);
    }

    if (b1 == 1)
    {
      Firebase.setInt("E1", 1);
      Serial.println("DRY RUN");
      digitalWrite(Relay1, LOW);
      Firebase.setInt("R2", 1);
      Firebase.setInt("R1", 0);
      Serial.println("PUMP  OFF");
    }
    else if (b1 == 0)
    {
      Firebase.setInt("E1", 0);

    }
    if (b2 == 1)
    {
      Firebase.setInt("E2", 1);
      Serial.println("OVERLOADING");
      digitalWrite(Relay1, LOW);
      Firebase.setInt("R2", 1);
      Firebase.setInt("R1", 0);
      Serial.println("PUMP  OFF");
    }
    else if (b2 == 0)
    {
      Firebase.setInt("E2", 0);

    }
    if (b3 == 1)
    {
      Firebase.setInt("E3", 1);
      Serial.println("SINGLE PHASE");
      digitalWrite(Relay1, LOW);
      Firebase.setInt("R2", 1);
      Firebase.setInt("R1", 0);
      Serial.println("PUMP  OFF");
    }
    else if (b3 == 0)
    {
      Firebase.setInt("E3", 0);

    }
  }

}
