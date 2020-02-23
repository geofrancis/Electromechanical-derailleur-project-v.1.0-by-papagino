// Electromechanical derailleur project v.1.0 by papagino http://52.25.253.50/forums/memberlist.php?mode=viewprofile&u=59295&sid=81266145bcda6c02b6cc8622467bdf6e
// Assumes you know the servo positions for each gear (determined using v.0.0 software)

#include <Servo.h> 
#include <EEPROM.h>            //Need this because we'll be saving the gear selection in EEPROM (to avoid shifts on startup)
#include <SevSeg.h>            //This is used for the 7 Segment LED Display to show what gear the bike is
SevSeg sevseg;


#define db_time 20             //Button debounce time in mS
#define servo_pin 9            //HW pin to which servo signal line is attached
#define servoDelta 1           //Amt servo moves with each button push (degress)
#define upButtonPin 2          //Up button push grounds it
#define dnButtonPin 3          //Dn button push grounds it
#define led_pin 10             //LED Turn ON when button is pressed
#define highestGear 9          //Number of sprockets on rear hub
#define threshold 1000       // Long press button time threshold in mS

#define rearGear1 176           //Servo position values for the various rear sprockets (numbered low to high gears, not by size)
#define rearGear2 154          //These were determined empirically using the diyshift0 sketch while hooked up to a laptop
#define rearGear3 136
#define rearGear4 118
#define rearGear5 100
#define rearGear6 84
#define rearGear7 64
#define rearGear8 48
#define rearGear9 28

#define rearGearAddr 0            //EEPROM address for saving the gear selected

Servo rearServo;                  // Create servo object to control a servo 
int   lastUpButtonState = 1;
int   lastDnButtonState = 1;
int   rearPos = 90;               // variable to store the servo position
int   rearGear;                   //Numbered sprocket (1 is lowest gear; 9 is highest)
 
void setup() 
{ 
  rearGear = EEPROM.read(rearGearAddr);
  rearServo.attach(servo_pin);                          //Attach the rear servo to the servo object
  
  if ((rearGear > 0) && (rearGear < 10))
  {
    shiftToRearGear(rearGear);                          //And immediately set it to whatever gear was last saved in EEPROM
  }
  else
  {
    shiftToRearGear(3);                                 //If not valid, goto gear 3
  }

    byte numDigits = 1;
    byte digitPins[] = {};
    byte segmentPins[] = {6, 5, 4, 7, 8, 11, 12, 13};
    bool resistorsOnSegments = true;

    byte hardwareConfig = COMMON_ANODE; 
    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
    sevseg.setBrightness(90);
  
  pinMode(upButtonPin, INPUT);
  pinMode(dnButtonPin, INPUT);
  pinMode(led_pin, OUTPUT);
   
  digitalWrite(upButtonPin, HIGH);                      //Enable internal pullup resistors
  digitalWrite(dnButtonPin, HIGH);
  digitalWrite(led_pin, LOW);

  Serial.begin(9600);    //USED ONLY FOR DEBUG
} 


 
 
void loop() 
{ 
  int upButtonState = digitalRead(upButtonPin);          //Poll the up/dn buttons
  int dnButtonState = digitalRead(dnButtonPin);
  int count;                                             //variables to count the time button is held

  if (upButtonState == 0){                               //Button was pushed
    delay(db_time);                                      //Wait a bit to debounce the button
    upButtonState = digitalRead(upButtonPin);            //Test again
    if (upButtonState == 0 && lastUpButtonState == 1){   //If still down AND was up before
      upOneGear();                                       //Shift up one gear
      lastUpButtonState = 0;                             //Save state
      digitalWrite(led_pin, HIGH);                       // Turn On LED when button is pressed
    }
  }
  else {                                                  //upButton must be open (button state = 1)
    delay (db_time);                                      //Wait a bit to debounce the button
    upButtonState = digitalRead(upButtonPin);             //Test again
    if (upButtonState == 1){                              //Still open, save state
      lastUpButtonState = 1;
      digitalWrite(led_pin, LOW);
    }
  }
  //
  //Now same for dowm
  //
  if (dnButtonState == 0){                               //Button was pushed
    delay(db_time);                                      //Wait a bit to debounce the button
    dnButtonState = digitalRead(dnButtonPin);            //Test again
  
    if (dnButtonState == 0 && lastDnButtonState == 1){   //If still down AND was up before
      dnOneGear();                                       //Shift down one gear
      lastDnButtonState = 0;                             //Save state
      digitalWrite(led_pin, HIGH);                       // Turn On LED when button is pressed
    }
  else if(dnButtonState == 0 && lastDnButtonState == 0)
  {
    unsigned long last_time=millis(); 
    
    do
    {
     dnButtonState = digitalRead(dnButtonPin);
     if(dnButtonState == 1)
      {
       break;
      }
     else if((millis()-last_time)>threshold)  
      {
       dnRearGear1();
      }
     
    }
   while(1);  
    
  
  }
  }
  else {                                                  //dnButton must be open (button state = 1)
    delay (db_time);                                      //Wait a bit to debounce the button
    dnButtonState = digitalRead(dnButtonPin);             //Test again
    if (dnButtonState == 1){                              //Still open, save stated
      lastDnButtonState = 1;
      digitalWrite(led_pin, LOW);
    }
  }
  

  
  
}
//
//Functions called
//
  void upOneGear(){
    if (rearGear < highestGear){
      rearGear = rearGear + 1;
    }
    shiftToRearGear(rearGear);
  }
  
    void dnOneGear(){
    if (rearGear > 1){
      rearGear = rearGear - 1;
    }
    shiftToRearGear(rearGear);
  }
  
  void dnRearGear1(){
    if (rearGear > 1){
      rearGear = 1;
    }
    shiftToRearGear(rearGear);
  }
  
  void shiftToRearGear(int gear){
      EEPROM.write(rearGearAddr, gear);
      switch (gear) {
    case 1:
      rearServo.write(rearGear1);
      sevseg.setNumber(1);
        sevseg.refreshDisplay();
      break;
    case 2:
      rearServo.write(rearGear2);
      sevseg.setNumber(2);
        sevseg.refreshDisplay();
      break;
    case 3:
      rearServo.write(rearGear3);
      sevseg.setNumber(3);
        sevseg.refreshDisplay();
      break;
    case 4:
      rearServo.write(rearGear4);
      sevseg.setNumber(4);
        sevseg.refreshDisplay();
      break;
    case 5:
      rearServo.write(rearGear5);
      sevseg.setNumber(5);
        sevseg.refreshDisplay();
      break;
    case 6:
      rearServo.write(rearGear6);
      sevseg.setNumber(6);
        sevseg.refreshDisplay();
      break;
    case 7:
      rearServo.write(rearGear7);
      sevseg.setNumber(7);
        sevseg.refreshDisplay();
      break;
    case 8:
      rearServo.write(rearGear8);
      sevseg.setNumber(8);
        sevseg.refreshDisplay();
      break;
    case 9:
      rearServo.write(rearGear9);
      sevseg.setNumber(9);
        sevseg.refreshDisplay();
      break;
