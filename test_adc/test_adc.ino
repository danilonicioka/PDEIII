#define MQ6 A0
#define NO_OF_SAMPLES   64          //Multisampling

void setup() {
  Serial.begin(9600);
  pinMode(MQ6,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  float adc_reading = 0;
  float voltage     = 0;
  //Multisampling
  for (int i = 0; i < NO_OF_SAMPLES; i++) {
      adc_reading += analogRead(MQ6);
  }
  adc_reading /= NO_OF_SAMPLES;
  //Convert adc_reading to voltage in mV
  voltage = 5*adc_reading/1024.0;
  Serial.print("=== Reading MQ6 ===\nRaw: ");
  Serial.print(adc_reading);
  Serial.print("\tVoltage: ");
  Serial.println(voltage);
  delay(1500);
}
