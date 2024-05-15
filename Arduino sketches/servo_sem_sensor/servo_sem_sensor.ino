#include "PID.h"

control_pid pid;

int set = 0;
int sig_min  = 3;//10;
int sig_zero = 514;
int delta_max = 60;
int value = 0;
float value_f = 0;
int out = 0;

Template <>
void command_update_value( const char name,  &var ){

}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  pinMode( 3,OUTPUT);
  pinMode(11,OUTPUT);
  //analogWrite(3,50);
  pid.init();
  pid.kp = 1;
  pid.ki = 15;

  TCCR2B = TCCR2B&0b11111000 | 0b00000100;

}

void loop() {

  while( Serial.available() > 0 ){
    String cmd = Serial.readStringUntil(';');
    cmd.trim();
         if( cmd.indexOf("A" ) >= 0 ) OCR2A = constrain( cmd.substring(1).toInt(), 0, 255 );
    else if( cmd.indexOf("B" ) >= 0 ) OCR2B = constrain( cmd.substring(1).toInt(), 0, 255 );
    else if( cmd.indexOf("kp") >= 0 ) pid.kp = cmd.substring(2).toFloat();
    else if( cmd.indexOf("ki") >= 0 ) pid.ki = cmd.substring(2).toFloat();
    else if( cmd.indexOf("kd") >= 0 ) pid.kd = cmd.substring(2).toFloat();
    else if( cmd.indexOf("Im") >= 0 ) pid.Imax = cmd.substring(2).toFloat();
    else if( cmd.indexOf("Dm") >= 0 ) delta_max = cmd.substring(2).toFloat();
    else if( cmd.indexOf("min") >= 0 ) sig_min = (int) cmd.substring(3).toFloat();
    else if( cmd.indexOf("reset") >= 0 ) pid.init();
    else if( cmd.indexOf("tare") >= 0 ){ sig_zero = ( analogRead( A0 ) + analogRead( A0 ) + analogRead( A0 ) )/3; pid.init(); }
    else if( cmd.indexOf("pid.on") >= 0 ){ pid_on = true; }
    //else if( cmd.indexOf("test") >= 0 ) test();
    else{ set = constrain( cmd.toInt(), 0-sig_zero, 1023-sig_zero ); pid.init(); }
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
  }

  digitalWrite( 11, 0 );
  digitalWrite(  3, 0 );
  
  delay(2);
  
  int last = value;
  
  //value = ( analogRead(A0) + analogRead(A0) + analogRead(A0) )/3;
  value = analogRead(A0);
  value = value - sig_zero;
  if( abs(value) < sig_min ) value = 0;
  if( abs( value - last ) > delta_max ) value = constrain( value, last-delta_max, last+delta_max );
  value_f = value_f*0.92 + value*0.08;

  // OCR2A(11): 92	OCR2B(3): 12	set: 0	kp: 8.00	ki: 5.00	kd: 0.00	min: 3	tare: 514

  out = constrain( pid.loop( set - value_f ), -255, 255 );
  
  //Serial.println( value );
  Serial.print( set   );Serial.write( '\t' );
  //Serial.print( servo   );Serial.print( "°\t" ); // angulo
  //Serial.print( pid.I*0.52644 );Serial.write( '\t' ); // angulo
  Serial.print( value );Serial.write( '\t' );
  Serial.print( value_f );Serial.write( '\t' );
  //Serial.print( real  );Serial.write( '\t' );
  Serial.print( pid.I );Serial.write( '\t' );
  //Serial.print( pid.D );Serial.write( '\t' );
  //Serial.print( real_f );Serial.write( '\t' );
  Serial.print( out   );Serial.write( '\n' );
  
  //analogWrite( 3, OCR2B );
  if( set == 0 ){
    if( abs(out) < 10 ){
      digitalWrite(  3, HIGH );
      digitalWrite( 11, HIGH );
    }else if( out > 0 ){
      digitalWrite( 3, LOW );
      analogWrite( 11, out );
    }else{
      digitalWrite( 11,  LOW );
      analogWrite(   3, -out );
    }
  }else{
    if( set > 0 ){
      digitalWrite( 3, LOW );
      //0.5*set+
      analogWrite( 11, constrain( out, 0, 255 ) );
    }else{
      //0.5*set+
      digitalWrite( 11,  LOW );
      analogWrite(   3, constrain( -out, 0, 255 ) );
    }
  }

  delay( 50 );
}
