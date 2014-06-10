/**
 * pxar HV power supply implementation for Keithley 2410
 */

#include "hvsupply.h"
#include "rs232.h"
#include "exceptions.h"
#include "helper.h"
#include "log.h"
#include <cstdlib>
#include <cmath>
#include <iostream>

using namespace pxar;

// ----------------------------------------------------------------------
hvsupply::hvsupply() {
  const int comPortNumber = 16; /* /dev/ttyUSB0 */
  if(!openComPort(comPortNumber,57600)) {
    LOG(logCRITICAL) << "Error connecting via RS232 port!";
    throw UsbConnectionError("Error connecting via RS232 port!");
  }
  LOG(logDEBUG) << "Opened COM port to Keithley 2410";


  writeCommandString("*RST\n");
  writeCommandString(":SYST:BEEP:STAT OFF\n");
  writeCommandString(":ROUT:TERM REAR\n");
  writeCommandString(":SOUR:VOLT:MODE FIX\n");
  //writeCommandString(":FORM:ELEM VOLT,CURR\n");
  writeCommandString(":FUNC:CONC ON\n");
  writeCommandString(":SENS:AVER:TCON REP\n");
  writeCommandString(":SENS:AVER:COUNT 2\n");
  writeCommandString(":SENS:AVER:STAT ON\n");
  writeCommandString(":CURR:PROT:LEV 100E-6\n");
  writeCommandString(":SENS:CURR:RANG 20E-6\n");
  writeCommandString(":SENS:CURR:NPLC 10\n");
  writeCommandString("SOUR:VOLT:IMM:AMPL -100\n");
  //writeCommandString(":FORM:ELEM STAT\n");
  writeCommandString(":FORM:ELEM VOLT,CURR\n");


  //  writeCommandString(":SENS:FUNC:ON \"CURR\"");
  //  writeCommandString(":ARM:SOUR TIM");
  //  writeCommandString(":ARM:TIM 0.001");
  //  writeCommandString(":ARM:COUN INF");  
  //  writeCommandString(":SENS:CURR:NPLC 0.01");
  //  writeCommandString("DISP:DIG 7");

}

// Destructor: Will turn off the HV and terminate connection to the HV Power Supply device.
hvsupply::~hvsupply() {
  LOG(logDEBUG) << "Turning Power Supply OFF";
  writeCommandString("OUTPUT 0");
  char answer[256] = { 0 };
  writeCommandStringAndReadAnswer(":OUTP:STAT?",answer);
  LOG(logDEBUG) <<"State of Keithley after shut down: " <<  answer;

  // Switch back to local mode:
  writeCommandString(":SYST:LOC");
  closeComPort();
}

// ----------------------------------------------------------------------
bool hvsupply::hvOn() {
  LOG(logDEBUG) << "Turning KEITHLEY 2410 on";
  char answer[256] = { 0 };  
  writeCommandString("OUTPUT 1");
  writeCommandString(":INIT");
  writeCommandStringAndReadAnswer(":OUTP:STAT?",answer);
  LOG(logDEBUG) <<"State of Keithley: " <<  answer;
  int a; 
  sscanf(answer, "%d", &a); 
  if (0 == a) {
    return false;
  }
  return true;
}

    
// ----------------------------------------------------------------------
bool hvsupply::hvOff() {
  char answer[1000] = {0};  
  LOG(logDEBUG) << "Turning Keithley 2410 off";
  writeCommandString("OUTPUT 0");
  writeCommandStringAndReadAnswer(":OUTP:STAT?",answer);
  LOG(logDEBUG) <<"State of Keithley: " <<  answer;
  int a; 
  sscanf(answer, "%d", &a); 
  if (1 == a) {
    return false;
  }
  return true;
}


    
// ----------------------------------------------------------------------
bool hvsupply::setVoltage(double volts) {
  char string[100];
  if (volts < 0.) volts = -1.*volts;
  sprintf(string, "SOUR:VOLT:IMM:AMPL -%i", static_cast<int>(volts));
  writeCommandString(string);
  sleep(1); 
  return false;
}


// ----------------------------------------------------------------------
bool hvsupply::tripped() {
  char answer[1000] = {0};  
  writeCommandStringAndReadAnswer(":SENS:CURR:PROT:TRIP?", answer);
  int a; 
  sscanf(answer, "%d", &a); 
  if (1 == a) {
    return true;
  }
  return false;
}
    
// ----------------------------------------------------------------------
double hvsupply::getVoltage() {
  float voltage(0), current(0);
  char answer[1000] = {0};  

  //  std::cout << "Measuring Voltage" << std::endl;
  writeCommandStringAndReadAnswer(":READ?", answer);
  //  std::cout << "Raw answer: "  << answer << std::endl;
  sscanf(answer, "%e,%e", &voltage, &current);
  return voltage;
  
//   std::cout << "Measuring Voltage" << std::endl;
//   writeCommandString(":ABOR");
//   sleep(1); 

//   writeCommandString(":ARM:COUN 40");
//   writeCommandString(":MEAS:CURR?");

//   sleep(1);
//   writeCommandStringAndReadAnswer(":FETC?",answer);
//   std::cout << "Raw answer: "  << answer << std::endl;
//   sscanf(answer, "%e", &voltage);

//   std::cout << "Voltage is: " << voltage  <<  std::endl;
//   writeCommandString(":TRIG:CLE");

//   return voltage;
}

// ----------------------------------------------------------------------
double hvsupply::getCurrent() {

  float current(0), voltage(0);
  char answer[1000] = {0};  

  //  std::cout << "Measuring Current" << std::endl;
  writeCommandStringAndReadAnswer(":READ?", answer);
  //  std::cout << "Raw answer: "  << answer << std::endl;
  sscanf(answer, "%e,%e", &voltage, &current);
  return current;

//   writeCommandString(":ARM:COUN 40");
//   writeCommandString(":MEAS:CURR?");

//   sleep(1);
//   writeCommandStringAndReadAnswer(":FETC?",answer);
//   std::cout << "Raw answer: "  << answer << std::endl;
//   sscanf(answer, "%e", &current);

//   std::cout << "Current is: " << current << std::endl;
//   writeCommandString(":INIT");
//   return current;
}

// ----------------------------------------------------------------------
bool hvsupply::setCurrentLimit(uint32_t /*microampere*/) {
  return false;
}

// ----------------------------------------------------------------------
double hvsupply::getCurrentLimit() {
  return 2000;
}
