#include <stdio.h>

#include <metal/lock.h>

METAL_LOCK_DECLARE(my_lock);

int main(void);
int secondary_main(int *start);

int _start = 0;

int slave_main(void) {
	int hartid = 0;
	__asm__ volatile("csrr %[hartid], mhartid"
			 : [hartid] "=r" (hartid) :: "memory");
	if(hartid == 0) {
		metal_lock_init(&my_lock);

		_start = 1;

		return main();
	} else if (hartid == 1){
		return secondary_main(&_start);
	} else {
		while(1) {
			__asm__("wfi");
		}
	}
}

int main(void) {
	while(1) {
		metal_lock_take(&my_lock);

		printf("Hart 0\n");

		metal_lock_give(&my_lock);
	}
}

int secondary_main(int *start) {
	while(!(*start)) ;	

	while(1) {
		metal_lock_take(&my_lock);

		printf("Hart 1\n");

		metal_lock_give(&my_lock);
	}
}
