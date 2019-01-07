/*
 * AUTHOR: Junaid Oloruntobi
 * CO-AUTHORS: Adebisi Tobi Stephen(Spyke), Adeniran Adebayo
 * DATE: OCTOBER, 2017
 * ***************************
 * ****** ABOUT PROGRAM*******
This program is for a demo attendance time sheet with biometrics fingerprint sensor using an arduino 
Mega as the size of the sketch is more than the arduino Uno memory. The project is based on the adafruit 
fingerprint sensor library and adapted for use for this project. The hardware comprises of the FPM-10A 
fingerprint sensor connected to a PC. The interface for controlling the hardware from the computer is
a special excel spreadsheet with a macro. Refer to the README text file for more information.
Only three main program contained in the library; FINGERPRINT, ENROL AND DELETE are concatenated into 
this program as saparate funtions and can be accessed with any of the three push buttons on the device.
******************RECOMMENDATIONS************************************
*The authors would appreciate contributions and recommendations on how to improve on this program for more 
*efficient performance. Albeit, on our own part, we are working on a windows desktop GUI for easy 
*contrl and flexibility. We would keep you notify on any changes.
*/


#include <Adafruit_Fingerprint.h> 
#include <SoftwareSerial.h>
#include "LiquidCrystal.h" 
#include <Pushbutton.h> //Library for preventing debouncing on push button. Contains a small chunk of code
#include <SPI.h>        //Required for reading/writing from/to SD card
#include <SD.h>         //The SD card library

 //The Device push button function keys connected to arduino Mega's analog pins
#define BUTTON_PIN A1 
#define BUTTON_PIN2 A2
#define BUTTON_PIN3 A3
#define CS_pin 53 //the SD card chip select pin connected to Mega's pin 53

Pushbutton button(BUTTON_PIN);  
Pushbutton button2(BUTTON_PIN2);
Pushbutton button3(BUTTON_PIN3);
//Variables for storing data
String firstName;
String surName;
String matNumber;
char text[100];
String text2;
char matArray[10];
char surnArray[20];
char firstArray[20];


uint8_t id; //Integer variable for which the fingerprint module maps saved print template as in its memory. 0-255
            //Also when retrieving a saved print, it returns the id number if found in its memory
//Defining the prototype function for getting prints            
uint8_t getFingerprintEnroll(int id);
int i=0;
LiquidCrystal lcd(4,5,6,7,8,9);
//The Fingerprint connects to the microcontroller via serial communication
SoftwareSerial mySerial(13, 12); //The TX and RX pins of the sensor connects to Mega's 13 and 12. Refer to the FPM10A Datasheet to confirm

//Variables for SD card data
File myFile;
String filename;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() { 
    lcd.begin(16,2);
    finger.begin(57600); 
    Serial.begin(9600);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Electronic");
    lcd.setCursor(0,1);
    lcd.print("Biometric System");
    delay(2000);
    
 if (finger.verifyPassword()) {
   lcd.clear();
   lcd.println("  Fingerprint ");
   lcd.setCursor(2,1);
   lcd.print("Sensor OK");
   delay(2000);
    } 
 else{
   lcd.clear();
   lcd.println("  Fingerprint ");
   lcd.setCursor(0,1);
   lcd.print(" Sensor Not OK");
   delay(2000);
    while (1);
   }
 lcd.clear();
 lcd.print("Initializing SD ");
 lcd.setCursor(0,1);
 lcd.print("Card.......");
 delay(1000);
 if (!SD.begin(CS_pin)) {
    lcd.print("Initialization");
    lcd.setCursor(4,1);
    lcd.print("failed!");
    delay(2000);
    lcd.clear();
    lcd.print("Check SD card");
    delay(1000);
    return;
  }
lcd.clear();
lcd.print("Initialization");
lcd.setCursor(6,1);
lcd.print("OK");
delay(1000);
  
}


void loop() {
 
  lcd.clear();
  lcd.print("Press Button to");
  lcd.setCursor(0,1);
  lcd.print("Select Option");
  delay(10);
  
  //Serial.println("Press Button to Select option");
  if(button.getSingleDebouncedRelease())
      getFingerprintEnroll();
  else if(button2.getSingleDebouncedRelease())
      getFingerprintID();
  else if(button3.getSingleDebouncedRelease())
      { id = 0;
  lcd.clear();
  lcd.print("Type in ID");
  lcd.setCursor(0,1);
  lcd.print("to delete...");
  delay(3000);
  Serial.println("CLEARSHEET");
  delay(1000);
  Serial.println("LABEL, ID No==>");
  do{lcd.clear();
     lcd.print("Enter ID in B1");
     lcd.setCursor(0,1);
     lcd.print("press OK");
     delay(10);} 
     while(!button3.getSingleDebouncedRelease());
     Serial.println("CELL,GET,B1");
     id=Serial.readStringUntil(10).toInt();
     delay(1000);
  id=uint8_t(id);
  deleteFingerprint(id);}
}

/////ENROLLING(STORING ONTO DATABASE) CODE//////////////
uint8_t getFingerprintEnroll(){
  while(true){ 
  int p = -1;
  lcd.clear();
  lcd.print("  Enrolling");
  lcd.setCursor(1,1);
  lcd.print("Place Finger");
  delay(2000);
  //////////FIRST IMAGE CAPTURE//////////////
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      
    case FINGERPRINT_OK:
    lcd.clear();
    //Serial.println("Image taken");
    lcd.print("Image taken");
    delay(1000);
    lcd.clear();
    lcd.print(" Remove Finger");
    delay(1000);
    break;
      
    case FINGERPRINT_NOFINGER:
    //Serial.println(".");
    lcd.clear();
    lcd.setCursor(0,1); 
    lcd.print("Waiting...");
    break;
    
    default:
    //Serial.println("Unknown error");
    lcd.clear();
    lcd.setCursor(3,0); 
    lcd.print("Unknown");
    lcd.setCursor(5,1);
    lcd.print("Error");
    delay(1000);
    break;
    }  
    
  }/////////ENDS FIRST IMAGE CAPTURE/////////////

  // OK success!

////////FIRST IMAGE CONVERSION//////////////////////
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("Image");
      lcd.setCursor(3,1);
      lcd.print("Converted");
      delay(2000);
      break;
      
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Invalid Image");
      lcd.clear(); 
      lcd.setCursor(4,0);
      lcd.print("Invalid");
      lcd.setCursor(5,1);
      delay(2000);
      return p;
      
    default:
    //Serial.println("Unknown error");
    lcd.clear();
    lcd.setCursor(3,0); 
    lcd.print("Unknown");
    lcd.setCursor(5,1);
    lcd.print("Error");
    delay(2000);
    return p;
  }////////ENDS FIRST CONVERSION////////////
  
  p = 0;
  
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  //Serial.println("Place same finger again");
  lcd.clear();
  lcd.print("Place same ");
  lcd.setCursor(0,1);
  lcd.print("finger again ");
  delay(2000);
  
  /////SECOND IMAGE CAPTURE//////////////////
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      lcd.clear();
      lcd.print("Image taken");
      delay(2000);
      break;
    
    case FINGERPRINT_NOFINGER:
     // Serial.print(".");
      lcd.clear();
      lcd.setCursor(0,1); lcd.print("Waiting...");
      break;
    
    default:
     // Serial.println("Unknown error");
      lcd.clear();
      lcd.print("Unknown Error");
      delay(1000);
      break;
    }
  }/////ENDS SECOND CAPTURE////////////////

///////BEGIN SECOND IMAGE CONVERSION////////////////
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("Image");
      lcd.setCursor(3,1);
      lcd.print("Converted");
      delay(2000);
      break;
    default:
   // Serial.println("Unknown error");
    lcd.clear();
    lcd.setCursor(3,0); 
    lcd.print("Unknown");
    lcd.setCursor(5,1);
    lcd.print("Error");
    delay(2000);
    return p;
  }//////END SECOND CONVERSION////////////////
  
  // OK converted!
  //Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
   // Serial.println("Prints matched!");
    lcd.clear();
    lcd.print("Prints matched!");
    delay(1000);
    lcd.clear();
/**************************************************************/
/************REQUEST USER DETAILS VIA PC**********************/
       lcd.clear();
       lcd.print("Setting up PC");
       lcd.setCursor(0,1);
       lcd.print("User Interface");
       delay(1000);
       Serial.println("CLEARSHEET");
       delay(1000);
       Serial.println("MSG,Enrolling in progress");
       Serial.println("LABEL,ID Number,Matric No,Surname,Firstname");
       delay(1000);
       lcd.clear();
       lcd.print("Enter details on");
       lcd.setCursor(3,1);
       lcd.print("Computer");
       delay(2000);
       do{
       lcd.clear();
       lcd.print("    Press OK   ");
       lcd.setCursor(3,1);
       lcd.print("When Ready");
       delay(10);
       }
       while(!button3.getSingleDebouncedRelease());
           lcd.clear();
           lcd.print("Fetching Data");
           lcd.setCursor(0,1);
           lcd.print("from Computer");
           Serial.println("CELL,GET,A2");
           id= Serial.readStringUntil(10).toInt();
           delay(1000);
           Serial.println("CELL,GET,B2");
           matNumber=Serial.readStringUntil(10);
           delay(1000);
           Serial.println("CELL,GET,C2");
           surName=Serial.readStringUntil(10);
           delay(1000);
           Serial.println("CELL,GET,D2");
           firstName=Serial.readStringUntil(10);
           delay(1000); 
           
       filename=String(id)+".txt";
       myFile=SD.open(filename);
       myFile.close();
       SD.remove(filename);
       myFile=SD.open(filename,FILE_WRITE);
       if(myFile)
         {lcd.clear();
          lcd.print("Writing Details");
          lcd.setCursor(0,1);
          lcd.print("to Database");
          myFile.println(matNumber+" "+surName+" "+firstName+" ");
          myFile.close();
          lcd.clear();
          lcd.print(surName);
          lcd.setCursor(0,1);
          lcd.print(firstName);
          delay(2000);
          lcd.clear();
          lcd.print("ID No:");
          lcd.print(" ");
          lcd.print(id);
          lcd.setCursor(0,1);
          lcd.print("EN");
          lcd.print(matNumber);
          delay(3000);
          lcd.clear();
        
          }
       else
         {lcd.clear();
          lcd.print("Error opening");
          lcd.setCursor(5,1);
          lcd.print("file");
          delay(2000);     
         }   
   }
   
   else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.print("Fingerprints did");
    lcd.setCursor(2,1);
    lcd.print("not Match");
    delay(2000);
    return p;
  } else {
    //Serial.println("Unknown error");
    lcd.clear();
    lcd.print("Unknown error");
    delay(2000);
    return p;
  }   
  
  //Serial.print("I.D number "); 
  lcd.clear();
  lcd.print("I.D number ");
  Serial.println(id);
  lcd.print(id);
  p = finger.storeModel(id);
  
  if (p == FINGERPRINT_OK) {
    //Serial.println("Stored!");
    lcd.setCursor(0,1);
    lcd.print(" Stored");
    delay(2000);
  } 
    else {
    //Serial.println("Unknown error");
    lcd.clear();
    lcd.print("Unknown Error");
    return p;
  }
  break;   
 }

surName=" ";
id=0;
matNumber=" ";
firstName=" ";
}

//////////ATTENDANCE LOG CODE///////////////////
uint8_t getFingerprintID() {int i;
  Serial.println("CLEARSHEET");
  Serial.println("MSG,Enrolling in progress");
  delay(1000);
  Serial.println("LABEL,Date,Time,ID Number,Matric No,Surname,Firstname");
  delay(1000);
  
 while(true){ 
  lcd.clear(); 
  lcd.print("Place Finger");
  delay(4000);
  uint8_t p = finger.getImage(); 
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      lcd.clear();
      lcd.print("Image taken");
      delay(2000);
      break;
      
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      lcd.clear();
      lcd.print("   No finger  ");
      lcd.setCursor(3,1);
      lcd.print("detected");
      delay(2000);
      break;
      //return p;
    default:
      //Serial.print("Unknown error ");
     // Serial.println(p);
      lcd.clear();
      lcd.print("Unknown Error");
      delay(2000);
      break;
      //return p;
  }////////////end Image capture////////////////////
  
//////////////Image conversion//////////////////////
  p = finger.image2Tz();
  switch (p) {
   case FINGERPRINT_OK:
      //Serial.println("Image converted");
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("Image");
      lcd.setCursor(3,1);
      lcd.print("Converted");
      delay(2000);
      break; 
    default:
   // Serial.println("Unknown error");
    lcd.clear();
    lcd.setCursor(3,0); 
    lcd.print("Unknown");
    lcd.setCursor(5,1);
    lcd.print("Error");
    delay(2000);
    return p;
  }
  
  //OPERATIONS TO BE PERFORMED WHEN IMAGE HAS BEEN CONVERTED 
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Found a print match!");
    lcd.clear();
    lcd.print("Match found in");
    lcd.setCursor(3,1);
    lcd.print("database");
    id=finger.fingerID;
    filename=String(id)+".txt";
    myFile=SD.open(filename);
    while(myFile.available())
    {text[i++]=(static_cast<char>(myFile.read()));
      }
    text2=text;
    int j=text2.indexOf(' ');
    for (int i=0;i<j;i++)
      matArray[i]=text[i];
    matNumber=matArray;
    i=(1+j);
    int p=text2.indexOf(' ',i);
    int k=0;
    for(;i<p;i++)
      surnArray[k++]=text[i];
    surName=surnArray;
    i=(p+1); k=0; p=(text2.length()-2);
    for(;i<=p;i++)
     firstArray[k]=text[i];
    firstName=firstArray;
    Serial.println((String)"DATA,DATE,TIME,"+id+","+matNumber+","+surName+","+firstName+",AUTOSCROLL_20");
    delay(1000);
    Serial.println("SAVEWORKBOOK");
    Serial.println("CELL,GET,A2");
    //String workBookName=Serial.readStringUntil(10);
    //delay(1000);
    //Serial.println("SAVEWORKBOOKAS,"+workBookName);
    delay(2000);
  } 
  else if (p == FINGERPRINT_NOTFOUND) {
    //Serial.println("Did not find a match");
      lcd.clear();
      lcd.print("Did not find");
      lcd.setCursor(0,1);
      lcd.print("a match");
      delay(2000);
      //return p;
   
  } else {
    //Serial.println("Unknown error");
    lcd.clear();
    lcd.setCursor(3,0); 
    lcd.print("Unknown");
    lcd.setCursor(5,1);
    lcd.print("Error");
    delay(2000);
    //return p;
  }   
 }
 
}///////END ATTENDANCE LOG CODE////////////////////

/////DELETE DATA FROM DATABASE INSTRUCTION////////////////////


uint8_t deleteFingerprint(uint8_t id) {
  do{lcd.clear();
     lcd.print("delete ");
     lcd.print(id);
     lcd.print(".Press");
     lcd.setCursor(0,1);
     lcd.print("OK to Confirm");
     delay(10);}
   while(!button3.getSingleDebouncedRelease()); 
  lcd.clear();
  lcd.print("deleting ID #");
  lcd.print(id);
  
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("ID ");
    lcd.print(id);
    lcd.print(" Deleted!");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication ");
    lcd.setCursor(5,1);
    lcd.print("error");
    delay(2000);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.clear();
    lcd.print("Could not delete");
    lcd.setCursor(0,1);
    lcd.print("in that location");
    delay(2000);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.clear();
    lcd.print("Error!");
    delay(2000);
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error 0"); 
    lcd.print(p, HEX);
    delay(2000);
    return p;
  }   
}
