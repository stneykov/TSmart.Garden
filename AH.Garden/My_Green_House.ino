/*
 * Angel Hodjev
 
 */

//  Includes section
// include TFT and SPI libraries
#include <TFT.h>  

//  Tony: Bilbioteka za SPI interfeis, neobhodimo za ekrana
#include <SPI.h>

// ***************** DHT22 BEGIN *****************
//  Tony: Dobaviame DHT biblioteka
//  Problem s bibliotekata!!! (Pri men go niama)
#include <DHT.h>

// ***************** BH1750_WE library *****************
#include <Wire.h>
#include <BH1750_WE.h>

// ***************** SHT21 library *****************
#include <Sodaq_SHT2x.h>
//  ***************** Definitions ***************** 
//  BH1750
#define BH1750_ADDRESS 0x23 //  Light Sensor Address

//  Relay
#define RPIN 4 // Relay Digital PIN

//  DHT22
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//  TFT
#define cs   10
#define dc   9
#define rst  8

#define REVERSE_RED_AND_BLUE 1   //  TONY: Оказа се че на някой екрани червеното и синьото са разменени

//  Ultrasonic Sonar
#define EchoPin 2   // The Arduino's the Pin2 connection to US-100 Echo / RX
#define TrigPin 3   // The Arduino's Pin3 connected to US-100 Trig / TX

//  ***************** END         ***************** 

//  Colors
#define BLACK   0x0000
#define BLUE    ToBGR(0x001F)// ConvertRGB(0,0,155)//0x001F
#define RED     ToBGR(0xF800)
#define GREEN   ToBGR(0x07E0)
#define MAGENTA ToBGR(0xF81F)
#define YELLOW  ToBGR(0xFFE0)
#define WHITE   ToBGR(0xFFFF)
#define GRAY    ToBGR(0x7BEF)
#define LIGHT_GRAY ToBGR(0xC618)
#define LIME    ToBGR(0x87E0)
#define AQUA    ToBGR(0x5D1C)
#define CYAN    ToBGR(0x07FF)
#define DARK_CYAN ToBGR(0x03EF)
#define ORANGE  ToBGR(0xFCA0)
#define PINK    ToBGR(0xF97F)
#define BROWN   ToBGR(0x8200)
#define VIOLET  ToBGR(0x9199)
#define SILVER  ToBGR(0xA510)
#define GOLD    ToBGR(0xA508)
#define NAVY    ToBGR(0x000F)
#define MAROON  ToBGR(0x7800)
#define PURPLE  ToBGR(0x780F)
#define OLIVE   ToBGR(0x7BE0)
 
BH1750_WE myBH1750(BH1750_ADDRESS); 

//
//Constants

int globalTftLineOffset = 0;
const int globalLineSize = 10;

DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
int dht_chk;
float dht_hum;  //Stores humidity value
float dht_temp; //Stores temperature value

float sht_hum;
float sht_temp;

//  Tony: Moisture 
int cs_wet = 300;
int cs_middle = 400;
int cs_dry = 500;
bool cs_pump_on = false;

//  Tony: Moisture 2



// ***************** DHT22 END *****************
// ***************** Relay *****************

// ***************** Relay END *****************
// ***************** TFT BEGIN *****************
// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
//  ***************** TFT END  *****************

//  Sonar Variables
unsigned long Time_Echo_us = 0;
unsigned long Len_mm  = 0;

void setup() 
{
    //initialize the library
    TFTscreen.begin();
    TFTscreen.setRotation(2);
    // clear the screen with a black background
    TFTscreen.background(0, 0, 0);

    Serial.begin(9600);
    dht.begin();

    pinMode(RPIN, OUTPUT);
    pinMode(A1, INPUT);

    Wire.begin();
    myBH1750.init();

    //    Ultrasonic
    pinMode(EchoPin, INPUT);    //  The set EchoPin input mode.
    pinMode(TrigPin, OUTPUT);   //  The set TrigPin output mode.

    //  Static TFT Text
    //TFTscreen.stroke(255,255,255);
    //TFTscreen.setTextSize(1);
    //TFTscreen.text("Tony", 0, 0);
}

void loop() 
{
    globalTftLineOffset = 0;
    //  Read all data

    //  TONY: DHT22 Reading
    //Read data and store it to variables hum and temp
    dht_hum = dht.readHumidity();
    dht_temp= dht.readTemperature();

    //  TONY: SHT21

    sht_hum = SHT2x.GetHumidity();
    sht_temp = SHT2x.GetTemperature();
    Serial.print("Humidity(%RH): ");
    Serial.print(sht_hum);
    Serial.print("     Temperature(C): ");
    Serial.println(sht_temp);
    Serial.print("     Dewpoint(C): ");
    Serial.println(SHT2x.GetDewPoint());
  
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(dht_hum);
    Serial.print(" %, Temp: ");
    Serial.print(dht_temp);
    Serial.println(" Celsius");

    //  TONY: Capacitvie Soil Mousture Sensor 1.2 Reading
    int cs_val;
    cs_val = analogRead(A0);
    int cs_val2=0;
    cs_val2 = analogRead(A1);

    //  TONY: Read BH1750 Light Sensor
    float lux = myBH1750.getLux();
  
    Serial.print("CS Value: ");
    Serial.println(cs_val);
    Serial.print("CS Value2: ");
    Serial.println(cs_val2);

    //  TONY: Read Sonar))) Sensor Data
    GetSonarData();
    
    //  Send all data to the TFS display
    // set the color for the figures
    //TFTscreen.stroke(redRandom, greenRandom, blueRandom);

    //  Clear screen
//    TFTscreen.stroke(0,0,0);
//    TFTscreen.background(0,0,0);
  
    //  TONY
    // Print readed values of External and Internal Temperature
    //TFTscreen.fillRect(64, 0, 64, 20, MAGENTA);
    
    //  Print DHT_TEMP
    //printText("Text", COLOR, X, Y, TextSize);
    printToTFT("------- DHT22 -------", WHITE, 0);
    movePrintLineDown();
    printToTFT("Temp: ", WHITE, 0);
    printToTFT(dht_temp, YELLOW, 30);
    
    printToTFT("Hum:  ", WHITE, 70);
    printToTFT(dht_hum, YELLOW, 95);

    movePrintLineDown();
    globalTftLineOffset += 7;
    printToTFT("------- SHT21 -------", WHITE, 0);
    movePrintLineDown();
    printToTFT("Temp: ", WHITE, 0);
    printToTFT(sht_temp, YELLOW, 30);
    
    printToTFT("Hum:  ", WHITE, 70);
    printToTFT(sht_hum, YELLOW, 95);

    movePrintLineDown();
    globalTftLineOffset += 7;
    printToTFT("---Capacitive Soil---", WHITE, 0);
    movePrintLineDown();
    printToTFT("Soil Value: ", WHITE, 0);
    printToTFT(cs_val, YELLOW, 70);

    movePrintLineDown();
    globalTftLineOffset += 7;
    printToTFT("---Moisure Soil v2---", WHITE, 0);
    movePrintLineDown();
    printToTFT("Soil Value: ", WHITE, 0);
    printToTFT(cs_val2, YELLOW, 70);

    movePrintLineDown();
    globalTftLineOffset += 7;
    printToTFT("-------BH 1750-------", WHITE, 0);
    movePrintLineDown();
    printToTFT("Lux: ", WHITE, 0);
    printToTFT(lux, YELLOW, 30);
    
    movePrintLineDown();
    globalTftLineOffset += 7;
    printToTFT("------- US100 -------", WHITE, 0);
    movePrintLineDown();
    printToTFT("Distance: ", WHITE, 0);
    printToTFT(Len_mm, YELLOW, 70);
    
    //printText("------- DHT22 -------", WHITE, 0,0,1);
    
    //printText("Temp: ", WHITE, 0, 0, 1);
    //  PrintValue(sensorValue, COLOR, X, Y, TextSize);
    //printValue(dht_temp, WHITE, 30, 0, 1);
     
    //  Print DHT_HUM
    //printText("Hum: ", WHITE, 70, 0, 1);
    //printValue(dht_hum, WHITE, 95, 0, 1);
    
    //  Print CS_VAL/Soil1
    //printText("Soil1: ", WHITE, 0, 90, 1);
    //printValue(cs_val, WHITE, 38, 90, 1);

    //  Print CS_VAL2/Soil2
    //printText("Soil2: ", WHITE, 0, 100, 1);
    //printValue(cs_val2, WHITE, 38, 100, 1);

    //printText("Lux: ", WHITE, 0,110, 1);
    //printValue(lux, WHITE, 38, 110, 1);

    

    //printText("Dist: ", WHITE, 0, 120, 1);
    //printValue(Len_mm, WHITE, 38, 120, 1);

    //  Moisture logic
    if (cs_val > cs_dry && !cs_pump_on)
    {
        digitalWrite(RPIN, LOW);    
        Serial.println("Turn water pump on");
        cs_pump_on = true;
    }
    if (cs_val < cs_middle && cs_pump_on)
    {
        digitalWrite(RPIN, HIGH);
        Serial.println("Turn water pump off");
        cs_pump_on = false;
    }

    Serial.print(F("LightSensor: "));
    Serial.print(lux);
    Serial.println(F(" Lux"));

    delay(5000);
/*
  // light up a single point
  TFTscreen.point(80,64);
  // wait 200 miliseconds until change to next figure
  delay(500);

  // draw a line
  TFTscreen.line(0,64,160,64);
  delay(500);

  //draw a square
  TFTscreen.rect(50,34,60,60);
  delay(500);
    
  //draw a circle
  TFTscreen.circle(80,64,30);
  delay(500);
*/
  //erase all figures
  //TFTscreen.background(0,0,0);

}

// ***************************************************************************
// Function GetSonarData
int GetSonarData()
{
    Serial.print("Sonar))) ");
    // By the Trig / Pin sending pulses trigger US-100 ranging
    digitalWrite(TrigPin, HIGH);                         // Send pulses begin by Trig / Pin
    delayMicroseconds(50);                               // Set the pulse width of 50us (> 10us)
    digitalWrite(TrigPin, LOW);                          // The end of the pulse    
    Time_Echo_us = pulseIn(EchoPin, HIGH);               // A pulse width calculating US-100 returned
    Serial.println(Time_Echo_us);
    if((Time_Echo_us < 60000) && (Time_Echo_us > 1)) 
    {
        // Pulse effective range (1, 60000).
        // Len_mm = (Time_Echo_us * 0.34mm/us) / 2 (mm) 
        Len_mm = (Time_Echo_us*34/100)/2;                   // Calculating the distance by a pulse width.   
        Serial.print("Present Distance is: ");              // Output to the serial port monitor 
        Serial.print(Len_mm, DEC);                          // Output to the serial port monitor   
        Serial.println("mm");                               // Output to the serial port monitor
    }
}

// Function Print float
void printValue(float num, uint16_t color, int x, int y, int textSize)
{
  TFTscreen.setCursor(x, y);
  TFTscreen.setTextColor(color);
  TFTscreen.setTextSize(textSize);
  TFTscreen.setTextWrap(true);
  TFTscreen.print(num);
  // printText1(value, RED,18,130,1);
}

// Function Print float
void printText(String text, uint16_t color, int x, int y, int textSize)
{
    /*
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(textSize);
  TFTscreen.text(text.c_str(), x, y);

  */
  TFTscreen.setCursor(x,y);
  TFTscreen.setTextColor(color, BLUE);
  TFTscreen.setTextSize(textSize);
  TFTscreen.print(text);
}

void movePrintLineDown()
{
    globalTftLineOffset += globalLineSize;
}
void printToTFT(String text, uint16_t color, int x)
{
    TFTscreen.setCursor(x, globalTftLineOffset);
    TFTscreen.setTextColor(color, BLACK);
    TFTscreen.setTextSize(1);
    TFTscreen.print(text);
}

void printToTFT(float value, uint16_t color, int x)
{
    TFTscreen.setCursor(x, globalTftLineOffset);
    TFTscreen.setTextColor(color, BLACK);
    TFTscreen.setTextSize(1);
    TFTscreen.print(value);
}

word ConvertRGB( byte R, byte G, byte B)
{
    if (REVERSE_RED_AND_BLUE)
    {
        return ( ((B & 0xF8) << 8) | ((G & 0xFC) << 3) | (R >> 3) );
    }
    return ( ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3) );
}

word ToBGR(word color)
{
    if (REVERSE_RED_AND_BLUE)
    {
        uint16_t r = color >> 11;
        uint16_t g = color & 0x07E0;
        uint16_t b = color & 0x1F;
        return b << 11 | g | r;
    }
    return color;
}
