// PLAYER 1
const int player1white = 2;
const int player1button = 8;
const int player1red = 12;

// PLAYER 2
const int player2white = 4;
const int player2button = 7;
const int player2red = 13;

bool gameStart = true; // Countdown
bool gameActive = true; //State of round, true if round being played
bool pauseActive = false; // true, LEDs off and Score-Display stays on

int winner = 0;
int color; // 1 = white, 2 = red

unsigned long redTime = 0; // timepoint for when red LED goes on
unsigned long winnerPause = 0; // timepoint for when winner of round is being shown

// SCORE-Display
// Segment-Order: A B C D E F G
const bool numbers[10][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}  // 9
};

const int segmentPins[7] = {A0, A1, A2, A3, A4, A5, 11};

const int digit1 = 6;
const int digit4 = 5;

int score1 = 0;
int score2 = 0;

unsigned long waitDuration = 0;
unsigned long waitStart = 0;

void showNumber(int number) {
  for (int i = 0; i < 7; i = i + 1) {
    digitalWrite(segmentPins[i], numbers[number][i]);
  }
}

void activateLEDs(){
  if (color == 1){
    digitalWrite(player1white, HIGH);
    digitalWrite(player2white, HIGH);
  }
  else if (color == 2){
    digitalWrite(player1red, HIGH);
    digitalWrite(player2red, HIGH);
  }
}

void deactivateLEDs(){
  digitalWrite(player1white, LOW);
  digitalWrite(player1red, LOW);
  digitalWrite(player2white, LOW);
  digitalWrite(player2red, LOW);
}

void updateScoreDisplay() {
  // --- SCORE (Multiplexing) ---
  //Player1-Display
  showNumber(score1);
  digitalWrite(digit1, LOW);
  delay(5);
  digitalWrite(digit1, HIGH);

  //Player2-Display
  showNumber(score2);
  digitalWrite(digit4, LOW);
  delay(5);
  digitalWrite(digit4, HIGH);
}

void setup() {

  randomSeed(analogRead(A7)); //avoids always starting the game with red
  color = random(1,3);
  pinMode(player1white, OUTPUT);
  pinMode(player1button, INPUT_PULLUP);
  pinMode(player1red, OUTPUT);
  pinMode(player2white, OUTPUT);
  pinMode(player2button, INPUT_PULLUP);
  pinMode(player2red, OUTPUT);

  for (int i = 0; i < 7; i++) {
  pinMode(segmentPins[i], OUTPUT);
  }

  pinMode(digit1, OUTPUT);
  pinMode(digit4, OUTPUT);
}

void loop() {

  if (gameStart == true){
    // Countdown
    showNumber(5);
    delay(1000);
    showNumber(4);
    delay(1000);
    showNumber(3);
    delay(1000);
    showNumber(2);
    delay(1000);
    showNumber(1);
    delay(1000);
    showNumber(0);
    delay(random(2000, 5000)); // random delay before first round starts
    
    gameStart = false;
  }

  
  
  updateScoreDisplay();
  
  // --- Round Phase: Active Play---
  if (gameActive) {
    activateLEDs();
  } 

    // White LED logic
    if(color==1){ 
      if (digitalRead(player1button) == LOW) {
        winner = 1;
        gameActive = false;
      }

      if (digitalRead(player2button) == LOW) {
        winner = 2;
        gameActive = false;
      }
    }
    
    // Red LED logic
    if(color==2){ //red
      if(redTime == 0) { 
        redTime = millis(); 
        } 
      if (digitalRead(player1button) == LOW) {
        winner = 2;
        gameActive = false;
      }
      if (digitalRead(player2button) == LOW) {
        winner = 1;
        gameActive = false;
      }


      // After 2 seconds in red mode, if no button pressed, turn off red LEDs.
      if (gameActive && (millis() - redTime >= 2000)) {
        digitalWrite(player1red, LOW);
        digitalWrite(player2red, LOW);
        gameActive = false;
      }
    }

  // --- Round End Phase ---
  if (gameActive == false)  {
    // Phase 1: Announce round winner and update score
    if (winnerPause == 0) {
      if (winner == 1) {
        digitalWrite(player2white, LOW);
        digitalWrite(player2red, LOW);
        score1++;  
      } 
      if (winner == 2) {
        digitalWrite(player1white, LOW);
        digitalWrite(player1red, LOW);
        score2++;  
      }
      winnerPause = millis();  
    }
    
    
    // Phase 2: Winner-LED stays on for 1 second
    if (!pauseActive && millis() - winnerPause >= 1000) {
      deactivateLEDs();
      pauseActive = true;  
    }

    // --- Phase 3: Game Over Check ---
    if (score1 >= 5 || score2 >= 5) {
      deactivateLEDs();

      static unsigned long blinkTimer = 0;
      static bool ledState = false;

      while (true) {
        updateScoreDisplay();

        if (millis() - blinkTimer >= 300) {
          blinkTimer = millis();
          ledState = !ledState;

          if (score1 >= 5) {
            digitalWrite(player1white, ledState ? HIGH : LOW);
          } 
          if (score2 >= 5) {
            digitalWrite(player2white, ledState ? HIGH : LOW);
          }
        }
      }
    }

    // Phase 4: Round Reset: Wait for random time (with Display on), then reset round state
    if (pauseActive == true) {
      if (waitStart == 0){
        waitStart = millis();
        waitDuration = random(2000, 5000);
      }

      updateScoreDisplay();
      
      if (millis() - waitStart >= waitDuration) {
        // Reset Round states
        winner = 0;
        gameActive = true;
        color = random(1,3);
        redTime = 0;
        winnerPause = 0;      
        pauseActive = false;
        waitStart = 0;
        waitDuration = 0;
      }       
    }
  }
}
