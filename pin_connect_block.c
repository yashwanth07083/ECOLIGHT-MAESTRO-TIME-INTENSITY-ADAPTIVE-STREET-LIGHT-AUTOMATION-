#include<LPC21XX.h>
void cfgportpinfun(unsigned int PortNo,unsigned int PinNo,unsigned int PinFun)
{
	if(PortNo==0)
	{
		if((signed)PinNo>=0 && PinNo<=15)
		{
			PINSEL0=((PINSEL0&(~(3<<(PinNo*2))))|(PinFun<<(PinNo*2)));
		}
		else if(PinNo>=16 &&PinNo<=31)
		{
			PINSEL1=((PINSEL1&(~(3<<((PinNo-16)*2))))|(PinFun<<((PinNo-16)*2)));
		}
	}/*
	else if(PortNo==1)
	{
		PINSEL2=((PINSEL2&(~(3<<((PinNo-16)*2))))|(PinFun<<((PinNo-16)*2)));
	}*/
}


	
