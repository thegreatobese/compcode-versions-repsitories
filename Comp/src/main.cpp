// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// topLeft              motor         20              
// backLeft             motor         10              
// topRight             motor         9               
// backRight            motor         1               
// Controller1          controller                    
// leftArm              motor         2               
// rightArm             motor         5               
// ---- END VEXCODE CONFIGURED DEVICES ----
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Adam Xu (adx59), Shafin Haque (ShafinH)                   */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  53343A comp / skills code                                 */
/*                                                                            */
/*----------------------------------------------------------------------------*/
 
#include "vex.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <sstream>
 
using namespace vex;
 
competition Competition;
 
double PI = 3.14159265;
double TURN_MULT = 0.6; // slow down driver turn speed
double INCH_MULT = 28;

int intakeSpeed = 100;
double nja_md = 1;

int MENU_MODE = 0;
int SELECTED_AUTON = 0;
std::string AUTON_CODES[8] = {"straight", "cornerblue", "V3", "-", "-", "-", "S", "CUSTOM"};

double ROUTINE[5][3] = {{0, 100, 0}, {-1, 100, 1}, {-1, 100, 0}, {-1, 100, 0}, {-1, 100, 0}};
int CRT = 0;
std::string CUSTOM_NAME[6] = {"V", "T", "A", "I", "S", "C"}; 

double averageEncoderVal(double des) {
  double avg = (fabs(topLeft.position(deg)) + fabs(topRight.position(deg)) + fabs(backLeft.position(deg)) + fabs(backRight.position(deg)))/4;
  if (des < 0) avg *= -1;

  return avg;
}

float autonSpeed = 100;
const float GEARRATIO = 4.5;
void moveForward(float inches) {
  float inchesPerDeg = 2*3.14*4/360*1*32/40;
  float degrees = inches/inchesPerDeg*GEARRATIO;
  backRight.spinFor(forward, degrees, deg, autonSpeed, vex::velocityUnits::pct,false);
  topRight.spinFor(forward, degrees, deg, autonSpeed, vex::velocityUnits::pct,false);
  topLeft.spinFor(forward, degrees, deg, autonSpeed, vex::velocityUnits::pct,false);
  backLeft.spinFor(forward, degrees, deg, autonSpeed, vex::velocityUnits::pct,true);
}

void turnFor(float degrees) {
  float wheelDegrees = 11.2/ 4*degrees*GEARRATIO;
  backRight.spinFor(forward, -wheelDegrees, deg, autonSpeed, vex::velocityUnits::pct,false);
  topRight.spinFor(forward, -wheelDegrees, deg, autonSpeed, vex::velocityUnits::pct,false);
  topLeft.spinFor(forward, wheelDegrees, deg, autonSpeed, vex::velocityUnits::pct,false);
  backLeft.spinFor(forward, wheelDegrees, deg, autonSpeed, vex::velocityUnits::pct,true);

}
void spinArm(float degr, float speed) {
    leftArm.spinFor(forward, degr, deg, speed, vex::velocityUnits::pct, false);
    rightArm.spinFor(forward, degr, deg, speed, vex::velocityUnits::pct, true);
}

void selector () {
  // inputs
  if (MENU_MODE == 0) {
    if (Controller1.ButtonRight.pressing() && SELECTED_AUTON < 6) {
      SELECTED_AUTON++;
    } 

    if (Controller1.ButtonLeft.pressing() && SELECTED_AUTON > 0) {
      SELECTED_AUTON--;
    } 

    if (Controller1.ButtonY.pressing()) {
      MENU_MODE = 1;
      SELECTED_AUTON = 7;
      Controller1.rumble("...");
    }
  } else if (MENU_MODE == 1) {
    ROUTINE[CRT][1] += 0.01 * (double) Controller1.Axis3.position(pct);
    ROUTINE[CRT][2] += 0.01 * (double) Controller1.Axis2.position(pct);
    ROUTINE[CRT][1] = round(10 * ROUTINE[CRT][1])/10; ROUTINE[CRT][2] = round(10 * ROUTINE[CRT][2])/10;

    if (Controller1.ButtonRight.pressing() && ROUTINE[CRT][0] < 5) {
      ROUTINE[CRT][0]++;
    }
    if (Controller1.ButtonLeft.pressing() && ROUTINE[CRT][0] > 0) {
      ROUTINE[CRT][0]--;
    }

    if (Controller1.ButtonUp.pressing() && CRT < 4) {
      ROUTINE[CRT+1][0] = ROUTINE[CRT+1][0] == -1 ? 0 : ROUTINE[CRT+1][0];
      CRT++;
    } 
    else if (Controller1.ButtonDown.pressing() && CRT > 0) {
      CRT--;
    }
  }

  if (Controller1.ButtonA.pressing()) {
      MENU_MODE = 2;
      Controller1.rumble(".");
  }
}

void ctrlRender () {
  Controller1.Screen.clearScreen();
  if (MENU_MODE == 0) {
    std::stringstream SelectorText[2];
    SelectorText[0] << "Battery: " << Brain.Battery.capacity() << "\%"; 
    SelectorText[1] << "Auton: " << AUTON_CODES[SELECTED_AUTON] << ""; 

    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print("- SELECT -");
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print(SelectorText[0].str().c_str());
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print(SelectorText[1].str().c_str());
  } else if (MENU_MODE == 1) {
    std::stringstream CustomText[3];
    CustomText[0] << "= Action #" << CRT+1 << " =";
    CustomText[1] << "[" << CUSTOM_NAME[(int) ROUTINE[CRT][0]] << "] / V: " << ROUTINE[CRT][1] << " / D: " << ROUTINE[CRT][2];
    for (int i = 0; i < 5; i++) { CustomText[2] << CUSTOM_NAME[(int) ROUTINE[i][0]] << "; "; } 

    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print(CustomText[0].str().c_str());
    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print(CustomText[1].str().c_str());
    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print(CustomText[2].str().c_str());
  } else if (MENU_MODE == 2) {
    std::stringstream top;
    std::stringstream bottom;
    top << "Temp: " << round(topLeft.temperature(temperatureUnits::fahrenheit));
    bottom << "Auton: " << AUTON_CODES[SELECTED_AUTON];

    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print(top.str().c_str());
    Controller1.Screen.newLine();
    Controller1.Screen.print(bottom.str().c_str());
    Controller1.Screen.newLine();

    if (nja_md == 0.3) {
      Controller1.Screen.setCursor(3, 1);
      Controller1.Screen.print("[NINJA]");
    }
  }
}
  
void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
}
 
void autonomous(void) {
  topLeft.setBrake(brake);
  topRight.setBrake(brake); 
  backLeft.setBrake(brake);
  backRight.setBrake(brake);

  if (SELECTED_AUTON == 7) {
    for (int n = 0; n < 5; n++) {
      if (ROUTINE[n][0] == 0) {
        
      } else if (ROUTINE[n][0] == 1) {
        
      } else if (ROUTINE[n][0] == 2) {
        
      } else if (ROUTINE[n][0] == 3) {
        
      } else if (ROUTINE[n][0] == 4) {
        
      } else if (ROUTINE[n][0] == 5) {
        
      }
    }
    return;
  }

  if (SELECTED_AUTON == 0) {
    spinArm(-680,100);
    moveForward(54);
    spinArm(650,50);
    vex::task::sleep( 500 );
    autonSpeed=100;
    moveForward(-30);
    /*
    mid code
    turnFor(-50);
    autonSpeed=50;
    spinArm(-650,50);
    moveForward(42);
    spinArm(650,50);  
    moveForward(-50);
    */
    // opposite side of platform without goal, grab one neutral goal
  } else if (SELECTED_AUTON == 1) {
    spinArm(-680,100);
    moveForward(60);
    spinArm(650,50);
    vex::task::sleep( 500 );
    autonSpeed=100;
    moveForward(-45);


    //side of platform with goal, grab one neutral goal
  } else if (SELECTED_AUTON == 2) {
    spinArm(-680,100);
  } else if (SELECTED_AUTON == 3) {
    
  } else if (SELECTED_AUTON == 4) {
    // left side, 2 goal
  } else if (SELECTED_AUTON == 5) {

  } else if (SELECTED_AUTON == 6) { 
    /*
    * Skills Auton
    * Four-goal, encoder, some wait-time utilized
    */
  } 
}

int cut = 1;
 
void usercontrol(void) {
  topLeft.setBrake(brake);
  topRight.setBrake(brake); 
  backLeft.setBrake(brake);
  backRight.setBrake(brake);

  while (true) {
    if (MENU_MODE == 2) {
      // drive train
      /*
      topLeft.spin(fwd, nja_md*(Controller1.Axis3.position(pct) + 0.7*Controller1.Axis1.position()), pct);
      topRight.spin(fwd, nja_md*(Controller1.Axis3.position(pct) - 0.7*Controller1.Axis1.position()), pct);
      backLeft.spin(fwd, nja_md*(Controller1.Axis3.position(pct) + 0.7*Controller1.Axis1.position()), pct);
      backRight.spin(fwd, nja_md*(Controller1.Axis3.position(pct) - 0.7*Controller1.Axis1.position()), pct);
      */

  topRight.spin(directionType::fwd,Controller1.Axis2.value(),velocityUnits::pct);
  backRight.spin(directionType::fwd,Controller1.Axis2.value(),velocityUnits::pct);
  topLeft.spin(directionType::fwd,Controller1.Axis3.value(),velocityUnits::pct);
  backLeft.spin(directionType::fwd,Controller1.Axis3.value(),velocityUnits::pct);
    }
 
    if (Controller1.ButtonL1.pressing()) { // cycle
      leftArm.spin(fwd, 100, pct);
      rightArm.spin(fwd, 100, pct);

      cut = 1;
    } else if (Controller1.ButtonL2.pressing()) {
      leftArm.spin(reverse, 40*cut, pct);
      rightArm.spin(reverse, 40*cut, pct);

      if (leftArm.position(deg) < -680) {
        cut = 0;
      }
    } else {
      leftArm.stop(hold);
      rightArm.stop(hold);
    }

    if (Controller1.ButtonLeft.pressing() && Controller1.ButtonDown.pressing()) {
      MENU_MODE = 0;
      SELECTED_AUTON = 0;
    }

    if (Controller1.ButtonB.pressing()) {
      nja_md =  nja_md == 1.0 ? 0.3 : 1.0;
    }

    if (Controller1.ButtonX.pressing() && Controller1.ButtonY.pressing()) {
      autonomous();
    }
  }
}
 
//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  
  // Run the pre-autonomous function.
  pre_auton();
  
  // Prevent main from exiting with an infinite loop.
  while (true) {
    if (MENU_MODE != 2) {
      selector();
    }
    ctrlRender();
  }
}
