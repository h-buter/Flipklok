/*
 * storeLoadMechanicalTime.h
 *
 *  Created on: 1 apr. 2025
 */

#ifndef STORELOADMECHANICALTIME_H_
#define STORELOADMECHANICALTIME_H_

#include <stdint.h>  // Include to define uint32_t

void storeTime(uint32_t  data);
uint32_t  loadTime();

#define INFO_SEGMENT_ADDRESS 0x1000  // Info Memory segment C, see page 319 of SLAU144K

#endif /* STORELOADMECHANICALTIME_H_ */
