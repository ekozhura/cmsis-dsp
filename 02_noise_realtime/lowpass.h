#ifndef __LOWPASS_H__
#define __LOWPASS_H__

#include "arm_math.h"

void lowpass_filter_init(void);

q15_t lowpass_filter(q15_t *input);

#endif
