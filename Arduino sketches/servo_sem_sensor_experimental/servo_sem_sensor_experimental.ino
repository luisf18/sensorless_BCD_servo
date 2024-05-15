#include "PID.h"

control_pid pid;

//153,235

int   set = 0;
int   sig_min  = 3;//10;
int   sig_zero = 514;
int   delta_max = 60;
int   value = 0;
float value_f = 0;
int   out = 0;
bool  pid_on = true;
int   set_pwm = 0;
float conv_deg = 1.0;
int   pwm_min = 20;
int   pwm_max = 50;

float pid_kp_0 = 1.2;
float pid_ki_0 = 15;
float pid_kp_run = 1;
float pid_ki_run = 3;


void control_loop( float set ){
  
  delay(2);
  
  int last = value;
  
  //value = ( analogRead(A0) + analogRead(A0) + analogRead(A0) )/3;
  int value_raw = analogRead(A0);
  value_raw = value_raw - sig_zero;
  value = value_raw;
  if( abs(value) < sig_min ) value = 0;
  
  if( set != 0 ){
    //if( abs( value - last ) > delta_max ) value = constrain( value, last-delta_max, last+delta_max );
    //value_f = value_f*0.92 + value*0.08;
    value_f = value_f*0.92 + value*0.08;
  }else{
    value_f = value;
  }

  // OCR2A(11): 92	OCR2B(3): 12	set: 0	kp: 8.00	ki: 5.00	kd: 0.00	min: 3	tare: 514

  out = pid.loop( set - value_f );
  if( !pid_on ) out = set_pwm;

  if( set == 0 ){
    if( abs(out) < 20 ){
      digitalWrite(  3, HIGH );
      digitalWrite( 11, HIGH );
    }else if( out > 0 ){
      out = constrain( out+pwm_min/2, 0, pwm_max );
      digitalWrite( 3, LOW );
      analogWrite( 11, out );
    }else{
      out = constrain( out-pwm_min/2, -pwm_max, 0 );
      digitalWrite( 11,  LOW );
      analogWrite(   3, -out );
    }
  }else{
    if( out > 0 ){
      out = constrain( out+pwm_min/2, 0, pwm_max );
      digitalWrite( 3, LOW );
      analogWrite( 11, out );
    }else{
      out = constrain( out-pwm_min/2, -pwm_max, 0 );
      digitalWrite( 11,  LOW );
      analogWrite(   3, -out );
    }
  }

  // plot
  Serial.print( set       );Serial.write( '\t' );
  Serial.print( value_raw );Serial.write( '\t' );
  Serial.print( value_f   );Serial.write( '\t' );
  Serial.print( pid.I     );Serial.write( '\t' );
  Serial.print( out       );Serial.write( '\n' );
  //Serial.print( pid.I*conv_deg );Serial.write( '\n' );

  delay( 20 );

  digitalWrite( 11, 0 );
  digitalWrite(  3, 0 );

}

void test( float sp, uint16_t n, int pos ){

  //float pid_kp = pid.kp;
  //float pid_ki = pid.ki;

  //set = constrain( sp, 0-sig_zero, 1023-sig_zero );
  //if( set==0 ){
  //  pid.kp = pid_kp_0;
  //  pid.ki = pid_ki_0;
  //}else{
  //  pid.kp = pid_kp_run;
  //  pid.ki = pid_ki_run;
  //}
  
  delay(500);
  pid.I = pos;
  Serial.println( "BEGIN" );
  Serial.print( "n\ttime[ms]\tsetpoit\tinput_raw\tinput\tIntegral\tout -> kp" );
  Serial.print( pid.kp );
  Serial.print( " ki " );
  Serial.println( pid.ki );

  for(int i=0;i<n;i++){
    Serial.print( i ); Serial.write('\t');
    Serial.print( millis() ); Serial.write('\t');
    control_loop( sp );
  }
  Serial.println( "END" );
  delay(500);
  pid.init();

  //pid.kp = pid_kp;
  //pid.ki = pid_ki;

}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  pinMode( 3,OUTPUT);
  pinMode(11,OUTPUT);
  //analogWrite(3,50);
  
  pid.init();
  pid.kp = pid_kp_0;
  pid.ki = pid_ki_0;

  // change pwm frequency
  TCCR2B = TCCR2B&0b11111000 | 0b00000100;

}

void loop() {

  while( Serial.available() > 0 ){
    String cmd = Serial.readStringUntil(';');

    //char key[20]; float x,y,z;
    //int i = sscanf(cmd,"%s %f %f %f ",key,&x,&y,&z);
    //if(i==0){ Serial.println("> fail"); continue; }

    cmd.trim();
    bool scan = (cmd.indexOf("scan") >= 0);

    if( !scan ){
           if( cmd.indexOf("A" ) >= 0 ) OCR2A = constrain( cmd.substring(1).toInt(), 0, 255 );
      else if( cmd.indexOf("B" ) >= 0 ) OCR2B = constrain( cmd.substring(1).toInt(), 0, 255 );
      else if( cmd.indexOf("kp") >= 0 ) pid.kp = cmd.substring(2).toFloat();
      else if( cmd.indexOf("ki") >= 0 ) pid.ki = cmd.substring(2).toFloat();
      else if( cmd.indexOf("kd") >= 0 ) pid.kd = cmd.substring(2).toFloat();
      else if( cmd.indexOf("Im") >= 0 ) pid.Imax = cmd.substring(2).toFloat();
      else if( cmd.indexOf("Dm") >= 0 ) delta_max = cmd.substring(2).toFloat();
      else if( cmd.indexOf("min") >= 0 ) sig_min = (int) cmd.substring(3).toFloat();
      else if( cmd.indexOf("pwm.min") >= 0 ){ pwm_min = (int) cmd.substring(7).toFloat(); }
      else if( cmd.indexOf("pwm.max") >= 0 ){ pwm_max = (int) cmd.substring(7).toFloat(); }
      else if( cmd.indexOf("reset") >= 0 ) pid.init();
      else if( cmd.indexOf("tare") >= 0 ){ sig_zero = ( analogRead( A0 ) + analogRead( A0 ) + analogRead( A0 ) )/3; pid.init(); }
      else if( cmd.indexOf("pid.on") >= 0 ){ pid_on = true; }
      else if( cmd.indexOf("pid.off") >= 0 ){ pid_on = false; }
      else if( cmd.indexOf("move") >= 0 ){ pid_on = false; set_pwm = (int) cmd.substring(4).toFloat(); }
      else if( cmd.indexOf("conv") >= 0 ){ conv_deg = 360.0/pid.I; }
      else if( cmd.indexOf("test") >= 0 ){
        cmd = cmd.substring(4);
        int sp = 0;
        int n = 200;
        int pos = 0;
        int i = sscanf( cmd.c_str(), "%i %i %i", &sp, &n, &pos );
        if( i >= 1 ){ test( sp, n, pos );}
        else{ Serial.print( cmd ); Serial.print( i ); Serial.println( ">> FAIL!!" ); }
      }
      else{
        set = constrain( cmd.toInt(), 0-sig_zero, 1023-sig_zero );
        if( set==0 ){
          pid.kp = pid_kp_0;
          pid.ki = pid_ki_0;
        }else{
          pid.kp = pid_kp_run;
          pid.ki = pid_ki_run;
        }
        pid.init();
      }
    }

    Serial.println(
      "OCR2A(11): " + String( OCR2A ) +
      "\tOCR2B(3): " + String( OCR2B ) +
      "\tset: " + String( set ) + 
      "\tkp: " + String( pid.kp ) + 
      "\tki: " + String( pid.ki ) + 
      "\tkd: " + String( pid.kd ) + 
      "\tmin: " + String( sig_min ) +
      "\ttare: " + String( sig_zero )
    );
    if(scan) delay( 2000 );
  }

  control_loop( set );
  
  
}
