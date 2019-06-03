#define C  262
#define D  294
#define E  330
#define F_NOTE  349
#define G  392
#define A  440
#define B  494
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const PROGMEM int LED1 = 13;
const PROGMEM int LED2 = 7;
const PROGMEM int LED3 = 6;
const PROGMEM int LED4 = 10;
const PROGMEM int LED5 = 9;

byte pointer[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00000,
};

const PROGMEM String lyrics[2][33] = {{ //[2][48] if uncommented
  "Twin", "kle ", "twin", "kle ", "lit", "tle ", 
  "star", ". . . ", "How ", "I ", "won", "der ",
  "what ", "you ", "are", ". . . ", "Up ", "a", "bove ",
  "the ", "world ", "so ", "high ", ". . . ", "like ", 
  "a ", "dia", "mond ", "in ", "the ", "sky", ". . . ", "      "
//  "Twin", "kle ", "twin", "kle ", "lit", "tle ", 
//  "star", ". . . ", "How ", "I ", "won", "der ",
//  "what ", "you ", "are ", ". . . ", 
}, {
  "Ma", "ry ", "had ", "a ", "lit", "tle ", "lamb", ", ",
  "lit", "tle ", "lamb ", ", ", "lit", "tle ", "lamb ", ", ", 
  "Ma", "ry ", "had ", "a ", "lit", "tle ", "lamb, ",
  "it's ", "fleece ", "was ", "white ", "as ", "snow ", ". . .", "        "
} };
int notes[2][33] = {{
  C, C, G, G, A, A, G, 0,
  F_NOTE, F_NOTE, E, E, D, D, C, 0,
  G, G, F_NOTE, F_NOTE, E, E, D, 0,
  G, G, F_NOTE, F_NOTE, E, E, D, 0//,
//  C, C, G, G, A, A, G, 0,
//  F_NOTE, F_NOTE, E, E, D, D, C, 0 
  },{
  E, D, C, D, E, E, E, 0,
  D, D, D, 0, E, G, G, 0, 
  E, D, C, D, E, E, E, E, 
  D, D, E, D, C, 0
  }
};

int lyricIndex = 0; //keeps track of which lyric has been printed
int selection = -1; //stores user selection for songs
String lcdText = "         "; //stores what will be displayed on LCD

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, pointer);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
}

void loop(){

  getSelection();

  //Fill lcd with lyrics
  while (lcdText.length() < 16){ 
    addNextLyric();
  }
  lcd.print(lcdText);
  delay(2000);

  //Repeat for each note in the song
  for (int i = 0; i < 32; i++){ 
    tone(8, notes[selection][i], 1000); //Play note

    //Get a random LCD, if there is a note playing make it flash
    int randomLcd = getRandomLcd(); 
    if (notes[i] != 0) digitalWrite(randomLcd, HIGH);

    //Repeat for each letter in the lyric
    for (int j = 0; j < lyrics[selection][i].length(); j++){
      scrollLyrics();
      delay(1000/(lyrics[selection][i].length()));
    }
    digitalWrite(randomLcd, LOW);
    delay(40); //Leave space between each note
  }

  lyricIndex = 0; //reset lcd after playing song
  lcdText = "         ";
}


void getSelection(){
  boolean valid = false; //Is valid if user enters 1 or 2
  lcd.clear();
  lcd.print("Choose a song:");
  delay(2000);

  //Repeat until 1 or 2 is typed
  while(!valid){
    lcd.clear();
    lcd.print("1. Twinkle Twin-"); 
    lcd.setCursor(0, 1);
    lcd.print(" kle Little Star");
    delay(1500);
    lcd.clear();
    lcd.print("2. Mary Had a");
    lcd.setCursor(0, 1);
    lcd.print(" Little Lamb");
    delay(1500);
    
    if (Serial.available() > 0){  //Something in buffer
      char input = Serial.read(); //Read it
      if (input == '1' || input == '2'){
        selection = input - '1'; //convert input to int and subtract 1
        valid = true;
      }
      else { //Not 1 or 2 as input (invalid)
        input = 'a'; //reset selection
        lcd.clear();
        lcd.print("Invalid Number...");
        delay(2000);
      }
    } //end if serial.available

    while(Serial.available() > 0){ //Clear Serial buffer
      Serial.read();
    }
  } //end while !valid
}


void scrollLyrics() {
  lcdText = lcdText.substring(1); //remove first char
  if (lcdText.length() < 16){ //add next lyric if needed
    addNextLyric();
  }
  //Draw arrow and lyrics
  lcd.clear();
  lcd.setCursor(8, 1);
  lcd.write(byte(0));
  lcd.home();
  lcd.print(lcdText);
}

void addNextLyric(){
  lcdText += lyrics[selection][lyricIndex++];
}

int getRandomLcd(){
  long randomNum = random(1, 6); //Gets random number from 1(incl.) to 6(excl.)
  switch(randomNum) {
    case 1: return LED1;
    break;
    case 2: return LED2;
    break;
    case 3: return LED3;
    break;
    case 4: return LED4;
    break;
    case 5: return LED5;
    break;
  }
}
