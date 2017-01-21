#define TA A1
#define POTI A0

enum State {IDLE, RUNNING, ALARM};
State state = IDLE;
float targetTime = 0;
float elapsedTime = 0;

void setup() {                
  // Serial setup
  Serial.begin(9600);
  Serial.println("Setup");

  // led + buzzer init
  for(int i = 0; i <= 12; i++)
    pinMode(i, OUTPUT);

  // input init
  pinMode(POTI,INPUT);
  pinMode(TA,INPUT);
  digitalWrite(TA,HIGH); //pull-up for TA
}




void loop() {
  

  switch(state)
  {
    case IDLE:  state_idle();
                setLeds();
                break;
    case RUNNING: break;
    case ALARM: break;
    default: state = IDLE;
             break;
  } 
  //delay(500);
}

// get target time and check for button
void state_idle()
{
  uint input = analogRead(POTI);
  input < 1 ? input = 1 : true;
  targetTime = input / 170.5;
  //Serial.println(input);
  //Serial.println(targetTime);

  bool ta = digitalRead(TA);
  //Serial.println(ta);
  //while(ta){};
  //if(ta)
    //state = RUNNING;
}

// update LED display
void setLeds()
{
  // reset current LEDs to off
  for(int i = 0; i <= 11; i++)
    digitalWrite(i, LOW);
    
  // display target time
  int tled = targetTime / 0.5;
  tled > 11 ? tled = 11 : (tled < 0 ? tled = 0 : tled = tled);
  digitalWrite(tled, HIGH);

  // display elapsed time
  int eled = elapsedTime / 0.5;
  eled > 11 ? eled = 11 : (eled < 0 ? eled = 0 : eled = eled);
  for(int i = 0; i < eled; i++)
    digitalWrite(i, HIGH);
}


/*for(int i = 0; i <= 12; i++)
  {
    digitalWrite(i, HIGH);
    delay(analogRead(A0));
    Serial.println(analogRead(A0));
    Serial.println(digitalRead(A1));
  }
  for(int i = 0; i <= 12; i++)
  {
    digitalWrite(i, LOW);
    delay(analogRead(A0));
    Serial.println(analogRead(A0));
    Serial.println(digitalRead(A1));
  }*/
