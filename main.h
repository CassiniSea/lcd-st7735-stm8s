#ifndef __MAIN_H
#define __MAIN_H

	@far @interrupt void tim1UpdateInterrupt(void);
	@far @interrupt void tim4UpdateInterrupt(void);
	@far @interrupt void gpioaExtiInterrupt(void);
	@far @interrupt void gpiocExtiInterrupt(void);

#endif