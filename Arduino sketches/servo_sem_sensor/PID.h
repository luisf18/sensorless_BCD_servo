// ================================================================================
// CLASSE CONTROLADOR PID
// ================================================================================

class control_pid{
  public:
  float kp = 1;
  float ki = 0;
  float kd = 0;
  uint32_t last_ms = 0;
  float last_erro = 0;
  float P = 0;
  float I = 0;
  float D = 0;
  float Imax = 1000;
  float loop(float erro){
    uint32_t ms = millis();
    uint32_t dt = ms - last_ms; if( dt == 0 ) dt = 1;
    P = erro;
    D = 1000.0*( erro - last_erro )/(float)dt;
    //I = constrain( I+ki*0.001*erro*dt, -I_MAX, I_MAX );
    I = constrain( I+0.001*erro*dt, -Imax, Imax );
    last_erro = erro;
    last_ms = ms;
    //return P*kp + I*ki + kd*D;
    return kp*P + I*ki + kd*D;
  }
  void init(){
    I = 0;
  }
};
