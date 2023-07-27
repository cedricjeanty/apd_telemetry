#include "Arduino.h"
#include "APD_telemetry.h"


APD_Telem::APD_Telem(uint8_t _poles = 1) {
  num_poles = _poles;
}

int APD_Telem::update(byte _byte){
  apd_q.push(_byte);
  if (apd_q.isFull()){
    //check if the stop bytes have been rxed
    if (apd_q[APD_STOP1] == 0xFF && apd_q[APD_STOP2] == 0xFF){
      return parse();
    }
  }
  return 0;
}

int APD_Telem::parse(void){

  uint32_t _update_ts = millis();

  //copy the data from the q to the packet structure
  for(uint8_t i = 0; i < APD_PKT_LEN; i++){
    pkt.bytes[i] = apd_q[i];
  }

  //Check the fletcher checksum
  if (pkt._csum != CheckFlectcher16()) {
    return -1; //Checksums do not match
  }
  apd_q.clear(); //the data is good, so clear out the queue

  // Voltage
  volts = (float)pkt._volts/100;

  // Temperature
  //convert value to resistance
  float Rntc = SERIESRESISTOR / ((4096 / (float)pkt._tempC) - 1);
  float temperature = log(Rntc / NOMINAL_RESISTANCE)/BCOEFFICIENT; // 1/B * ln(R/Ro)
  temperature += 1.0 / (NOMINAL_TEMPERATURE + 273.15);       // + (1/To)
  temperature = 1.0 / temperature;                                         // Invert
  temperature -= 273.15;                                                   // convert to Celcius
  tempC = (float)constrain(temperature, 0, 200);

  // Current
  amps = (float)pkt._amps / BITS_PER_AMP;

  // RPM
  rpm = pkt._erpm / num_poles;

  // Throttle
  throttle = (float)pkt._throttle / 10000;
  
  // Motor Duty
  if (pkt._motorduty > 0){
    phase_amps = amps / ((float)pkt._motorduty / 10000.0);
  } else {
    phase_amps = 0.0;
  }

  status = pkt._status;

  update_ts = _update_ts;

  return 1;
}

bool APD_Telem::is_started(void){
  return status & (1 << MOTOR_STARTED_BIT);
}

bool APD_Telem::is_saturated(void){
  return status & (1 << MOTOR_SATURATED_BIT);
}

bool APD_Telem::is_overtemp(void){
  return status & (1 << ESC_OVER_TEMP_BIT);
}

bool APD_Telem::is_overvolt(void){
  return status & (1 << ESC_OVER_VOLT_BIT);
}

bool APD_Telem::is_undervolt(void){
  return status & (1 << ESC_UNDER_VOLT_BIT);
}

bool APD_Telem::start_err(void){
  return status & (1 << MOTOR_START_ERR_BIT);
}

int APD_Telem::CheckFlectcher16(void) {
    uint16_t fCCRC16;
    uint16_t c0 = 0;
    uint16_t c1 = 0;

    // Calculate checksum intermediate bytesUInt16
    for (uint8_t i = 0; i < 18; i++) //Check only first 18 bytes, skip crc bytes
    {
        c0 = (uint16_t)(c0 + ((uint8_t)apd_q[i])) % 255;
        c1 = (uint16_t)(c1 + c0) % 255;
    }
    // Assemble the 16-bit checksum value
    fCCRC16 = ( c1 << 8 ) | c0;
    return fCCRC16;
}
