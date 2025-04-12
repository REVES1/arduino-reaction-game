// PLAYER 1
const int player1white = 2;
const int player1button = 8;
const int player1red = 12;


// PLAYER 2
const int player2white = 4;
const int player2button = 7;
const int player2red = 13;


bool SpielAktiv = true;

bool pauseAktiv = false;

int Gewinner = 0;

int farbe;

unsigned long redTime = 0;

unsigned long winnerPause = 0;

// SCORE-Anzeige
// Segment-Reihenfolge: A B C D E F G
const bool zahlen[10][7] = {
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

// Segmente werden Pins zugewiesen
const int segmentPins[7] = {A0, A1, A2, A3, A4, A5, 11};

// Digit-Zuordnung
const int digit1 = 6;
const int digit4 = 5;

int score1 = 0;
int score2 = 0;


void zeigeZahl(int zahl) {
  for (int i = 0; i < 7; i = i + 1) {
    digitalWrite(segmentPins[i], zahlen[zahl][i]);
  }
}



void aktiviereLEDs(){
  if (farbe == 1){
    digitalWrite(player1white, HIGH);
    digitalWrite(player2white, HIGH);
  }
  else if (farbe == 2){
    digitalWrite(player1red, HIGH);
    digitalWrite(player2red, HIGH);
  }
}

void deaktiviereLEDs(){
  digitalWrite(player1white, LOW);
  digitalWrite(player1red, LOW);
  digitalWrite(player2white, LOW);
  digitalWrite(player2red, LOW);
}

void setup() {
  farbe = random(1,3);
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

// bis hierhin ist alles ok lol

void loop() {

  //SCORE (Multiplexing)
  //Player1
  zeigeZahl(score1);
  digitalWrite(digit1, LOW);
  delay(5);
  digitalWrite(digit1, HIGH);

  //Player2
  zeigeZahl(score2);
  digitalWrite(digit4, LOW);
  delay(5);
  digitalWrite(digit4, HIGH);




  if (SpielAktiv) {
    aktiviereLEDs();
  }
    if(farbe==1){ //weiß
      if (digitalRead(player1button) == LOW) {
        Gewinner = 1;
        SpielAktiv = false;
      }

      if (digitalRead(player2button) == LOW) {
        Gewinner = 2;
        SpielAktiv = false;
      }
    }
    
    if(farbe==2){ //rot
      if(redTime == 0) { 
        redTime = millis(); 
        } 

      
      // Alle Button-Abfragen in einem Block:
      if (digitalRead(player1button) == LOW) {
        Gewinner = 2;
        SpielAktiv = false;
      }
      if (digitalRead(player2button) == LOW) {
        Gewinner = 1;
        SpielAktiv = false;
      }


    
      if (SpielAktiv && (millis() - redTime >= 2000)) {
        digitalWrite(player1red, LOW);
        digitalWrite(player2red, LOW);
        SpielAktiv = false;
      }
    }


  if (SpielAktiv == false)  {
    // Phase 1: Gewinner festlegen und Score erhöhen (wird nur einmal ausgeführt)
    if (winnerPause == 0) {
      if (Gewinner == 1) {
        digitalWrite(player2white, LOW);
        digitalWrite(player2red, LOW);
        score1++;  // Erhöhe Score von Spieler 1
      } 
      if (Gewinner == 2) {
        digitalWrite(player1white, LOW);
        digitalWrite(player1red, LOW);
        score2++;  // Erhöhe Score von Spieler 2
      }
      winnerPause = millis();  // Speichere den Startzeitpunkt dieser Phase
    }
    
    // Phase 2: Gewinnerlicht bleibt 1 Sekunde an, bevor alles ausgeschaltet wird
    if (!pauseAktiv && millis() - winnerPause >= 1000) {
      deaktiviereLEDs();
      pauseAktiv = true;  // Gehe in die Pause-Phase
    }
    
    // Phase 3: Zufällige Wartezeit (während die Punktanzeige aktiv bleibt)
    if (pauseAktiv == true) {
      unsigned long warteStart = millis();
      unsigned long warteDauer = random(2000, 5000);
      while (millis() - warteStart < warteDauer) {
        // Zeige fortlaufend die Punktanzeige
        zeigeZahl(score1);
        digitalWrite(digit1, LOW);
        delay(5);
        digitalWrite(digit1, HIGH);
        
        zeigeZahl(score2);
        digitalWrite(digit4, LOW);
        delay(5);
        digitalWrite(digit4, HIGH);
      }
      // Nach der Wartezeit: alles zurücksetzen
      Gewinner = 0;
      SpielAktiv = true;
      farbe = random(1,3);
      redTime = 0;
      winnerPause = 0;      // Setze auch den Gewinner-Timer zurück
      pauseAktiv = false;   // Setze den Pause-Zustand zurück für die nächste Runde
    }
  }
}



