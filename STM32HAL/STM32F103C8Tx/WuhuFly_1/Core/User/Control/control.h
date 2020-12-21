#ifndef   _CONTROL_H
#define   _CONTROL_H

#include "main.h"
#include "structconfig.h"

void Control(FLOAT_ANGLE *att_in,FLOAT_XYZ *gyr_in, RC_TYPE *rc_in);
void Yaw_Carefree(FLOAT_ANGLE *Target_Angle, const FLOAT_ANGLE *Measure_Angle);
#endif

