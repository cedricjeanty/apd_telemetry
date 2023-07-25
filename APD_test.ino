#include "APD_telemetry.h"

APD_Telem APD1(36); //36 poles

void setup() {
  Serial.begin(115200); //this is the USB serial
  Serial1.begin(115200); //this is the motor controller telemetry serial

}

void loop() {
  int ret = 0;
  while(Serial1.available()){
    ret = APD1.update(Serial1.read());
    if (ret == 1){
      break;
    }
  }
  
  if (ret == 1){
    Serial.print(" volts: ");
    Serial.print(APD1.volts);
    Serial.print(" amps: ");
    Serial.print(APD1.amps);
    Serial.print(" tempC: ");
    Serial.print(APD1.tempC);
    Serial.print(" rpm: ");
    Serial.print(APD1.rpm);
    Serial.println();
  } else {
    Serial.println(ret);
  }
  delay(50);
}
