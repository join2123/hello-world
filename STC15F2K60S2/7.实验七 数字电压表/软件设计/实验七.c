/////////////////////////////////////////////////////////////////////////////////
//设计项目:数字电压表_中断实验_adc实验                                            //
//设 计 人:畅福善                                                              //
//设计时间:2016.6.8                                                           //
//设计说明:设计一个开关计数器， 实现一个模拟开关连续计数，控制8个LED指示灯显示 //
//         利用两位数码管显示，增加数据报警功能，和按键取消报警                //
//         硬件连接条件:1、单片机P0.0~P0.7依次连接第1~8个LED指示灯             //
//                      2、4个模拟开关依次连接到P3.4,P3.5,P3.6,P3.7            //
//                      3、数码管数据连接IC7 锁存控制P2.6                      //
//                      4、数码管数据连接IC6 锁存控制P2.7                      //
//                      5、P1.1为10位ADC输入端                                 //
/////////////////////////////////////////////////////////////////////////////////

//#include <AT89X51.H> 
#include  "STC15F2K60S2.H"

////////函数说明////////
void delay02s(void);		//延时2秒子程序//
void delay10ms(void);		//延时10ms的子程序//
void delay50us(void);		//延时50us的子程序//
void delay500us(void);		//延时500us的子程序//
void key(void);             //按键函数
void display(void);         //显示函数
void alarm_buzzer(void);		//交流蜂鸣器自检程序//
//void display_seg(void);     //数码管显示函数 
void baojing(void);         //报警函数 
void int_time0(void);       //t0定时器初始化函数
void int_time1(void);       //t1定时器初始化函数
void display_seg_v327(void);   //v3.27板显示函数
void display_seg_HC6800(void); //HC6800板显示函数
void ADC_init(void); //ADC初始化函数
////////初始化函数////////
void FMQ_init(void);      //蜂鸣器初始化函数
void SEG_init(void);      //数码管初始化函数
void LED_init(void);      //LED初始化函数
void key_init(void);      //按键初始化函数
////////SFR定义////////
//led定义
sbit L1=P1^0;//////定义第1个指示灯的名称为L1  还可以直接采用P0_0;因为AT89X51.H内部已做定义,下依次类推
sbit L2=P1^1;//////定义第2个指示灯的名称为L2 
sbit L3=P1^2;//////定义第3个指示灯的名称为L3  
sbit L4=P1^3;//////定义第4个指示灯的名称为L4 
sbit L5=P1^4;//////定义第5个指示灯的名称为L5  
sbit L6=P1^5;//////定义第6个指示灯的名称为L6 
sbit L7=P1^6;//////定义第7个指示灯的名称为L7  
sbit L8=P1^7;//////定义第8个指示灯的名称为L8 
//按键定义
sbit key_1=P3^4;//////定义第1个按键为KEY_1
sbit key_2=P3^5;//////定义第1个按键为KEY_2
sbit key_3=P4^2;//////定义第1个按键为KEY_3
sbit key_4=P4^4;//////定义第1个按键为KEY_4
//锁存器定义
sbit duan_clc=P2^6;//////定义段锁存器(IC7)时钟脉冲
sbit wen_clc=P2^7;//////定义位锁存器(IC6)时钟脉冲
//蜂鸣器定义
sbit FMQ=P3^2;//////定义蜂鸣器单片机驱动口
//////////////变量定义///////////////
unsigned  char data adc_data_h _at_ 0x30;/////////STC_ADC保存绝对地址变量
unsigned  char data adc_data_l _at_ 0x31;/////////STC_ADC保存绝对地址变量
unsigned long sum_AD;/////////AD和
unsigned int sum_AD_JSQ;/////////AD滤波计数器
float XDSD;////////相对湿度值
unsigned int XDSD_xs;////////相对湿度值

unsigned  int count;/////////按键计数器
unsigned  int AD1_count,AD2_ount,AD3_ount,AD4_ount;/////////4路AD值

unsigned  char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40,0x00};////数码管显示码表
//////////////////////////////0     1    2    3    4    5    6    7    8    9	A    B     C    D    E    F	  -    		
////////////// 带小数点//////////////////
unsigned  char code table_dot[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40,0x00};////数码管显示码表
///////////////////////////////////0.  1.   2.   3.    4.   5.  6.   7.   8.   9.	A    B     C    D    E    F	  -    				 
unsigned  char shiwei;//10位数据
unsigned  char gewei;//个位数据                            	
unsigned  char SEG_1,SEG_2,SEG_3,SEG_4,SEG_5,SEG_6,SEG_7,SEG_8;//////8个数码管显示单元定义
bit flag_baojing;///////定义报警标志

unsigned char jsq_50ms,jsq_1s;///////50ms计数器和1S计数器
unsigned char jsq_1min,jsq_1hours;///////1分计数器和1小时计数器

unsigned int jsq_fmq;

void main(void) 			//主程序//
{
/////////初始化区////////////////
//FMQ_init();////////蜂鸣器初始化、自检    
//int_time0();/////////定时器1初始化////////////////
	LED_init();

	SEG_init();
	//alarm_buzzer();		//交流蜂鸣器自检程序//
	ADC_init();/////////ADC初始化//////////////// 
while(1) 			    //进入循环//以下为无限循环程序区
     	{ 
////////////////利用中断实现10位ADC//////////////////////////////////////

////////////////利用中断实验时间与数码管的关系,其中采用中断函数计数//////////////////////////////////////
display_seg_v327();   //v3.27板显示函数
//display_seg_HC6800();   //HC6800板显示函数
ADC_CONTR=0X89;/////重新启动转换1000,1001B
    } 

}

///////////////////////////////////////////////////
//函数名称:void key(void)
//函数功能:按键函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void key(void)      //按键函数
{
	if(key_1==0)/////////如果按键按下
	{
	  delay10ms();///////延时10ms
	  if(key_1==0)///////按键的确按下
	    {
		count++;///////按键计数器计数		
		if(count>=16){count=0;}//////控制计数器最大值;16即为16进制;20即为20进制;但最大计数255，因为count定义的是unsigend char
		if(count>=10)
			{
			flag_baojing=1;//////计数器超过10给出报警标志;
			}
		else
			{
			flag_baojing=0;//////计数器未超过10取消报警标志;
			}
//		P0=~count;///////将计数值送P0口利用LED指示灯以BCD码（8421码）显示
		while(key_1==0){;}///////按键等待抬起
		}
	}
	if(key_2==0)/////////如果按键按下
	{
	  delay10ms();///////延时10ms
	  if(key_2==0)///////按键的确按下
	    {
		flag_baojing=0;//////手动取消报警标志;
		while(key_2==0){;}///////按键等待抬起
		}
	}
}


///////////////////////////////////////////////////
//函数名称:void display_seg_v327(void)      //v3.27板显示函数
//函数功能:数码管显示函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void display_seg_v327(void)      //v3.27板显示函数
{
//count=7;
SEG_1=17;
SEG_2=17;
SEG_3=17;
SEG_4=17;
//SEG_5=7;
//SEG_6=2;
//SEG_7=5;
//SEG_8=6;
//////////////显示数据处理程序/////////////////////////////

//SEG_5=count/1000;//得到1000位数据
//SEG_6=(count%1000)/100;//得到个位数据100
//SEG_7=((count%1000)%100)/10;//得到10位数据
//SEG_8=((count%1000)%100)%10;//得到个位数据
//////////////显示程序/////////////////////////////

/////////1位显示//////////////////////
P0=table[SEG_1];//////数据从P0口送出到IC7---74HC573锁存器
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x01;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存

/////////2位显示//////////////////////
P0=table[SEG_2];//////数据从P0口送出到IC7---74HC573锁存器
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x02;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
/////////3位显示//////////////////////
P0=table[SEG_3];//////数据从P0口送出到IC7---74HC573锁存器
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x04;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
/////////4位显示//////////////////////
P0=table[SEG_4];//////数据从P0口送出到IC7---74HC573锁存器
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x08;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
		///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存

/////////5位显示//////////////////////
P0=table[SEG_5];//////数据从P0口送出到IC7---74HC573锁存器
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x10;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存

/////////6位显示//////////////////////
//P0=table[SEG_6];//////数据从P0口送出到IC7---74HC573锁存器

P0=table_dot[SEG_6];//////数据从P0口送出到IC7---74HC573锁存器---------------使用带小数点
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x20;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
/////////7位显示//////////////////////
P0=table[SEG_7];//////数据从P0口送出到IC7---74HC573锁存器
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x40;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
/////////8位显示//////////////////////
P0=table[SEG_8];//////数据从P0口送出到IC7---74HC573锁存器
duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
P0=~0x80;//////位码从P0口送出到IC6---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
//	delay10ms();///延时0.2s
	delay500us();///延时0.5ms
		///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
}
///////////////////////////////////////////////////
//函数名称:void display_seg_HC6800(void)
//函数功能:数码管显示函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void display_seg_HC6800(void)      //显示函数
{
//count=7;
//SEG_1=4;
//SEG_2=3;
//SEG_3=8;
//SEG_4=9;
//SEG_5=7;
//SEG_6=2;
//SEG_7=5;
//SEG_8=6;
//////////////显示数据处理程序/////////////////////////////

//SEG_5=count/1000;//得到1000位数据
//SEG_6=(count%1000)/100;//得到个位数据100
//SEG_7=((count%1000)%100)/10;//得到10位数据
//SEG_8=((count%1000)%100)%10;//得到个位数据
//////////////显示程序/////////////////////////////

/////////1位显示//////////////////////
P0=table[SEG_1];//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=1;P24=1;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0X00;//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=1;P24=1;///U4位选Y1

/////////2位显示//////////////////////
P0=table[SEG_2];//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=1;P24=1;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0X00;//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=1;P24=1;///U4位选Y1
	///去鬼影//

/////////3位显示//////////////////////
P0=table[SEG_3];//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=0;P24=1;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=0;P24=1;///U4位选Y1

/////////4位显示//////////////////////
P0=table[SEG_4];//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=0;P24=1;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=0;P24=1;///U4位选Y1

/////////5位显示//////////////////////
P0=table[SEG_5];//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=1;P24=0;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=1;P24=0;///U4位选Y1

/////////6位显示//////////////////////
P0=table_dot[SEG_6];//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=1;P24=0;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=1;P24=0;///U4位选Y1

/////////7位显示//////////////////////
P0=table[SEG_7];//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=0;P24=0;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
P22=1;P23=0;P24=0;///U4位选Y1

/////////8位显示//////////////////////
P0=table[SEG_8];//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=0;P24=0;///U4位选Y1
delay10ms();
	delay500us();///延时0.5ms
	///去鬼影//
P0=0X00;//////数据从P0口送出到IC7---74HC573锁存器
P22=0;P23=0;P24=0;///U4位选Y1
/*
/////////1位显示//////////////////////

P0=table[SEG_8];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=0;P23=0;P24=0;///138倒数第1位
     delay500us();///延时500us

/////////2位显示//////////////////////
P0=table[SEG_7];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=1;P23=0;P24=0;///138倒数第2位
     delay500us();///延时500us
/////////3位显示//////////////////////
P0=table[SEG_6];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=0;P23=1;P24=0;///138倒数第3位
     delay500us();///延时500us

/////////4位显示//////////////////////
P0=table[SEG_5];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=1;P23=1;P24=0;///138倒数第4位
     delay500us();///延时500us

/////////5位显示//////////////////////
P0=table[SEG_4];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=0;P23=0;P24=1;///138倒数第5位
     delay500us();///延时500us
/////////6位显示//////////////////////
P0=table[SEG_3];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=1;P23=0;P24=1;///138倒数第6位
     delay500us();///延时500us

/////////7位显示//////////////////////
P0=table[SEG_2];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=0;P23=1;P24=1;///138倒数第7位
     delay500us();///延时500us

/////////8位显示//////////////////////
P0=table[SEG_1];//////数据从P0口送出到IC7---74HC573锁存器
P10=0;P10=1;P10=0;///IC7数据锁存
P22=1;P23=1;P24=1;///138倒数第8位
     delay500us();///延时500us
*/
}

///////////////////////////////////////////////////
//函数名称:void delay500us(void)
//函数功能:延时500us秒的子程序
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void delay500us(void)		//延时10ms的子程序//
{ 		
		unsigned char k; /////局部变量定义
					{
				  		for(k=248;k>0;k--) ////内层循环
							{
							;
							}
					}
}  
///////////////////////////////////////////////////
//函数名称:void delay10ms(void)
//函数功能:延时10ms秒的子程序
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void delay10ms(void)		//延时10ms的子程序//
{ 		
//		unsigned char i,j,k; /////局部变量定义
		unsigned char j,k; /////局部变量定义
//  		for(i=20;i>0;i--)   //最外层循环
//			{
		    	for(j=20;j>0;j--)   // 上层循环
					{
				  		for(k=248;k>0;k--) ////内层循环
							{
							;
							}
					}
//			}
} 
 
///////////////////////////////////////////////////
//函数名称:void delay02s(void)
//函数功能:延时0.2秒的子程序
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void delay02s(void)		//延时子程序//
{ 		
		unsigned char i,j,k; /////局部变量定义
  		for(i=20;i>0;i--)   //最外层循环
			{
		    	for(j=20;j>0;j--)   // 上层循环
					{
				  		for(k=248;k>0;k--) ////内层循环
							{
							;
							}
					}
			}
} 
///////////////////////////////////////////////////
//函数名称:void int_time0(void)
//函数功能:定时器t0初始化
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void int_time0(void) 
{
	//设置定时器0工作方式//
	TMOD=0x01;								//定义为定时器0方式1,SFR中TMOD位于不能被8整除的地址,所以要按字节方式赋值//
	//赋值方法1//
	TH0=(65536-100)/256;					//赋初值//
	TL0=(65536-100)%256;
	//打开中断通道//
	ET0=1;									//开启定时器0溢出中断//
	EA=1;									//开启总的中断开关//


///////开表/////
	TR0=1;									//开启定时器0//

}

///////////////////////////////////////////////////
//函数名称:void	interrupt_t0(void)	interrupt	1
//函数功能:t0中断函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void	interrupt_t0(void)	interrupt	1		//定时器0溢出中断//
{
////////////赋初值//////////////////////////////////
	//赋初值方法1//
	TH0=(65536-100)/256;					//赋初值//
	TL0=(65536-100)%256;

			if(SEG_6>=2)
			{
			FMQ=!FMQ;
			}


}


///////////////////////////////////////////////////
//函数名称:void	ADC_init(void)
//函数功能:ADC初始化
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void	ADC_init(void)	
{
	unsigned int i; /////局部变量定义
/////////////////分配模拟量输入口///////////////////////////
P1ASF=0X02;/////设置P1.1为ADC输入0000,0010B
//P1ASF=P1ASF|0X02;/////设置P1.1为ADC输入
//P1ASF=0X0f;/////设置P1.0~P1.3为ADC输入
/////////////////ADC控制///////////////////////////
ADC_CONTR=0X81;//1000,0001B打开AD转换电源，设置输入通道为P1.1、转换速度为一次90个时钟周期、转换标志清零
for(i=0;i<10000;i++);///1ms适当延时，延时等待ADC通道稳定，理论20~200us
/////////////////设置ADC转换结果输入格式///////////////////////////
CLK_DIV |=0X20;////ADRJ=1;设置AD转换结果的输入格式为右对齐0000,0010B
/////////////////打开中断通道///////////////////////////
EADC=1;////
EA=1;////
/////////////////启动ADC转换///////////////////////////
ADC_CONTR=0X89;///启动转换1000,1100B



}

///////////////////////////////////////////////////
//函数名称:void	interrupt_ADC(void)	interrupt	5
//函数功能:t0中断函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void	interrupt_ADC(void)	interrupt	5		
{
/////////////////ADC控制-----其主要目的是清标志///////////////////////////
ADC_CONTR=0X81;//将ADC_FLAG清零1000,0001B

/////////////////ADC结果处理-----导出数据高低位///////////////////////////
adc_data_h=ADC_RES&0X03;///ADC高位值
adc_data_l=ADC_RESL;///ADC低位值
/////////////////ADC结果处理-----计算ADC值///////////////////////////
AD1_count=adc_data_h*256+adc_data_l;

////////////////数据处理/////////////////////////////

////////////////平均滤波/////////////////////////////
sum_AD_JSQ++;

if(sum_AD_JSQ<=1024){sum_AD=AD1_count+sum_AD;}

//else
if(sum_AD_JSQ>1024)
{
//sum_AD=1234;
sum_AD=sum_AD/1024;

sum_AD_JSQ=0;//sum_AD=0;XDSD=0;


////////////////ADC数据转换为0~5v电压/////////////////////////////
sum_AD=(1000*sum_AD)/1024;
if(sum_AD>350)
{
	//alarm_buzzer();
}
////////////////ADC数据显示/////////////////////////////

////////////////四联数码管显示数据准备////////////////////////////////////////////////////
//sum_AD=1234;
//count=sum_AD;

SEG_5=sum_AD/1000;//得到1000位数据
if(SEG_5==0){
SEG_5=17;
}
SEG_6=(sum_AD%1000)/100;//得到个位数据100
SEG_7=((sum_AD%1000)%100)/10;//得到10位数据
SEG_8=((sum_AD%1000)%100)%10;//得到个位数据
}
///////////////新的转换开始////////////////////////////////////////////////////
ADC_CONTR=0X8C;/////重新启动转换

}
///////////////////////////////////////////////////
//函数名称:void key_init(void)
//函数功能:按键初始化函数
//入口参数:
//出口参数:
//函数说明:P3口是准双向口所以准备读取按键（按键输入）前要向P3口写“1”关闭P3口接地的场效应
///////////////////////////////////////////////////
void key_init(void)      //按键初始化函数
{
P3=0xff;/////初始化P3口准备读取按键（按键输入）
}
///////////////////////////////////////////////////
//函数名称:void LED_init(void)
//函数功能:LED初始化、自检函数
//入口参数:
//出口参数:
//函数说明:采用全部显示的方法解决初始化问题
///////////////////////////////////////////////////
void LED_init(void)      //LED初始化函数
{
//////////////多次闪烁显示程序/////////////////////////////
		unsigned char i; /////局部变量定义
  		for(i=3;i>0;i--)   //循环闪烁
			{
				L1=0;L2=0;L3=0;L4=0;L5=0;L6=0;L7=0;L8=0;
				delay02s();///延时0.2s
				delay02s();///延时0.2s
				delay02s();///延时0.2s
				L1=1;L2=1;L3=1;L4=1;L5=1;L6=1;L7=1;L8=1;
				delay02s();///延时0.2s
				delay02s();///延时0.2s
				delay02s();///延时0.2s
			}
}
///////////////////////////////////////////////////
//函数名称:void SEG_init(void)
//函数功能:数码管初始化、自检函数
//入口参数:
//出口参数:
//函数说明:采用全部显示的方法解决初始化问题
///////////////////////////////////////////////////
void SEG_init(void)      //数码管初始化函数
{
//////////////持续显示程序/////////////////////////////
		unsigned char i; /////局部变量定义
  		for(i=9;i>0;i--)   //最外层循环
			{
			//SEG_1=i,SEG_2=i,SEG_3=i,SEG_4=i,SEG_5=i,SEG_6=i,SEG_7=i,SEG_8=i;
			SEG_1=i;			
			P0=table[SEG_1];//////数据从P0口送出到IC7---74HC573锁存器
			duan_clc=0;duan_clc=1;duan_clc=0;///IC7数据锁存
			P0=~0xFF;//////位码从P0口送出到IC6---74HC573锁存器--------数码管全亮
			wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
			//	delay10ms();///延时0.2s
				delay02s();///延时0.2s
				delay02s();///延时0.2s
				delay02s();///延时0.2s
				delay02s();///延时0.2s
				delay02s();///延时0.2s

				///去鬼影//
			P0=0XFF;//////数据从P0口送出到IC7---74HC573锁存器
			wen_clc=0;wen_clc=1;wen_clc=0;///IC6位码数据锁存
			///display_seg();

			}
			//SEG_1=0,SEG_2=16,SEG_3=16,SEG_4=16,SEG_5=16,SEG_6=16,SEG_7=16,SEG_8=16;//单个显示0，说明数码管准备完成
			SEG_1=0;
}

///////////////////////////////////////////////////
//函数名称:void alarm_buzzer(void)
//函数功能:交流蜂鸣器自检程序
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void alarm_buzzer(void)		//交流蜂鸣器自检程序//
  { 		
	unsigned char k; /////局部变量定义
	for(k=248;k>0;k--) ////内层循环
		{
		FMQ=1;
		delay50us();
		FMQ=0;
		delay50us();
		}
		FMQ=1;
  }  
///////////////////////////////////////////////////
//函数名称:void delay50us(void)
//函数功能:延时50us秒的子程序
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void delay50us(void)		//延时50us的子程序//
  { 		
///////当下载时不选择使用内部时钟请利用显示程序实现延时，数码管就不抖动了//////////////////
	unsigned int k; /////局部变量定义
	for(k=1;k>0;k--) ////内层循环
		{
		display_seg_v327();
		//display_seg_HC6800();   //HC6800板显示函数

		}
////////////当下载时选择使用内部时钟请使用下延时//////////////////
//	unsigned int k; /////局部变量定义
//	for(k=50;k>0;k--) ////内层循环
//		{
//		;
//		}
/////////////////////////////////
////////////当下载时选择不使用内部时钟请使用下延时，报警但数码管闪烁//////////////////
//	unsigned int k; /////局部变量定义
//	for(k=500;k>0;k--) ////内层循环
//		{
//		;
//		}
/////////////////////////////////
  }  

