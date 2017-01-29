#define TA A1
#define POTI A0
#define ISR_CD 300    // Block interrupts for this amount of time in ms.
#define TIME_MULTI 20  // Accelerates the countdown. Set to 1 in production.

enum State {IDLE, RUNNING, ALARM};
State state = IDLE;
int targetTime = 0; // ms
int elapsedTime = 0;  //ms
volatile bool TAtriggered = false;

void setup() {                
  // Serial setup
  Serial.begin(9600);
  Serial.println("Setup");

  // led + buzzer init
  for(int i = 0; i <= 12; i++)
    pinMode(i, OUTPUT);

  // input init
  pinMode(POTI,INPUT);
  pinMode(TA,INPUT_PULLUP);
  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(TA), pin_ISR, RISING);
}




void loop() {
  switch(state)
  {
    
    case IDLE:  state_idle();
                setLeds();
                break;
    case RUNNING: countTime();
                  setLeds();
                  break;
    case ALARM: break;
    default: state = IDLE;
             break;
  } 
}

// get target time and check for button
void state_idle()
{
  uint input = analogRead(POTI);
  input < 0 ? input = 0 : true;
  targetTime = 30000 + 351*input;  // one step are 30s in ms

  // round time to 30s chunks
  targetTime /= 30000;
  targetTime *= 30000;
  
  if(TAtriggered)
  {
    state = RUNNING;
    TAtriggered = false;
  };  
}

// update LED display
void setLeds()
{
  // reset current LEDs to off
  for(int i = 0; i <= 11; i++)
    digitalWrite(i, LOW);
    
  // display target time
  int tled = targetTime / 30000 - 1;  // ms to half-secs
  tled > 11 ? tled = 11 : (tled < 0 ? tled = 0 : tled = tled);
  digitalWrite(tled, HIGH);

  // display elapsed time
  int eled = elapsedTime / 30000;
  eled > 11 ? eled = 11 : (eled < 0 ? eled = 0 : eled = eled);
  for(int i = 0; i < eled; i++)
    digitalWrite(i, HIGH);
}

// count up to targeted time
void countTime()
{
  if(elapsedTime < targetTime)
   {
    elapsedTime += 200;
    Serial.println(elapsedTime);
    delay(200 / TIME_MULTI);
   } else {
      state = ALARM;
   }
   
   if(TAtriggered)
   {
      elapsedTime = 0;
      TAtriggered = false;
      state = IDLE;
   }
}

// interrupt for button pressed.
void pin_ISR()
{
  static unsigned long lastTimeTriggered = 0;
  // Debounce and also trigger if millis had an overflow
  if((millis() - lastTimeTriggered > ISR_CD) || millis() < lastTimeTriggered)
  {
    Serial.println("ISR");
    TAtriggered = true;
    lastTimeTriggered = millis();
  }
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
