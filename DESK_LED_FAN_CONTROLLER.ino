// Which pins points to the buttons, order of Red Green Blue
const int btnsRGB[3] = {4, 2, 7};

// Which pins points to the leds, order of Red Green Blue
const int ledsRGB[3] = {10, 9, 11};

// Button & variable to control wether to subtract or add to valueRGB
const int btnMode = 12;
bool modeAdd = true;

// To save current strength to display on RGB led
int valueRGB[3] = {0, 0, 0};

void setup() {
  Serial.begin(9600);

  // Initialize buttons
  for(int i = 0; i < 2; i++){
    pinMode(btnsRGB[i], INPUT);
    analogWrite(btnsRGB[i], 0);
  }

  // Initialize led outputs
  for(int i = 0; i < 2; i++){
    pinMode(ledsRGB[i], INPUT);
  }

  // Initialize mode button
  pinMode(btnMode, INPUT);
}

void loop() {
    // If red button was pressed
    if(digitalRead(btnsRGB[0]) == HIGH){
      updateValue(0);
  }
  
  // If green button was pressed
  if(digitalRead(btnsRGB[1]) == HIGH){
    updateValue(1);
  }

  // If blue button was pressed
    if(digitalRead(btnsRGB[2]) == HIGH){
      updateValue(2);
  }

  // If switch mode button was pressed
  if(digitalRead(btnMode) == HIGH){
    if(modeAdd == true){
      modeAdd = false;
    }else{
      modeAdd = true;
    }

    // Ugly way to stop button from unintentional press
    delay(1000);
  }

  // no chill
  delay(30);
}


// Update valueRGB and write change
void updateValue(int colour){

  // Add or subtract depending on mode
  if(modeAdd == true){

    // Don't let the value be more than 255
    if(valueRGB[colour] >= 255){
      valueRGB[colour] = 255;
    } else {
      valueRGB[colour]++;
    }
    }else{

      // Don't let the value be less than 0
      if(valueRGB[colour] <= 0){
        valueRGB[colour] = 0;
      } else {
        valueRGB[colour]--;
      }
    }

    // Actually update the value of corresponding pin
    analogWrite(ledsRGB[colour], valueRGB[colour]);
    Serial.print (ledsRGB[colour], valueRGB[colour]);
}
