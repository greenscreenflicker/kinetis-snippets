/*
 * mhmath.h
 *
 *  Created on: Aug 25, 2016
 *      Author: mmh
 */

#ifndef INCLUDES_MHMATH_H_
#define INCLUDES_MHMATH_H_


__attribute__((always_inline)) static float inline v3rtf(float op1) {
    float result;
    __asm__ volatile ("VMUL.f32 %0, %1, %1\n"
    		"vsqrt.f32 %0, %0\n"
    		"vsqrt.f32 %0, %0\n"
    		"vsqrt.f32 %0, %0\n"
    		: "=w" (result)
			: "w" (op1) );
    return (result);
}

__attribute__((always_inline)) static float inline mh_sqrt(float op1) {
    float result;
    __asm__ volatile ("vsqrt.f32 %0, %1" : "=w" (result) : "w" (op1) );
    return (result);
}

__attribute__((always_inline)) static float inline mh_6thrt(float op1) {
    float result;
    __asm__ volatile (
    		"vsqrt.f32 %0, %0\n"
    		"vsqrt.f32 %0, %0\n"
    		"vsqrt.f32 %0, %0\n"
    		: "=w" (result)
			: "w" (op1) );
    return (result);
}

__attribute__((always_inline)) static float inline mh_fabs(float op1) {
    float result;
    __asm__ volatile ("VABS.f32 %0, %1\n"
    		: "=w" (result)
			: "w" (op1) );
    return (result);
}


float fastcos(float x);
float fastacos(float x);

#endif /* INCLUDES_MHMATH_H_ */
