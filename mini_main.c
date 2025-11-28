#include <lpc21xx.h>
#include "mini_kpm_defines.h"
#include "mini_lcd_defines.h"
#include "delay.h"
#include "types.h"
#include "rtc_defines.h"
#include "mini_adc_defines.h"
#include "mini_macro_defines.h"
#include "pin_connect_block.h"


//LED OUTPUT PINS
#define LED_OPINS 8

//KEYPAD REQUIRED VARIABLES
unsigned char kpmLUT[4][4]={{'7','8','9','/'},{'4','5','6','*'},{'1','2','3','-'},{'c','0','=','+'}};
int num;
char key;

// Array to hold names of days of the week
char week[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};

//RTC variables to get or set the data
s32 hour,min,sec,date,month,year,day;

//Interrupts defines
#define EINT0_FUN FUN2
#define FUN2 1
#define EINT0 14

int request=0;//If external raised for the request then user use this variable

//ADC Analog variable
//float ldr_val=0.0;
float eAR=0.0;
float setval=2.5;
//unsigned int adcval=0;

//Initialize the KPM
void InitKPM(void)
{
	WRITENIBBLE(IODIR1,ROW0,15);
}

//Check whether key pressed or not using status
unsigned int colscan(void)
{
	unsigned int status;
	status=(READNIBBLE(IOPIN1,COL0)<15)?1:0;
	return status;
}

//To check the which row number
unsigned int rowcheck(void)
{
	unsigned int r;
	for(r=0;r<4;r++)
	{
		WRITENIBBLE(IOPIN1,ROW0,~(1<<r));
		if(colscan())
		{
			break;
		}
	}
	WRITENIBBLE(IOPIN1,ROW0,0);
	return r;
}
//To check the which column number
unsigned int colcheck(void)
{
	unsigned int c;
	for(c=0;c<4;c++)
	{
		 if(READBIT(IOPIN1,(COL0+c))==0)
			 break;
	}
	return c;
}
//To check which key pressed using KPM Lookuptable using rowcheck and colomncheck
char keyscan(void)
{
	unsigned int r,c;
	char key;
	while(colscan()==0);//wait untill the keypress
	r=rowcheck();//row value
	c=colcheck();//colomn value
	key=kpmLUT[r][c];//character value from keypad(KPM lookup table)
	return key;
}
//Scan the number pressed in keypad
void readnum(int *num,char *key)
{
	*num=0;
	while(1)
	{
		*key=keyscan();
		while(colscan());//wait untill push button release
		delay_ms(100);
		
		if((*key>='0' && *key<='9'))//If pressed button is number
		{
			*num=((*num*10)+(*key-48));//convert charcter to integer convertion
		}
		else//If pressed button is other than number or Enter(=)
			break;
	}
}
//To Write In the LCD
void writeLCD(unsigned char anybyte)
{
	//select write operation
	//IOCLR0=1<<RW Register write is grounded in the kit
	//put any byte onto data pins 
	WRITEBYTE(IOPIN0,DPINS,anybyte);
	//provide high to low pulse
	IOSET0=1<<EN;
	delay_us(1);
	IOCLR0=1<<EN;
	delay_us(1);
}
//To command LCD
void cmdLCD(unsigned char cmd)
{
	//select CMD register
	IOCLR0=1<<RS;
	//write CMD to register
	writeLCD(cmd);
	//delay
	delay_ms(1);
}
//To write character in the LCD
void charLCD(unsigned char ch)
{
	//To select data reg
	IOSET0=1<<RS;
	//write ascii value via data register to ddram/display
	writeLCD(ch);
	//delay
	delay_ms(1);
}
//To Initialize the LCD
void InitLCD(void)
{
	//configure the po.18 to 25 as gpio output pins
	IODIR0=(255<<DPINS|1<<RS|1<<EN);
	//power on delay
	delay_ms(15);
	//To Give command MODE_8BIT_1LINE
	cmdLCD(MODE_8BIT_1LINE);
	//power on delay
	delay_ms(5);
	//To Give command MODE_8BIT_1LINE
	cmdLCD(MODE_8BIT_1LINE);
	//power on delay
	delay_ms(100);
	//To Give command MODE_8BIT_1LINE,2line
	cmdLCD(MODE_8BIT_1LINE);
	cmdLCD(MODE_8BIT_2LINE);
	//To DISP_ON_CUR_OFF
	cmdLCD(DISP_ON_CUR_OFF);
	//To CLEAR LCD
	cmdLCD(CLEAR_LCD);
	//Giving the command to move right
	cmdLCD(SHIFT_CUR_RIGHT);
}
//To Display String in LCD
void strLCD(char *ptr)
{
	while(*ptr)
	{
		charLCD(*ptr++);//TO display Each character from the string
	}
}
//To display Integer in LCD
void u32LCD(int num)
{
	//To store each digit of number as a character in Array
	unsigned int arr[20],rem;
	int i=0;
	if(num==0)
	{
		charLCD('0');
		return;
	}
  while(num)
	{
		rem=num%10;
		arr[i++]=rem+48;//Converting each digit number as character in array
		num=num/10;
	}
	for(--i;i>=0;i--)//Running loop from last to display each one
	{
		charLCD(arr[i]);//TO display Each character from the Array
	}
}
//To display float values
void F32LCD(float fNum,unsigned char nDP)
{
  unsigned int num,i;
	if(fNum<0.0)
	{
		charLCD('-');
		fNum=-fNum;
	}
	num=fNum;
	u32LCD(num);
	charLCD('.');
	for(i=0;i<nDP;i++)
	{
		fNum=(fNum-num)*10;
		num=fNum;
		charLCD(num+48);
	}
}

void InitLEDS(void)
{
	//Configure the p0.8 to p0.15 as Gpio output pins
	WRITEBYTE(IODIR0,LED_OPINS,255);
	//set off state initially the Lower NIBBLE LEDS as Active low and upper NIBBLE LEDS as Active high 
	WRITEBYTE(IOPIN0,LED_OPINS,(0xf0^255));
}
/*
Initialize the Real-Time Clock (RTC)
This function disables the RTC, sets the prescaler values, 
and then enables the RTC.
*/
void RTC_Init(void) 
{
  // Disable and reset the RTC
	CCR = RTC_RESET;

#if (CPU == LPC2129)	
  // Set prescaler integer and fractional parts
	PREINT = PREINT_VAL;
	PREFRAC = PREFRAC_VAL;
	CCR = RTC_ENABLE; 
#elif(CPU == LPC2148)
  // Enable the RTC
	CCR = RTC_ENABLE | RTC_CLKSRC;  
#endif
}

/*
Get the current RTC time
hour Pointer to store the current hour
minute Pointer to store the current minute
second Pointer to store the current second
*/
void GetRTCTimeInfo(s32 *hour, s32 *minute, s32 *second)
{
	*hour = HOUR;
	*minute = MIN;
	*second = SEC;
}

/*
Display the RTC time on LCD
hour value (0 23)
minute value (0 59)
second value (0 59) seperated by ':'
*/
void DisplayRTCTime(u32 hour, u32 minute, u32 second)
{
	cmdLCD(GOTO_LINE1_POS0);
	charLCD((hour/10)+48);
	charLCD((hour%10)+48);
	charLCD(':');
	charLCD((minute/10)+48);
	charLCD((minute%10)+48);
	charLCD(':');
	charLCD((second/10)+48);
	charLCD((second%10)+48);
}

/*
Get the current RTC date
day Pointer to store the current date (1 31)
month Pointer to store the current month (1 12)
year Pointer to store the current year (four digits)
*/
void GetRTCDateInfo(s32 *date, s32 *month, s32 *year)
{
	*date = DOM;
	*month = MONTH;
	*year = YEAR;
}

/*
Display the RTC date on LCD
Day of month (1 31)
Month (1 12)
Year (four digits) and seperated by '/'
*/
void DisplayRTCDate(u32 date, u32 month, u32 year)
{
	cmdLCD(GOTO_LINE2_POS0);
	charLCD((date/10)+48);
	charLCD((date%10)+48);
	charLCD('/');
	charLCD((month/10)+48);
	charLCD((month%10)+48);
	charLCD('/');
	u32LCD(year);
}

/*
Set the RTC time
Hour to set (0 23)
Minute to set (0 59)
Second to set (0 59)
*/
void SetRTCTimeInfo(u32 hour, u32 minute, u32 second)
{
	HOUR = hour;
	MIN = minute;
	SEC = second;
}

/*
Set the RTC date
day of month to set (1 31)
month to set (1 12)
year to set (four digits)
*/
void SetRTCDateInfo(u32 date, u32 month, u32 year)
{
	DOM = date;
	MONTH = month;
	YEAR = year;
}

/*
Get the current day of the week
dow Pointer to store Day of Week (0=Sunday, ..., 6=Saturday)
*/
void GetRTCDay(s32 *day)
{
	*day=DOW;
}

/*
Display the current day of the week on LCD
dow (Day of Week) (0=Sunday, ..., 6=Saturday)
*/
void DisplayRTCDay(u32 day)
{
	cmdLCD(GOTO_LINE1_POS0+10);
	strLCD(week[day]);
}

/*
Set the day of the week in RTC
Day of Week to set (0=Sunday, ..., 6=Saturday)
*/
void SetRTCDay(u32 day)
{
	DOW=day;
}
//If editing required
void Edit_RTC_Info(void)
{
	while(1)
	{
	   cmdLCD(CLEAR_LCD);
	   delay_ms(1);
     strLCD("1hr 2min 3sec ");  //select
     cmdLCD(GOTO_LINE2_POS0);     //choice 
     strLCD("4dat 5mon 6year");		   //to edit
     delay_ms(400);
     cmdLCD(CLEAR_LCD);
     delay_ms(1);
	   strLCD("7day 8exit");
	   readnum(&num,&key);
	   cmdLCD(GOTO_LINE1_POS0);
	   u32LCD(num);
		//Go to task according to the choice
	   switch(num)
	   {
			   //Case 1 for Edit hour
		     case 1:cmdLCD(CLEAR_LCD);
                delay_ms(1);
		            strLCD("Enter hour");
		            readnum(&num,&key);
		            cmdLCD(GOTO_LINE2_POS0);
		            u32LCD(num);
		            delay_ms(100);
		            if(num<0||num>23)
		            {
			              cmdLCD(CLEAR_LCD);
                    delay_ms(1);;
			              strLCD("Invalid Input");
			              cmdLCD(GOTO_LINE2_POS0);
			              strLCD("retry request");
									  delay_ms(100);
			             break;
		            }
		            HOUR = num;
					      break;
				 //Case 2 for Edit minutes
		     case 2:cmdLCD(CLEAR_LCD);
                delay_ms(1);
		            strLCD("Enter minutes");
		            readnum(&num,&key);
		            cmdLCD(GOTO_LINE2_POS0);
		            u32LCD(num);
		            delay_ms(100);
		            if(num<0||num>59)
		            {
			             cmdLCD(CLEAR_LCD);
                   delay_ms(1);
			             strLCD("Invalid Input");
			             cmdLCD(GOTO_LINE2_POS0);
			             strLCD("retry request");
									 delay_ms(100);
		          	   break;
		            }
								MIN=num;
								break;
				 //Case 3 for Edit seconds
		     case 3:cmdLCD(CLEAR_LCD);
                delay_ms(1);
		            strLCD("Enter seconds");
		            readnum(&num,&key);
		            cmdLCD(GOTO_LINE2_POS0);
		            u32LCD(num);
		            delay_ms(100);
		            if(num<0||num>59)
		            {
			             cmdLCD(CLEAR_LCD);
                   delay_ms(1);
			             strLCD("Invalid Input");
			             cmdLCD(GOTO_LINE2_POS0);
			             strLCD("retry request");
									delay_ms(100);
		          	   break;
		            }
	              SEC=num;
					      break;
				 //Case 4 for Edit Date
		     case 4:cmdLCD(CLEAR_LCD);
               delay_ms(1);
		           strLCD("Enter Date");
		           readnum(&num,&key);
		           cmdLCD(GOTO_LINE2_POS0);
		           u32LCD(num);
		           delay_ms(100);
		           if(num<1||num>31)
		           {
			             cmdLCD(CLEAR_LCD);
                   delay_ms(1);
			             strLCD("Invalid Input");
			             cmdLCD(GOTO_LINE2_POS0);
			             strLCD("retry request");
								 delay_ms(100);
								 break;
		           }
		           DOM=num;
					     break;
				 //Case 5 for Edit Hour
		     case 5:cmdLCD(CLEAR_LCD);
               delay_ms(1);
		           strLCD("Enter Month");
		           readnum(&num,&key);
		           cmdLCD(GOTO_LINE2_POS0);
		           u32LCD(num);
		           delay_ms(100);
		           if(num<1||num>12)
		           {
			             cmdLCD(CLEAR_LCD);
                   delay_ms(1);
			             strLCD("Invalid Input");
			             cmdLCD(GOTO_LINE2_POS0);
			             strLCD("retry request");
			             break;
		           }
	             MONTH=num;
					     break;
				 //Case 6 for Edit Year
		     case 6:cmdLCD(CLEAR_LCD);
               delay_ms(1);
		           strLCD("Enter Year");
		           readnum(&num,&key);
		           cmdLCD(GOTO_LINE2_POS0);
		           u32LCD(num);
		           delay_ms(100);
	             YEAR=num;
							 break;
					//Case 7 for Edit Day
		      case 7:cmdLCD(CLEAR_LCD);
               delay_ms(1);
		           strLCD("Enter day");
		           readnum(&num,&key);
		           cmdLCD(GOTO_LINE2_POS0);
		           u32LCD(num);
		           delay_ms(100);
		           if(num<0||num>6)
		           {
			             cmdLCD(CLEAR_LCD);
                   delay_ms(1);
			             strLCD("Invalid Input");
			             cmdLCD(GOTO_LINE2_POS0);
			             strLCD("retry request");
			             break;
		           }
							 DOW=num;
							 break;
					//Case 8 for Exit from Editing and go to Display menu
				  case 8:cmdLCD(CLEAR_LCD);
                 delay_ms(1);
		             return;
				//If user Entered the Invalid choice
		    default: cmdLCD(CLEAR_LCD);
                delay_ms(1);
			          strLCD("Invalid choice");
							  delay_ms(100);
		 }
	}
}
//Display the request for choice
void Display_menu(void)
{
 while(1)
 {
	cmdLCD(CLEAR_LCD);
	delay_ms(1);
	 //If editing required by the user
	strLCD("1.EDIT RTC INFO");
	cmdLCD(GOTO_LINE2_POS0);
	 //Exit from the Display menu
	strLCD("2.EXIT");
	readnum(&num,&key);
	if(num==1)
	{
		//If editing is required
		Edit_RTC_Info();
	}
	else if(num==2)
	{
		//To exit from the Display menu
		cmdLCD(0x01);
		break;
	}
	else
	{
		//If invalid choice entered by the user
		cmdLCD(CLEAR_LCD);
		delay_ms(1);
		strLCD("Invalid choice");
		delay_ms(100);
	}
  }
}
//Interrupts
void eint0_isr(void)__irq
{
	request=1;
	//clear the interrupt
	EXTINT=1<<0;
	VICVectAddr=0;
}
void enable_eint0(void)
{
	// p0.16 is cfg as eint0
	  PINSEL1|=(1<<0);
    PINSEL1&=~(1<<1);
	//select the type of the interrupt irq/fiq
	VICIntSelect=0;//irq type
	//enable the eint0
	VICIntEnable=1<<EINT0;
	//set the enable bit and channel binary 
	VICVectCntl0=(1<<5)|EINT0;
	//assign the ISR address
	VICVectAddr0=(unsigned int)eint0_isr;
	//edge triggereing
	EXTMODE=1<<0;
	//falling edge
	//EXTPOLAR=0<<0;
}

//ADC Initialization
void Init_ADC(void)
{
	  cfgportpinfun(0,27,1);
	  //PINSEL1|=0X15400000;//Configuring function 1
	  ADCR=(1<<PDN_BIT)|(CLKDIV<<CLKDIV_BITS);//To set pindown bit,Clock divider value in clock divider bits
}
//To get the Approxmate analog value
float Get_analog_value(void)
{
	float eAR;//Approximate analog value
	unsigned int ADCVal;//Approximate Digital value
	ADCR&=0xffffff00;//This is for without effecting the pins other than channels
	ADCR|=(1<<START_CONVERTION)|(1<<CH0);//This is for the to start ADC convertion and Select the channel
	delay_us(3);
	while(((ADDR>>DONE_BIT)&1)==0);//Wait untill the convertion completed
	ADCR&=~(1<<START_CONVERTION);//To reset(stop) Start convertion
	ADCVal=((ADDR>>ADC_RESULT)&1023);//To get the 10 bit ADC value
	eAR=(ADCVal*(3.3/1023));//To get the approx Analog value
	return eAR;
}

//Main for the Operating Street Lights
int main()
{
	//To Initialize the LCD
	InitLCD();
	//To Initialize the RTC
	RTC_Init();
	//To Initialize the KPM
	InitKPM();
	////To Initialize the Street Lights(LED's)
	InitLEDS();
	//To Initialize the ADC
	Init_ADC();
	//If External Interrupt raised
	enable_eint0();
	//To set the default values in the RTC for example Working
	SetRTCDateInfo(7,8,2003);
	SetRTCTimeInfo(17,59,45);
	SetRTCDay(4);
	while(1)
	{   
		  if(request)//If request raised enters this if 
			{
				request=0;
				Display_menu();//To Display the menu About the request raised
			}
		  GetRTCTimeInfo(&hour,&min,&sec);//To get the Hours,Minutes,Seconds from RTC registers
		  GetRTCDateInfo(&date,&month,&year);//To get the Date,Month,Year from RTC registers
		  GetRTCDay(&day);//To get the Day from RTC register
		  DisplayRTCTime(hour,min,sec);//To Display the Hours,Minutes,Seconds from User variables
		  DisplayRTCDate(date,month,year);//To Display the Date,Month,Year from User Variables
		  DisplayRTCDay(day);//To Display the Day from user day variable
			eAR=Get_analog_value();
			cmdLCD(GOTO_LINE2_POS0+13);
			F32LCD(eAR,3);
			if(((hour>=18)||(hour<=6))&&((eAR)>setval))
		  {
					WRITEBYTE(IOPIN0,LED_OPINS,(0x0F^255));
			}
			else
			{
				//initially off
				WRITEBYTE(IOPIN0,LED_OPINS,(0xF0^255));
			}
	}
}
