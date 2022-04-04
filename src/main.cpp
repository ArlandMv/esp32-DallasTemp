#include <OneWire.h>
#include <DallasTemperature.h>

/*  
*  CHECK OUT https://youtu.be/fRSGkmnJhpE
*/

// Data wire is plugged into port 27 on the ESP32
#define ONE_WIRE_BUS 27

/* #define SENSOR1 1
#define SENSOR2 2
#define SENSOR3 3 */

/* Address =  0x28 0xE8 0x18 0x7 0xD6 0x1 0x3C 0x43
Address =  0x28 0x9 0x4B 0x7 0xD6 0x1 0x3C 0x86
Address =  0x28 0xBB 0x38 0x7 0xD6 0x1 0x3C 0xCC */

String addressA =  "0x28 0xE8 0x18 0x7 0xD6 0x1 0x3C 0x43"; 

//CHANGE address to {address}

DeviceAddress address1 = {0x28, 0xE8, 0x18, 0x7, 0xD6, 0x1, 0x3C, 0x43};//dirección del sensor 1
DeviceAddress address2 = {0x28, 0x9, 0x4B, 0x7, 0xD6, 0x1, 0x3C, 0x86};//dirección del sensor 2
DeviceAddress address3 = {0x28, 0xBB, 0x38, 0x7, 0xD6, 0x1, 0x3C, 0xCC};//dirección del sensor 3

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer; //for only one sensor?

/*
 * START FUNCTIONS
 */

void clear(){
  Serial.write(27);
  Serial.print("[2J");
  Serial.write(27);
  Serial.print("[H");
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void printTemperatures()
{
  Serial.print("Number of devices found: ");
  Serial.println(sensors.getDeviceCount());

  Serial.println();
  Serial.print("Device 0: ");
  printTemperature(address1);
  Serial.println();
  Serial.print("Device 1: ");
  printTemperature(address2);
  Serial.println();
  Serial.print("Device 2: ");
  printTemperature(address3);
  Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print a device power mode
void printSensorPowerMode()
{
  Serial.println("Sensors Power mode");
  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");  
}

void lookUpSensorAddress();


/*
 * END FUNCTIONS
 */

void setup() {
  // start serial port
  Serial.begin(115200);
  clear();
  delay(10000);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Assign address manually. The addresses below will need to be changed
  // to valid device addresses on your bus. Device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  // Note that you will need to use your specific address here
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

  // Method 1:
  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  
  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them. It might be a good idea to 
  // check the CRC to make sure you didn't get garbage. The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");

  // show the addresses we found on the bus
  /* Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println(); */
  for(int i=0;i<sensors.getDeviceCount();i++) 
  {
    if (!sensors.getAddress(insideThermometer, i)) {
      Serial.println("Unable to find address for Device " + i);
      Serial.println("Temperature for Device "+String(i)+" is: " + String(sensors.getTempCByIndex(i)));
   }else 
    {
      Serial.print("Device ");
      Serial.print(i);
      Serial.print(" Address: ");
      printAddress(insideThermometer);
      Serial.println();
    }
  } 

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();
}

void loop() {
  // print the temperature for each device that is connected to the bus
  // call sensors.requestTemperatures() to issue a global temperature 
  
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // It responds almost immediately. Let's print out the data

  // Use a simple function to print out the data
  //printTemperature(insideThermometer); 

  // Or, use the more detailed method to print out the data of each device
  printTemperatures();

  /* Serial.println("printAddress: ");
  printAddress(insideThermometer);
  Serial.println(); */
  //printSensorPowerMode();

  lookUpSensorAddress();

  Serial.println("change adresses to array of strings");

  delay(5000);
}


void lookUpSensorAddress()
{
  byte addr[8];  
  Serial.println("Obteniendo direcciones:");
  while (oneWire.search(addr)) 
  {  
    Serial.print("Address = ");
    for( int i = 0; i < 8; i++) {
      Serial.print(" 0x");
      Serial.print(addr[i], HEX);
    }
  Serial.println();
  }
}
