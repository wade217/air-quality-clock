#include <Wire.h>                   // I2C communication
#include <LiquidCrystal_I2C.h>      // LCD
#include <RTClib.h>                 // RTC
#include <DHT.h>                    // TEMP & HUMID
//#include <MQ2.h>                  // Smoke sensor
LiquidCrystal_I2C lcd(0x27, 16, 4); // Create LCD with I2C address 0x27, 16 characters per line, 2 lines
RTC_DS3231 rtc;                     // Create rtc for the DS3231 RTC module, address is fixed at 0x68
#define DHTPIN 4                    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11               // DHT 11
DHT dht(DHTPIN, DHTTYPE);           //
float sensor = A0;                  //Air quality sensor setup mq2
float gas_value;                    //
int pbuttonPin = 2;                 //Push Button Relay 1 Control
int relayPin = 10;                  //
int val = 0;                        //
int lightON = 0;                    //
int pushed = 0;                     //
//int pbuttonPin2 = 3;                //Push Button Relay 2 Control
//int relayPin2 = 11;                 //
//int val2 = 0;                       //
//int lightON2 = 0;                   //
//int pushed2 = 0;                    //

/*
   function to update RTC time using user input
*/
void updateRTC()
{

  lcd.clear();  // clear LCD display
  lcd.setCursor(0, 0);
  lcd.print("Edit Mode...");

  // ask user to enter new date and time
  const char txt[6][15] = { "year [4-digit]", "month [1~12]", "day [1~31]",
                            "hours [0~23]", "minutes [0~59]", "seconds [0~59]"
                          };
  String str = "";
  long newDate[6];

  while (Serial.available()) {
    Serial.read();  // clear serial buffer
  }

  for (int i = 0; i < 6; i++) {

    Serial.print("Enter ");
    Serial.print(txt[i]);
    Serial.print(": ");

    while (!Serial.available()) {
      ; // wait for user input
    }

    str = Serial.readString();  // read user input
    newDate[i] = str.toInt();   // convert user input to number and save to array

    Serial.println(newDate[i]); // show user input
  }

  // update RTC
  rtc.adjust(DateTime(newDate[0], newDate[1], newDate[2], newDate[3], newDate[4], newDate[5]));
  Serial.println("RTC Updated!");
}
/*
   function to update LCD text
*/
void updateLCD()
{

  /*
     create array to convert digit days to words:

     0 = Sunday    |   4 = Thursday
     1 = Monday    |   5 = Friday
     2 = Tuesday   |   6 = Saturday
     3 = Wednesday |
  */
  const char dayInWords[7][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

  /*
     create array to convert digit months to words:

     0 = [no use]  |
     1 = January   |   6 = June
     2 = February  |   7 = July
     3 = March     |   8 = August
     4 = April     |   9 = September
     5 = May       |   10 = October
     6 = June      |   11 = November
     7 = July      |   12 = December
  */
  const char monthInWords[13][4] = {" ", "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
                                   };

  // get time and date from RTC and save in variables
  DateTime rtcTime = rtc.now();

  int ss = rtcTime.second();
  int mm = rtcTime.minute();
  int hh = rtcTime.twelveHour();
  int DD = rtcTime.dayOfTheWeek();
  int dd = rtcTime.day();
  int MM = rtcTime.month();
  int yyyy = rtcTime.year();

  // move LCD cursor to upper-left position
  lcd.setCursor(0, 0);

  // print date in dd-MMM-yyyy format and day of week
  if (dd < 10) lcd.print("0");  // add preceeding '0' if number is less than 10
  lcd.print(dd);
  lcd.print("-");
  lcd.print(monthInWords[MM]);
  lcd.print("-");
  lcd.print(yyyy);

  lcd.print("   ");
  lcd.print(dayInWords[DD]);

  // move LCD cursor to lower-left position
  lcd.setCursor(0, 1);

  // print time in 12H format
  if (hh < 10) lcd.print("0");
  lcd.print(hh);
  lcd.print(':');

  if (mm < 10) lcd.print("0");
  lcd.print(mm);
  lcd.print(':');

  if (ss < 10) lcd.print("0");
  lcd.print(ss);

  if (rtcTime.isPM()) lcd.print(" PM"); // print AM/PM indication
  else lcd.print(" AM");
}
void temph()
{
  // Wait a few seconds between measurements.
  delay(100);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  // float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  // float hic = dht.computeHeatIndex(t, h, false);
  lcd.setCursor(10, 2);
  lcd.print(F("Humid: "));
  lcd.print(h, 0);
  lcd.print(F("%"));
  lcd.setCursor(0, 2);
  lcd.print(F("Temp: "));
  //  Serial.print(t);
  //  Serial.print(F("°C "));
  lcd.print((f - 10.2), 0);
  lcd.print(F("F"));
  //  Serial.print(F("°F  Heat index: "));
  //  Serial.print(hic);
  //  Serial.print(F("°C "));
  //  Serial.print(hif);
  //  Serial.println(F("°F"));
}
void mq2()
{
  lcd.setCursor(0, 3);
  lcd.print(F("MQ2: "));
  gas_value = analogRead(sensor);
  lcd.print((gas_value), 0);
  lcd.setCursor(10, 3);
  lcd.print(F("FAN: LOW"));
  delay(100);
}
void setup()
{
  Serial.begin(9600);     // initialize serial

  lcd.init();             // initialize lcd
  lcd.backlight();        // switch-on lcd backlight

  rtc.begin();            // initialize rtc
  dht.begin();            // init DHT11

  pinMode(sensor, INPUT); //mq2

  pinMode(pbuttonPin, INPUT_PULLUP); //Button 1
  pinMode(relayPin, OUTPUT);         //Relay 1
//  pinMode(pbuttonPin2, INPUT_PULLUP);//Button 2
//  pinMode(relayPin2, OUTPUT);        //Relay 2
}
void relay() {
  val = digitalRead(pbuttonPin);
//  val2 = digitalRead(pbuttonPin2);


  //Relay 1
  if (val == LOW && lightON == LOW) {

    pushed = 1 - pushed;
    delay(100);
  }
  lightON = val;

  if (pushed == HIGH) {
//    Serial.println("Light ON");
    digitalWrite(relayPin, LOW);

  } else {
//    Serial.println("Light OFF");
    digitalWrite(relayPin, HIGH);

  }



  //Relay 2


//  if (val2 == HIGH && lightON2 == LOW) {

//   pushed2 = 1 - pushed2;
//    delay(100);
//  }

//  lightON2 = val2;

//  if (pushed2 == HIGH) {
//    Serial.println("Light ON");
//    digitalWrite(relayPin2, LOW);

//  } else {
//    Serial.println("Light OFF");
//    digitalWrite(relayPin2, HIGH);

//  }


  delay(100);
}
void loop()
{
  updateLCD();            // update LCD
  temph();                // update temp & humidity
  mq2();                  // update mq2
  relay();                // update relays & control buttons

  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'u') updateRTC();  // update RTC time
  }
}
