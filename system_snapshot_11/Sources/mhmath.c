/*
 * mhmath.c

 *
 *  Created on: Aug 25, 2016
 *      Author: mmh
 */

#include "mhmath.h"
#define M_PI		3.14159265358979323846


//self developped fast cosine function
//Error < 500e-6
float fastcos(float x){
	//condition variable
	if(x<0) x=-x; //symetrize
	while(x>2*M_PI){
		x=x-2*M_PI;
	}
	float sign=1;
	if(x>M_PI){
		sign=-1;
		x=x-M_PI;
	}
	float x2=x*x;
	float x4=x2*x2;
	float cospre=1-0.497158878937*x2+0.0374784807595*x4;

	//adjust sign
	float cos=sign*cospre;
	return cos;
}
/*
float fastsin(float x){
	return fastcos(M_PI/2-x);
}*/
/*
 * import math
def nVidia_acos(x):
    negate = float(x<0)
    x=abs(x)
    ret = -0.0187293
    ret = ret * x
    ret = ret + 0.0742610
    ret = ret * x
    ret = ret - 0.2121144
    ret = ret * x
    ret = ret + 1.5707288
    ret = ret * math.sqrt(1.0-x)
    ret = ret - 2 * negate * ret
    return negate * 3.14159265358979 + ret
 */
float fastacos(float x){
	float ret;
	float negate=x<0;
	x=mh_fabs(x);
	ret=-0.0187293f;
    ret = ret * x;
    ret = ret + 0.0742610f;
    ret = ret * x;
    ret = ret - 0.2121144f;
    ret = ret * x;
    ret = ret + 1.5707288f;
    ret = ret * mh_sqrt(1.0f-x);
    ret = ret - 2 * negate * ret;
    return negate * 3.14159265358979f + ret;
}

float sqrt3_sign(float x){
	float sign=1;
	if(x<0) sign=-1;
	x=sign*v3rtf(x);
	return x;
}

