// MQ6
#define VCC     5
#define MQ6    A3
#define RL  20000
#define CALIBRATION_SAMPLES               100.0 // Multisampling
#define CALIBRATION_SAMPLE_INTERVAL       100   // Milisseconds
#define HEAT_TIME_MQ6                   20000   // Milisseconds
#define Rs_Ro_clean_air 10.0

void setup() {
  pinMode(MQ6,INPUT);
  
  Serial.begin(9600);
  Serial.print(F("MQ6 is heating..."));
  delay(HEAT_TIME_MQ6);
  Serial.println(F("\tdone!\n"));
  Serial.println(F("=== Reading MQ6 ==="));
//  delay(1000);  // Passing unstable DHT condition
//  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t  sum         = 0;
  float     sensorValue = 0;
  float     Vo          = 0;
  float     Rs          = 0;
  float     Ro          = 0;
  static int contador   = 1;
  
  //Multisampling
  for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
      sum += analogRead(MQ6);
      delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  
  sensorValue = (float)sum/CALIBRATION_SAMPLES;
  Vo  = 5*(sensorValue)/1023.0;
  Rs  = (((float)(VCC - Vo)/Vo)*RL);
  Ro  = Rs / Rs_Ro_clean_air;

  Serial.print(F("["));
  Serial.print(contador);
  Serial.print(F("]"));
  Serial.print(F("  Raw: "));
  Serial.print(sensorValue);
  Serial.print(F("  Voltage: "));
  Serial.print(Vo);
  Serial.print(F("V")); 
  Serial.print(F("  Rs: "));
  Serial.println(Rs);
  contador++;
}
