//rover_controller_b_functions_motor

int setMotorPwm(int motorPin, int newPwm) 
{
  analogWrite(motorPin, newPwm*2); //AnalogWrite scales 0-255.  ocu pwm still comes
  return (newPwm);     // in as 0-125, so doubleuntil OCU pwm range is changed to 0-250
}

int setMotorDir(int dirPin, int newDir) 
{
  digitalWrite(dirPin, newDir);
  return(newDir);
}

void disableMotors(void)
{
  digitalWrite(motorEnablePin, MOTORS_OFF);
  motors_enabled_flag = 0;
  pwmMotorLeft=0;
  pwmMotorRight=0;
  pwmPan=0;
  pwmTilt=0;
  messageBuffer = "ALL STOP! MOTORS DISABLED! PRESS 'E' TO ENABLE";
}

void enableMotors(void)
{
  digitalWrite(motorEnablePin, MOTORS_ON);
  motors_enabled_flag = 1;
  messageBuffer = "MOTORS ENABLED";
}

void set_tractor_motor_pwms(void)
{
  if (motors_enabled_flag == 1)
  {
    setMotorPwm(motorLeftPin, pwmMotorLeft);
    setMotorDir(dirLeftPin, dirMotorLeft); 
    setMotorPwm(motorRightPin, pwmMotorRight);
    setMotorDir(dirRightPin, dirMotorRight); 
  }

  if (motors_enabled_flag == 0)
  {
    setMotorPwm(motorLeftPin, 0);
    setMotorDir(dirLeftPin, 1); 
    setMotorPwm(motorRightPin, 0);
    setMotorDir(dirRightPin, 1); 
  } 
}

