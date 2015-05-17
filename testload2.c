#include "testload2.h"

void call_user_start() {
	uint8 loop;
	for(loop = 0; loop < 50; loop++) {
		ets_printf("testload 2\r\n");
		ets_delay_us(20000);
	}
}
