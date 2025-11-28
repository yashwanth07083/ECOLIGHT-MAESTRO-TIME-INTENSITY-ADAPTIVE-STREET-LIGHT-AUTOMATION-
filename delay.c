//delay.c
void delay_us(unsigned int dlyus)
{
	dlyus*=12;
	while(dlyus--);
}
void delay_ms(unsigned int dlyms)
{
	dlyms*=12000;
	while(dlyms--);
}
void delay_s(unsigned int dlys)
{
	dlys*=12000000;
	while(dlys--);
}



