#include <Key.h>
#include <Keypad.h>

#include <InteractingObjects_ButtonPad.h>

static const int pad_length = 4;
static const int reed_switch[pad_length] = {48, 49, 50, 51}; //array of reed switches
boolean currentStates[pad_length] = {LOW,LOW,LOW,LOW};
boolean lastStates[pad_length] = {LOW,LOW,LOW,LOW};

const int potPin = A0;
int pinValueCurrent;
int pinValueLast;

byte pinBtn[ROWS]       = {22,23,24,25}; // Buttons rows
byte pinBtnGnd[COLS]    = {26,27,28,29}; // Buttons cols
byte customPinLedRGB[ROWS][3] = {{2,3,44},{5,6,7},{8,9,10},{11,12,45}}; // LEDs rows
byte customPinLedGnd[COLS]    = {30,31,32,33}; // LEDs cols

// "Standard" colors                        
#define COLOR_RED     0
#define COLOR_GREEN   1
#define COLOR_BLUE    2
#define COLOR_YELLOW  3
#define COLOR_MAGENTA 4
#define COLOR_CYAN    5
#define COLOR_WHITE   6
#define COLOR_OFF     7

byte standardColors[][3] =  { 
  {255,0  ,0  }, 
  {0  ,255,0  }, 
  {0  ,0  ,255},
  {255,255,0  },
  {255,0  ,255},
  {0  ,255,255},
  {255,255,255},
  {0  ,0  ,0  }
};

// For keypad
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};

// Creating keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys),pinBtn,pinBtnGnd,ROWS,COLS); 

// Creating led matrix
rgbLedMatrix ledMatrix = rgbLedMatrix(customPinLedRGB,customPinLedGnd,ROWS,COLS);

void setPWMFreq() {
  
  TCCR0B = TCCR0B & B11111000 | B00000011;    // D4 & D13 - Keeping default because Timer0 is used for Arduino timing function ike millis(), delay()...
  
  // set timer 1,2,3,4,5 divisor to 1 for PWM frequency of 31372.55 Hz
  TCCR1B = TCCR1B & B11111000 | B00000001;    // D11 & D12
  TCCR2B = TCCR2B & B11111000 | B00000001;    // D9 & D10 - Used by tone()... but I don't use tone() so I don't care !
  TCCR3B = TCCR3B & B11111000 | B00000001;    // D2, D3 & D5
  TCCR4B = TCCR4B & B11111000 | B00000001;    // D6, D7 & D8
  TCCR5B = TCCR5B & B11111000 | B00000001;    // D44, D45 & D46
}
//-------------------------------------------------------------------------
// What to do when a key is pressed
//-------------------------------------------------------------------------
void onKeyPressed(char keyCode) {
  byte row=keyCode/4;
  byte col=keyCode%4;

  //Serial.print("row"); Serial.println(row);
  //Serial.print("col"); Serial.println(col);

  
  ledMatrix.matrixLedSetState(row,col,COLOR_RED);
  //ledMatrix.matrixLedSetRandom(row,col); // Switch the led on with a randow color or changing the color if on  
  //ledMatrix.matrixLedUnlock(row,col);    // ... and "unlock" the led, i.e. when button is release, switch it off
}


void setup() 
{

  //make each reed switch in the array an input pullup
  for (int i = 0; i < pad_length; i++) {
    pinMode(reed_switch[i], INPUT_PULLUP);
  }
  pinMode(potPin, INPUT);

  setPWMFreq();     // Setting a higher frequency for PWM
  
  // Just testing leds
  ledMatrix.ledTestAll(standardColors[COLOR_RED]);
  ledMatrix.ledTestAll(standardColors[COLOR_GREEN]);
  ledMatrix.ledTestAll(standardColors[COLOR_BLUE]);
  ledMatrix.ledTestAll(standardColors[COLOR_WHITE]);
  
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() 
{

  // At least one key state changed
  if (customKeypad.getKeys()){

    for (int i=0; i<LIST_MAX; i++)  {  // Go through 10 keys max
        if ( customKeypad.key[i].stateChanged ) { // Check if state changed
          
          switch (customKeypad.key[i].kstate) {         // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                
                case PRESSED:
                  onKeyPressed(customKeypad.key[i].kcode);
                  break;
          }
        }
    }
  }

  // refreshing the led matrix (all we did up until now is updating the status "logically")
  ledMatrix.matrixLedRefresh(500);

  for (int i = 0; i < pad_length; i++) {

    currentStates[i] = digitalRead(reed_switch[i]);

    if (currentStates[i] == LOW && lastStates[i] == HIGH) {
      //if switch was triggered
      midiMessage(0xB0, 0, i, 0x7F);
      //Serial.println(i + " was pressed");
    }
    else if (currentStates[i] == HIGH && lastStates[i] == LOW) {
      //if switch was detriggered
      midiMessage(0xB0, 0, i, 0x00);
      //Serial.println(i + " was detriggered");
    }
    
    lastStates[i] = currentStates[i];
    
  }
  
}

//send MIDI message
void midiMessage(byte control, byte chan, byte note, byte vel) {
  Serial.write(control + chan);
  Serial.write(note);
  Serial.write(vel);
}

