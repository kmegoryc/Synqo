 //config variables
#define NUM_LED_COLUMNS (4)
#define NUM_LED_ROWS (4)
#define NUM_BTN_COLUMNS (4)
#define NUM_BTN_ROWS (4)
#define NUM_COLORS (1)

#define MAX_DEBOUNCE (3)

// Global variables
static bool LED_buffer[NUM_LED_COLUMNS][NUM_LED_ROWS];

static const uint8_t btncolumnpins[NUM_BTN_COLUMNS] = {26, 27, 28, 29};
static const uint8_t btnrowpins[NUM_BTN_ROWS]       = {22, 23, 24, 25};
static const uint8_t ledcolumnpins[NUM_LED_COLUMNS] = {30, 31, 32, 33};
static const uint8_t colorpins[NUM_LED_ROWS]        = {2, 5, 8, 11};

static int8_t debounce_count[NUM_BTN_COLUMNS][NUM_BTN_ROWS];

static const int beat_length = 16;
static const int pad_length = 4;
static const int reed_switch[pad_length] = {40, 42, 44, 46}; //array of reed switches
boolean currentStates[pad_length] = {LOW,LOW,LOW,LOW};
boolean lastStates[pad_length] = {LOW,LOW,LOW,LOW};

static const int potPin = A0; //potentiometer pin
int potValue = 0;

int curKey;

static void setuppins()
{
  uint8_t i;
  
  // LED columns
  for (i = 0; i < NUM_LED_COLUMNS; i++)
  {
    pinMode(ledcolumnpins[i], OUTPUT);

    // with nothing selected by default
    digitalWrite(ledcolumnpins[i], HIGH);
  }

  // button columns
  for (i = 0; i < NUM_BTN_COLUMNS; i++)
  {
    pinMode(btncolumnpins[i], OUTPUT);

    // with nothing selected by default
    digitalWrite(btncolumnpins[i], HIGH);
  }

  // button row input lines
  for (i = 0; i < NUM_BTN_ROWS; i++)
  {
    pinMode(btnrowpins[i], INPUT_PULLUP);
  }

  // LED drive lines
  for (i = 0; i < NUM_LED_ROWS; i++)
  {
    pinMode(colorpins[i], OUTPUT);
    digitalWrite(colorpins[i], LOW);
  }

  // Initialize the debounce counter array
  for (uint8_t i = 0; i < NUM_BTN_COLUMNS; i++)
  {
    for (uint8_t j = 0; j < NUM_BTN_ROWS; j++)
    {
      debounce_count[i][j] = 0;
    }
  }
}

static void scan()
{
  static uint8_t current = 0;
  uint8_t val;
  uint8_t i, j;

  // Select current columns
  digitalWrite(btncolumnpins[current], LOW);
  digitalWrite(ledcolumnpins[current], LOW);

  // output LED row values
  for (i = 0; i < NUM_LED_ROWS; i++)
  {
    if (LED_buffer[current][i])
    {
      digitalWrite(colorpins[i], HIGH);
    }
  }

  // pause a moment
  delay(1);

  // Read the button inputs
  for ( j = 0; j < NUM_BTN_ROWS; j++)
  {
    val = digitalRead(btnrowpins[j]);

    if (val == LOW)
    {
      // active low: val is low when btn is pressed
      if ( debounce_count[current][j] < MAX_DEBOUNCE)
      {
        debounce_count[current][j]++;
        if ( debounce_count[current][j] == MAX_DEBOUNCE )
        {
          //Serial.println("Key Down ");
          //Serial.println((current * NUM_BTN_ROWS) + j);
          curKey = (current * NUM_BTN_ROWS) + j;
          // toggle the current LED state
          LED_buffer[current][j] = !LED_buffer[current][j];
        }
      }
    }
    else
    {
      // otherwise, button is released
      if ( debounce_count[current][j] > 0)
      {
        debounce_count[current][j]--;
        if ( debounce_count[current][j] == 0 )
        {
          //Serial.println("Key Up ");
          //Serial.println((current * NUM_BTN_ROWS) + j);

          // If you want to do something when a key is released, do it here:

        }
      }
    }
  }// for j = 0 to 3;

  delay(1);

  digitalWrite(btncolumnpins[current], HIGH);
  digitalWrite(ledcolumnpins[current], HIGH);

  for (i = 0; i < NUM_LED_ROWS; i++)
  {
    digitalWrite(colorpins[i], LOW);
  }

  current++;
  if (current >= NUM_LED_COLUMNS)
  {
    current = 0;
  }

}

void setup()
{

  //make each reed switch in the array an input pullup
  for (int i = 0; i < pad_length; i++) {
    pinMode(reed_switch[i], INPUT_PULLUP);
  }

  //make potentiometer an input
  pinMode(potPin, INPUT);
  
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Serial.println("Starting Setup...");

  // setup hardware
  setuppins();

  // init global variables
  for (uint8_t i = 0; i < NUM_LED_COLUMNS; i++)
  {
    for (uint8_t j = 0; j < NUM_LED_ROWS; j++)
    {
      LED_buffer[i][j] = 0;
    }
  }

  //Serial.println("Setup Complete.");

}

void reed_switches() {

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

void loop() {

  reed_switches();

  scan();

}

