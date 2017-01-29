#define BTN A1
#define POTI A0
#define BUZZER 12
#define ALARM_ON_DUR 250   // time in ms, resolution 50ms
#define ALARM_OFF_DUR 2000   // time in ms, resolution 50ms
#define ISR_CD 300    // Block interrupts for this amount of time in ms.
#define TIME_MULTI 1  // Accelerates the countdown. Set to 1 in production.

enum State {IDLE, RUNNING, ALARM};
State state = IDLE;
int targetTime = 0; // ms
int elapsedTime = 0;  //ms
volatile bool BTNtriggered = false;   // ISR changes this when button is pressed.

void setup() {                
  // Serial setup
  Serial.begin(9600);

  // led + buzzer init
  for(int i = 0; i <= 12; i++)
    pinMode(i, OUTPUT);

  // input init
  pinMode(POTI,INPUT);
  pinMode(BTN,INPUT_PULLUP);
  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(BTN), BTN_ISR, RISING);
}




void loop() {
  switch(state)
  {
    
    case IDLE:  state_idle();
                setLeds();
                break;
    case RUNNING: state_running();
                  setLeds();
                  break;
    case ALARM: state_alarm();
                setLeds();
                break;
    default: state = IDLE;
             break;
  } 
}

// get target time and check for button
void state_idle()
{
  elapsedTime = 0;
  uint input = analogRead(POTI);
  input < 0 ? input = 0 : input;
  targetTime = 30000 + 351*input;  // one step are 30s in ms

  // round time to 30s chunks
  targetTime /= 30000;
  targetTime *= 30000;
  
  if(BTNtriggered)
  {
    state = RUNNING;
    BTNtriggered = false;
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
void state_running()
{
  if(elapsedTime < targetTime)
   {
    elapsedTime += 100;
    Serial.println(elapsedTime/1000);
    delay(100 / TIME_MULTI);
   } else {
      state = ALARM;
   }
   
   if(BTNtriggered)
   {
      elapsedTime = 0;
      BTNtriggered = false;
      state = IDLE;
   }
}

// Do alarm-state stuff: Ctrl buzzer and wait for BTN
void state_alarm()
{
  static unsigned int timecnt = 0;
  static bool cntup = true;
  if(BTNtriggered)
  {
    state = IDLE;
    timecnt = 0;
    cntup = true;
    BTNtriggered = false;
      // reset buzzer
    digitalWrite(BUZZER, LOW);
    return;
  }

  // Count up to ALARM_ON_DUR
  if(timecnt <= ALARM_ON_DUR && cntup)
  {
    digitalWrite(BUZZER, HIGH);
    delay(50);
    if(timecnt >= ALARM_ON_DUR)
    {
      timecnt = 0;
      cntup = false;
    }
    else
    {
      timecnt += 50;
    }
  }
  
  // Count up to ALARM_OFF_DUR
  if(timecnt <= ALARM_OFF_DUR && !cntup)
  {
    digitalWrite(BUZZER, LOW);
    delay(50);
    if(timecnt >= ALARM_OFF_DUR)
    {
      timecnt = 0;
      cntup = true;
    } 
    else
    {
      timecnt += 50;
    }
  }
  
}

// interrupt for button pressed.
void BTN_ISR()
{
  static unsigned long lastTimeTriggered = 0;
  // Debounce and also trigger if millis had an overflow
  if((millis() - lastTimeTriggered > ISR_CD) || millis() < lastTimeTriggered)
  {
    Serial.println("ISR triggered");
    BTNtriggered = true;
    lastTimeTriggered = millis();
  }
}

