// ----------------------
// Particle Photon Dimmer
// ----------------------


#define DEBUGME
#ifdef DEBUGME
	#define DEBUGp(message)		Serial.print(message)
	#define DEBUGpln(message)	Serial.println(message)
#else
	#define DEBUGp(message)
	#define DEBUGpln(message)
#endif
const int brightnessAddress = 0;
int led1 = D7;
int AC_LOAD = D5;
int dimming = 65;  // Dimming level (1-127)
int old = A0;
int brightness = 45;


void zero_cross_int()  //function to be fired at the zero crossing to dim the light
{
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms 
  // Every zerocrossing : (50Hz)-> 10ms (1/2 Cycle) For 60Hz: (1/2 Cycle) => 8.33ms 
  // 10ms=10000us 
  // 10000 10 / 128 = 78 For 60Hz: (8333us - 8.33us) / 128 = 65 

  int dimtime = (78*dimming);    // For 60Hz =>65    
  delayMicroseconds(dimtime);    // Wait till firing the TRIAC
  digitalWrite(AC_LOAD, HIGH);   // Fire the TRIAC
  delayMicroseconds(10);         // triac On propogation delay (for 60Hz use 8.33)
  digitalWrite(AC_LOAD, LOW);    // No longer trigger the TRIAC (the next zero crossing will swith it off) TRIAC
}

void setup()
{

   pinMode(led1, OUTPUT);
   pinMode(AC_LOAD, OUTPUT);// Set AC Load pin as output
   attachInterrupt(D4, zero_cross_int, RISING);
   
   Particle.function("pow",ledPower);
   Particle.function("set",setBrightness);
   Particle.function("inc",increaseBrightness);
   Particle.function("dec",decreaseBrightness);
   
   Particle.variable("brightness", brightness);
   brightness = EEPROM.read(brightnessAddress);
   setDimming(brightness);
}

void loop()  {

}

void setDimming (int value) {
    setLed(value);
    value = 127 * value / 100;
    dimming = 128 - (value % 128);
    dimming = dimming > 127? 127 : dimming;
    EEPROM.write(brightnessAddress, brightness);
}

void setLed(int brightness) {
    if (brightness > 10) {
        digitalWrite(led1,HIGH);
    }
    else {
        digitalWrite(led1,LOW);
    }
}

int increaseBrightness(String Command ) { 
    int increasedBrigntness = brightness + 5;
    brightness = increasedBrigntness > 100 ? 100 : increasedBrigntness;
    setDimming(brightness);
    return brightness;
}

int decreaseBrightness(String Command ) { 
    int decreasedBrigntness = brightness - 5;
    brightness = decreasedBrigntness < 0 ? 0 : decreasedBrigntness;
    setDimming(brightness);
    return brightness;
}

int setBrightness(String Command ) {
    //char *commandChars = Command;
    brightness = (int)(roundf(strtod(Command,NULL))); // Round the brightness
    brightness = brightness > 100 ? 100 : brightness < 0 ? 0 : brightness; // Bound the brightness
    setDimming(brightness);
    return brightness;
}
int ledPower(String command) {
    if (brightness > 0) {
        int oldBrightness = brightness;
        brightness = 0;
        setDimming(brightness);
        EEPROM.write(brightnessAddress, oldBrightness > 20 ? oldBrightness : 20);
    }
    else {
        brightness = EEPROM.read(brightnessAddress);
        setDimming(brightness);
    }
}

