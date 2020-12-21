//
// Created by Y on 2020/11/3.
//

#ifndef WUHUFLY_JY901_H
#define WUHUFLY_JY901_H

#include "main.h"

#define debug_JY901 0

extern float ax,ay,az;
extern float wx,wy,wz;
extern float roll,pitch,yaw;
extern float Q0,Q1,Q2,Q3;

void readJY901(void);
void calculateJY901(void);

#endif //WUHUFLY_JY901_H
