/*
 * storeLoadMechanicalTime.h
 *
 *  Created on: 1 apr. 2025
 */

#ifndef STORELOADMECHANICALTIME_H_
#define STORELOADMECHANICALTIME_H_

#include <stdint.h>  // Include to define uint32_t

//void storeTime(uint32_t  data);
//uint32_t  loadTime();

void write_SegC(uint32_t value);
uint32_t load_from_flash(void);

#endif /* STORELOADMECHANICALTIME_H_ */
