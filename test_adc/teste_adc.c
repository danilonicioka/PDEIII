#define MQ6 A0
#define NO_OF_SAMPLES   64          //Multisampling

uint32_t raw(){
  uint32_t adc_reading = 0;
  //Multisampling
  for (int i = 0; i < NO_OF_SAMPLES; i++) {
      adc_reading += analogRead(MQ6);
  }
  adc_reading /= NO_OF_SAMPLES;
  return adc_reading;
}
