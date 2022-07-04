#define POT 15
#define NO_OF_SAMPLES   64          //Multisampling

void setup() {
  // put your setup code here, to run once:
  pinMode(POT,INPUT);
  Serial.begin(115200);
  Serial.println(F("=== ADC TEST ==="));
}

void loop() {
  // put your main code here, to run repeatedly:

  uint32_t raw = 0;

  //Multisampling
  for (int i = 0; i < NO_OF_SAMPLES; i++) {
      raw += analogRead(POT);
  }
  raw /= NO_OF_SAMPLES;
  
  float voltage = 3.3*raw/4095.0;

  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print("\tVoltage: ");
  Serial.println(voltage);
  delay(1500);
}
