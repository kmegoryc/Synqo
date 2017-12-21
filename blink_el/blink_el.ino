#define A 2
#define D 5
#define F 7
#define H 9

byte commandByte;
byte noteByte;
byte velocityByte;

void checkMIDI(){
  do{
    if (Serial.available()){
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
      if (commandByte == 144 && velocityByte > 0){//if note on message
        switch (noteByte) {
        case 60:
          digitalWrite(H,HIGH);
          break;
        case 62: 
          digitalWrite(F,HIGH);
          break;
        case 64:
          digitalWrite(D,HIGH);
          break;
        case 66:
          digitalWrite(A,HIGH);
          break;
        }
      }
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}

void setup(){
  Serial.begin(9600);
  
  pinMode(A, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(H, OUTPUT);
  digitalWrite(H, LOW);
  digitalWrite(F, LOW);
  digitalWrite(D, LOW);
  digitalWrite(A, LOW);

  
}

void loop(){

  checkMIDI(); 
  delay(100);
  digitalWrite(H, LOW);
  digitalWrite(F, LOW);
  digitalWrite(D, LOW);
  digitalWrite(A, LOW);

}


