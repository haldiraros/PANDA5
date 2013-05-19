#ifndef LINEFOLLOWER_H
#define LINEFOLLOWER_H

void PWM(int left, int right);
void calibrate();
void read_adc();
int calc_err();
int PD();
void loop_LF();

extern int prog, array_sensors[7], sensors[7];
extern int prev_err, overshot;
extern int blad, pop_blad, Kp, Kd;
extern int V_zad;

extern volatile int on, exe_loop;

#endif
