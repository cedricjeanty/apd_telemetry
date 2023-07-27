
/**
 * APD Telemetry Class
 * 20230509
 * Cedric Jeanty
 */

#ifndef APD_TELEM_H
#define APD_TELEM_H

#include <CircularBuffer.h>

#define APD_PKT_LEN 22


const float SERIESRESISTOR = 10000;
const float NOMINAL_RESISTANCE = 10000;
const float NOMINAL_TEMPERATURE = 25;
const float BCOEFFICIENT = 3455;
const float BITS_PER_AMP = 12.5;

/*Data buf format
0: Voltage Low 
1: Voltage High
2: Temperature Low 
3: Temperature High
4: Bus Current Low
5: Bus Current High
6: Reserved (Not Used)
7: Reserved (Not Used)
8: ERPM 1
9: ERPM 2
10: ERPM 3
11: ERPM 4
12: Throttle Duty Low
13: Throttle Duty High
14: Motor Duty Low
15: Motor Duty High
16: Status Flags
17: Reserved (Not Used)
18: Fletcher Checksum Low
19: Fletcher Checksum High
20: Stop Byte Low
21: Stop Byte High*/

// Voltage
const uint8_t APD_V_LO = 0;
const uint8_t APD_V_HI = 1;

// Temperature
const uint8_t APD_T_LO = 2;
const uint8_t APD_T_HI = 3;

// Current
const uint8_t APD_I_LO = 4;
const uint8_t APD_I_HI = 5;

// Reserved0
const uint8_t APD_R0_LO = 6;
const uint8_t APD_R0_HI = 7;

// eRPM
const uint8_t APD_RPM0 = 8;
const uint8_t APD_RPM1 = 9;
const uint8_t APD_RPM2 = 10;
const uint8_t APD_RPM3 = 11;

// Input Duty
const uint8_t APD_DUTYIN_LO = 12;
const uint8_t APD_DUTYIN_HI = 13;

// Motor Duty
const uint8_t APD_MOTORDUTY_LO = 14;
const uint8_t APD_MOTORDUTY_HI = 15;

// Reserved1
const uint8_t APD_R1 = 16;

// Status
const uint8_t APD_FLAGS = 17;
// checksum
const uint8_t APD_CSUM_LO = 18;
const uint8_t APD_CSUM_HI = 19;

//Stop bytes
const uint8_t APD_STOP1 = 20;
const uint8_t APD_STOP2 = 21;


/* Status Flags
# Bit position in byte indicates flag set, 1 is set, 0 is default
# Bit 0: Motor Started, set when motor is running as expected
# Bit 1: Motor Saturation Event, set when saturation detected and power is reduced for desync protection
# Bit 2: ESC Over temperature event occuring, shut down method as per configuration
# Bit 3: ESC Overvoltage event occuring, shut down method as per configuration
# Bit 4: ESC Undervoltage event occuring, shut down method as per configuration
# Bit 5: Startup error detected, motor stall detected upon trying to start*/
const uint8_t MOTOR_STARTED_BIT = 0;
const uint8_t MOTOR_SATURATED_BIT = 1;
const uint8_t ESC_OVER_TEMP_BIT = 2;
const uint8_t ESC_OVER_VOLT_BIT = 3;
const uint8_t ESC_UNDER_VOLT_BIT = 4;
const uint8_t MOTOR_START_ERR_BIT = 5;


class APD_Telem {

  public:
    uint8_t num_poles = 1;

    float volts = 0.0;
    float tempC = 0.0;
    float amps = 0.0;
    long rpm = 0;
    float throttle = 0.0;
    float phase_amps = 0.0;
    uint32_t update_ts = 0;
    byte status = 0;

    APD_Telem(uint8_t _poles);
    int update(byte _byte);
    bool is_started(void);
    bool is_saturated(void);
    bool is_overtemp(void);
    bool is_overvolt(void);
    bool is_undervolt(void);
    bool start_err(void);

  private:

    CircularBuffer<byte, APD_PKT_LEN> apd_q;

    union _pkt {
      struct {
        uint16_t _volts;
        uint16_t _tempC;
        int16_t _amps;
        uint16_t _res0;
        uint32_t _erpm;
        uint16_t _throttle;
        uint16_t _motorduty;
        uint8_t _status;
        uint8_t _res1;
        uint16_t _csum;
        uint16_t _stop;
      };
      byte bytes[APD_PKT_LEN];
    } pkt;

    int parse(void);
    int CheckFlectcher16(void);
};


#endif
