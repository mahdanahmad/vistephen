#include <math.h>
#include <TimerOne.h>
#include "HX711.h"

#define P1 A2
#define P2 A3
#define IM A4

#define MIN1 3
#define MIN2 11 
#define MEN 12
#define MSLP 13

#define MAX_PWM 255
#define MIN_PWM 0

unsigned long time;

float potentio1[4];
float potentio2[4];
float p1_origin[4];
float p2_origin[4];
float pc1[2];
float pc2[2];
float vp1[2];
float vp2[2];
float ap1[2];
float ap2[2];
float mc[1];
float vm[1];
float am[1];

float x[2];
float y[2];
float xo[2];
float yo[2];
float xc[2];
float vc[2];

const float filterv = 0.98;
const float filtera = 0.98;
const float dt= 0.166;

float vi_motor[2];
float i_motor[2];
float id_motor[2];
float rlc=0;
float lc=0;
float force = 0;
float torque = 0;
float ferr[2];
float dferr[2];
float iferr[2];

float m1=0.13456; // sector mass kg
float m2=0.04162; // connecting rod kg 
float m3=0.06814; // load mass kg
float Ia=0.0003525009; // inertia sector kgm^2
float Ib=0.0002083289; // intertia connecting rod kgm^2
float g=9.8; // gravity accel m2
float p=0.02016; // sector cog to origin m
float l=0.105; // crank radius m
float r=0.075; // connecting rood length m
float n=15; // driver - sector pulley ratio

float Idr=0.000007572; // inertia driver parts kgm^2
float Im=0.0000050; // inertia rotor kgm^2
float Ip=0.00000003; // inertia potetiometer kgm^2
float Beff=0.9223; // damping driver efffective
float kt=0.1792; // K torque motor
float kemf=0.2134; // K emf
float R=13.6; //motor resistance Ohm
float V;
float PWM;
float fdes=0;
float xdev=0;
unsigned int dir;

float M1a;
float M1b;
float M1c;
float M2a;
float M2b;
float M3;
float Ieff;
float T;
float kp=0.03;
float ti=1000;
float td=0;


HX711 scale(A1, A0);

void setup() {

  Serial.begin(9600);

  //change timer2 register B prescaler at digital pin 3&11 for PWM frequency 31372.55 Hz. CANNOT use tone, spi!
  TCCR2B = TCCR2B & B11111000 | B00000001; //0x01 
  pinMode(MIN1, OUTPUT);
  pinMode(MIN2, OUTPUT);
  pinMode(MEN, OUTPUT);
  pinMode(MSLP, OUTPUT);

  scale.set_scale(436.f);
  save_init();
}


void loop() { 
  time = millis();
  if (Serial.available()){
    potentio1[1] = analogRead(P1);
    if(potentio1[1]<350)&&(potentio1[1]>70){
    haptics();
    r_force();
    control();
    Serial.println(xc[0]);
    Serial.println(yo[0]);
    Serial.println(p2_origin[1]);
    Serial.print(rlc);
    delay(1);
    }
  }  
}

void save_init(){
  //initial position
  potentio1[0] = analogRead(P1);
  potentio2[0] = analogRead(P2);
  p1_origin[0]=0.01744*(540-potentio1[0])/4.5;
  p2_origin[0];
  if(potentio2[0]<900){
    p2_origin[0]=0.01744*(potentio2[0]-990)/21.9;
  }
  else if(potentio2[0]>900 && potentio2[0]<1024){
    p2_origin[0]=0.01744*(((potentio2[0]-990)/8.8)+6);
  }

  pc1[0]=p1_origin[0];
  pc2[0]=p2_origin[0];
  vp1[0]=0;
  vp2[0]=0;
  ap1[0]=0;
  ap2[0]=0;
  
  x[0]=11.5*cos(p1_origin[0])+sqrt(132.25-56.25*sin(p1_origin[0])*sin(p1_origin[0]));
  y[0]=x[0]*sin(p2_origin[0]);
  xo[0]=x[0]*cos(p2_origin[0]);
  yo[0]=y[0];

  xc[0]=0;
  vc[0]=0;
  vi_motor[0]=0;
  i_motor[0]=0;
  id_motor[0]=0;

  ferr[0]=0;
  dferr[0]=0;
  iferr[0]=0;
}

void haptics(){
  //kinematis
  potentio1[1] = analogRead(P1);
  potentio2[1] = analogRead(P2);
  p1_origin[1]=0.01744*(540-potentio1[1])/4.5;
  p2_origin[1];
  if(potentio2[1]<900){
    p2_origin[1]=0.01744*(potentio2[1]-990)/21.9;
  }
  else if(potentio2[1]>=900){
    p2_origin[1]=0.01744*(((potentio2[1]-990)/8.8)+6);
  }
  
  pc1[1]=p1_origin[1]+pc1[0];
  pc2[1]=p2_origin[1]+pc2[0];
  vp1[1]=filterv*vp1[0]+(1-filterv)*(p1_origin[1]-pc1[0])/dt;
  vp2[1]=filterv*vp2[0]+(1-filterv)*(p2_origin[1]-pc2[0])/dt;
  ap1[1]=filtera*ap1[0]+(1-filtera)*(vp1[1]-vp1[0])/dt;
  ap2[1]=filtera*ap2[0]+(1-filtera)*(vp2[1]-vp2[0])/dt;
  ap1[0]=ap1[1];
  ap2[0]=ap2[1];
  vp1[0]=vp1[1];
  vp2[0]=vp2[1];
  pc1[0]=p1_origin[1];
  pc2[0]=p2_origin[1];
  
  x[1]=11.5*cos(p1_origin[1])+sqrt(132.25-56.25*sin(p1_origin[1])*sin(p1_origin[1]));
  y[1]=x[1]*sin(p2_origin[1]);
  xo[1]=x[1]*cos(p2_origin[1]);
  yo[1]=y[1];

  xc[1]=x[1]+xc[0]-x[0];
  vc[1]=x[1]-xc[0]/dt;
  xc[0]=x[1];
  vc[0]=vc[1];

  //load cell
  rlc=scale.get_units();
  float lc_offset=22+48*sin(p2_origin[1]);
  lc=rlc-lc_offset;

  //motor
  vi_motor[1] = analogRead(IM);
  i_motor[1] = (vi_motor[1]/1023)*2.08;
  id_motor[1] = (i_motor[1]-i_motor[0])/dt;
  vi_motor[0] = vi_motor[1];
  i_motor[0] = i_motor[1];
  id_motor[0] = id_motor[1];
  
}

void set_pwm(int a, int s){
  if (s> MAX_PWM){
    s=MAX_PWM;
  }
  if(a==1){//FORWARD 
    analogWrite(MIN1,s);
    digitalWrite(MIN2,LOW);  
    digitalWrite(MEN,HIGH);   
    digitalWrite(MSLP,HIGH);
  }
  else if (a==2){ //BACKWARD
    s=255-s;  
    analogWrite(MIN1,s);
    digitalWrite(MIN2,HIGH);
    digitalWrite(MEN,HIGH);   
    digitalWrite(MSLP,HIGH);
  }
}

void control(){
  ferr[1]=fdes-(rlc/100);
  dferr[1]=(ferr[1]-ferr[0])/dt;
  //if(abs(ferr[1])<200){
    iferr[1]=iferr[0]+ferr[1];
  //}
  ////else{
    ///iferr[1]=0;
  //}
  ferr[0]=ferr[1];
  dferr[0]=dferr[1];
  iferr[0]=iferr[1];

  mc[0]=n*pc1[0];
  vm[0]=n*vp1[0];
  am[0]=n*ap1[0];
  
  M1a=2*(1/2*Ia+1/2*m1*p*p);
  M1b=2*(1/2*Ib+1/2*m2*r*r*sin(pc1[0])*sin(pc1[0])+1/8*m2*((l*l*r*r*cos(pc1[0])*cos(pc1[0]))/(l*l-r*r*sin(pc1[0])*sin(pc1[0])))+1/2*m2*((r*r*sin(pc1[0])*sin(pc1[0])*cos(pc1[0]))/sqrt(l*l-r*r*sin(pc1[0])*sin(pc1[0]))));
  M1c=2*(1/2*m3*r*r*sin(pc1[0])*sin(pc1[0])+1/2*m3*((r*r*r*r*sin(pc1[0])*sin(pc1[0])*cos(pc1[0])*cos(pc1[0]))/(l*l-r*r*sin(pc1[0])*sin(pc1[0])))+m3*((r*r*sin(pc1[0])*sin(pc1[0])*cos(pc1[0]))/sqrt(l*l-r*r*sin(pc1[0])*sin(pc1[0]))));
  
  M2a=(1/2*m2*r*r*sin(2*pc1[0])+1/8*m2*l*l*(((2*r*r*r*r*sin(pc1[0])*cos(pc1[0])*cos(pc1[0])*cos(pc1[0]))/((l*l-r*r*sin(pc1[0])*sin(pc1[0]))*(l*l-r*r*sin(pc1[0])*sin(pc1[0]))))-((2*r*r*sin(pc1[0])*cos(pc1[0]))/(l*l-r*r*sin(pc1[0])*sin(pc1[0]))))+1/2*m2*(((r*r*r*r*r*sin(pc1[0])*sin(pc1[0])*sin(pc1[0])*cos(pc1[0])*cos(pc1[0]))/sqrt((l*l-r*r*sin(pc1[0])*sin(pc1[0]))*(l*l-r*r*sin(pc1[0])*sin(pc1[0]))*(l*l-r*r*sin(pc1[0])*sin(pc1[0]))))-((r*r*r*sin(pc1[0])*sin(pc1[0])*sin(pc1[0]))/sqrt(l*l-r*r*sin(pc1[0])*sin(pc1[0])))+((2*r*r*sin(pc1[0])*cos(pc1[0])*cos(pc1[0]))/sqrt(l*l-r*r*sin(pc1[0])*sin(pc1[0])))));
  M2b=(1/2*m3*r*r*sin(2*pc1[0])+1/2*m3*(((2*r*r*r*r*r*r*sin(pc1[0])*sin(pc1[0])*sin(pc1[0])*cos(pc1[0])*cos(pc1[0])*cos(pc1[0]))/((l*l-r*r*sin(pc1[0])*sin(pc1[0]))*(l*l-r*r*sin(pc1[0])*sin(pc1[0]))))+((2*r*r*r*r*sin(pc1[0])*cos(pc1[0])*cos(pc1[0])*cos(pc1[0]))/(l*l-r*r*sin(pc1[0])*sin(pc1[0])))-((2*r*r*sin(pc1[0])*sin(pc1[0])*sin(pc1[0])*cos(pc1[0]))/(l*l-r*r*sin(pc1[0])*sin(pc1[0]))))+m3*(((r*r*r*r*r*sin(pc1[0])*sin(pc1[0])*sin(pc1[0])*cos(pc1[0])*cos(pc1[0]))/sqrt((l*l-r*r*sin(pc1[0])*sin(pc1[0]))*(l*l-r*r*sin(pc1[0])*sin(pc1[0]))*(l*l-r*r*sin(pc1[0])*sin(pc1[0]))))-((r*r*r*sin(pc1[0])*sin(pc1[0])*sin(pc1[0]))/sqrt(l*l-r*r*sin(pc1[0])*sin(pc1[0])))+((2*r*r*sin(pc1[0])*cos(pc1[0])*cos(pc1[0]))/sqrt(l*l-r*r*sin(pc1[0])*sin(pc1[0])))));
  
  M3=m1*g*p*cos(pc1[0])-0.5*m2*g*r*cos(pc1[0]);

  Ieff=Idr+Im+Ip+(Ia/(n*n));

  T=((1/n)*(M1a+M1b+M1c)+Ieff)*am[0]+(Beff-((1/(n*n))*(M2a+M2b)))*vm[0]-M3+kp*ferr[0]+(kp/ti)*iferr[0]+(kp*td)*dferr[0];

  V=(T*R/kt)+kemf*vm[0];

  PWM = (V/12)*255;
  dir = 1;

  set_pwm(dir,PWM);
}


void r_force(){
  float xdev=x[0]-xc[0];
  if (xdev>0)&&(xdev<=1){
    fdes=0;
  }
  else if (xdev>1)&&(xdev<=2.5){
    fdes=4.0248*(xdev-1);
  }
  else if (xdev>2.5)&&(xdev<=2.9){
    fdes=6.0372-4.218*(xdev-1.5-1);
  }
  else if (xdev>2.9)&&(xdev<=3.7){
    fdes=4.354+3.8913*(xdes-1.9-1);
  }
  else if (xdev>3.7)&&(xdev<=5.5){
    fdes=7.467;
  }
  else if (xdev>5.5)&&(xdev<=5.9){
    fdes=7.467+11.665*(xdev-4.5-1);
  }
  else if (xdev>5.9)&&(xdev<=6.7){
    fdes=12.1330-12.1213*(xdev-4.9-1);
  }             
  else if (xdev>6.7)&&(xdev<=7.3){
    fdes=2.436;
  }
  else if (x(i)>7.3)&&(x(i)<=8){
    fdes=20;
  }        
  else{
    fdes=0;
  }  
}


void test_motor(){
  int pwm=0;
  int dir=1;
  set_pwm(dir,pwm);
  if (Serial.available()){
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      pwm += 1;
    else if(temp == '-' || temp == 's')
      pwm -= 1;
    else if(temp == '0' || temp == 'z')
      pwm=0;
  }
}

