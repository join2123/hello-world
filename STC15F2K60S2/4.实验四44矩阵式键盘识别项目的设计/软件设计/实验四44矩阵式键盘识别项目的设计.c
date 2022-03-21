/////////////////////////////////////////////////////////////////////////////////
//设计项目:4x4控制数码管设计实验                                               //
//设 计 人:畅福善                                                              //
//设计时间:2014.5.5                                                            //
//设计说明:设计一个开关计数器， 实现一个4x4键盘，控制数码管显示                //
//         利用数码管依次显示，1 2 3 4 5 6 7 8 9 A B C D E F                   //
//         硬件连接条件:1、单片机P0.0~P0.7依次连接第1~8个LED指示灯             //
//                      2、4个模拟开关依次连接到P3.4,P3.5,P3.6,P3.7            //
//                      3、数码管数据连接IC7 锁存控制P2.6                      //
//                      4、数码管数据连接IC6 锁存控制P2.7                      //
/////////////////////////////////////////////////////////////////////////////////

//#include <AT89X51.H> 
//#include "STC89C52RC.H"
#include "stc15f2k60s2.h"
////////函数说明////////
void delay02s(void);		//延时2秒子程序//
void delay10ms(void);		//延时10ms的子程序//
void delay500us(void);		//延时500us的子程序//
void delay50us(void);		//延时50us的子程序//

void key(void);             //按键函数
void key44(void);           //4*4按键函数

void display(void);         //二极管8421显示函数
void display_seg(void);     //数码管显示函数 
void display_seg1(void);     //34个、数码管显示函数 

void baojing(void);         //报警函数 
void alarm_buzzer(void);	//按键音

void display_ceshi(void);	//2个数码管自检程序
void display_ceshi_8(void); //8个数码管自检程序
void display_seg_8(void);
void ceshiyin(void);       //蜂鸣器自检
void jiafa(void);          //加法
void addition_calculators(void);//加法计算器

////////变量说明////////
//led定义
sbit L1=P1^0;//////定义第1个指示灯的名称为L1  还可以直接采用P0_0;因为AT89X51.H内部已做定义,下依次类推
sbit L2=P1^1;//////定义第2个指示灯的名称为L2 
sbit L3=P1^2;//////定义第3个指示灯的名称为L3  
sbit L4=P1^3;//////定义第4个指示灯的名称为L4 
sbit L5=P1^4;//////定义第5个指示灯的名称为L5  
sbit L6=P1^5;//////定义第6个指示灯的名称为L6 
sbit L7=P1^6;//////定义第7个指示灯的名称为L7  
sbit L8=P1^7;//////定义第8个指示灯的名称为L8 
//独立按键定义
sbit key_1=P3^4;//////定义第1个独立按键为KEY_1
sbit key_2=P3^5;//////定义第2个独立按键为KEY_2
sbit key_3=P3^6;//////定义第3个独立按键为KEY_3
sbit key_4=P3^7;//////定义第4个独立按键为KEY_4
//4*4按键定义
sbit line_1=P2^0;//////定义第1行
sbit line_2=P2^1;//////定义第2行
sbit line_3=P2^2;//////定义第3行
sbit line_4=P2^3;//////定义第4行

sbit row_1=P3^4;//////定义第1列
sbit row_2=P3^5;//////定义第2列
sbit row_3=P4^2;//////定义第3列
sbit row_4=P4^4;//////定义第4列
///////////////////////////////////////
sbit fmq=P3^2;//////定义fmq为蜂鸣器控制端
/////////////////////////////////////////
unsigned  char count;/////////按键计数器
unsigned  char w;
unsigned  char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40,0x00,0x41};////数码管显示码表
//////////////////////////////0     1    2    3    4    5    6    7    8    9	A    B     C    D    E    F	  -   
unsigned  char SEG_1,SEG_2,SEG_3,SEG_4,SEG_5,SEG_6,SEG_7,SEG_8;//////8个数码管显示单元定义
///////////////0     1      2      3     4     5    6     7    						 
unsigned  char shiwei;//10位数据
unsigned  char gewei;//个位数据                            	

unsigned  char temp_i;//分析按键临时变量 
unsigned  char seg_key;//数码管临时按键显示变量 
bit flag_baojing;///////定义报警标志

void main(void) 			//主程序//
{   
/////////初始化区////////////////
display_ceshi();
display_ceshi_8();
ceshiyin();


//////////IO口输入初始化////////////////////
line_1=1;line_2=1;line_3=1;line_4=1;row_1=1;row_2=1;row_3=1;row_4=1;
//////////数码管初始化////////////////////
///数码管自检
//////////蜂鸣器初始化////////////////////

	while(1) 			    //进入循环//以下为无限循环程序区
     	{ 
		 key44(); 			//调用4*4按键函数/2/
		 //display();         //调用显示函数//
		 display_seg();     //调用位数码管显示函数
		 //addition_calculators();
        } 
}

void jiafa(void)
{
	key44();
	
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
//函数名称:void key44(void)
//函数功能:按键函数
//函数说明:一种4*4键盘
//4*4按键定义
//sbit line_1=P2^0;//////定义第1行
//sbit line_2=P2^1;//////定义第2行
//sbit line_3=P2^2;//////定义第3行
//sbit line_4=P2^3;//////定义第4行
//
//sbit row_1=P3^4;//////定义第1列
//sbit row_2=P3^5;//////定义第2列
//sbit row_3=P4^2;//////定义第3列
//sbit row_4=P4^4;//////定义第4列
//
//sbit fmq=P3^2;//////定义fmq为蜂鸣器控制端
///////////////////////////////////////////////////
void key44(void)
{
line_1=1;line_2=1;line_3=1;line_4=1;row_1=1;row_2=1;row_3=1;row_4=1;
/////////////////////扫描第一行////////////////////////////////////////	
	line_1=0;							    //将第一行P20置0，扫描第一行4列
//扫描第一列
	if(row_1==0)							//扫描第一列P34是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_1==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=7;						//识别第一行第一列，并给出数码管显示内容
		}
		while(row_1==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第二列	
	if(row_2==0)							//扫描第二列P35是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_2==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=8;						//识别第二列，并给出数码管显示内容
		}
		while(row_2==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第三列	
	if(row_3==0)							//扫描第三列P42是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_3==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=9;						//识别第三列，并给出数码管显示内容
		}
		while(row_3==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;
//扫描第四列	
	if(row_4==0)							//扫描第四列P44是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_4==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=10;						//识别第四列，并给出数码管显示内容
		}
		while(row_4==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;	

	line_1=1;							//第一行扫描检测完毕，将P20置1
	
/////////////////////扫描第二行////////////////////////////////////////	
	line_2=0;							    //将第二行P21置0，扫描第二行4列
//扫描第一列
	if(row_1==0)							//扫描第一列P34是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_1==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=4;						//识别第一列，并给出数码管显示内容
		}
		while(row_1==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第二列	
	if(row_2==0)							//扫描第二列P35是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_2==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=5;						//识别第二列，并给出数码管显示内容
		}
		while(row_2==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第三列	
	if(row_3==0)							//扫描第三列P42是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_3==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=6;						//识别第三列，并给出数码管显示内容
		}
		while(row_3==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;
//扫描第四列	
	if(row_4==0)							//扫描第四列P44是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_4==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=11;						//识别第四列，并给出数码管显示内容
		}
		while(row_4==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;	

	line_2=1;							//第二行扫描检测完毕，将P21置1	
/////////////////////扫描第三行////////////////////////////////////////	
	line_3=0;							    //将第三行P22置0，扫描第三行4列
//扫描第一列
	if(row_1==0)							//扫描第一列P34是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_1==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=1;						//识别第一列，并给出数码管显示内容
		}
		while(row_1==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第二列	
	if(row_2==0)							//扫描第二列P35是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_2==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=2;						//识别第二列，并给出数码管显示内容
		}
		while(row_2==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第三列	
	if(row_3==0)							//扫描第三列P42是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_3==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=3;						//识别第三列，并给出数码管显示内容
		}
		while(row_3==0){display_seg();	}				//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;
//扫描第四列	
	if(row_4==0)							//扫描第四列P44是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_4==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=12;						//识别第四列，并给出数码管显示内容
		}
		while(row_4==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;	

	line_3=1;							//第三行扫描检测完毕，将P22置1
/////////////////////扫描第四行////////////////////////////////////////	
	line_4=0;							    //将第四行P23置0，扫描第三行4列
//扫描第一列
	if(row_1==0)							//扫描第一列P34是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_1==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=0;						//识别第一列，并给出数码管显示内容
		}
		while(row_1==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第二列	
	if(row_2==0)							//扫描第二列P35是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_2==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=15;						//识别第二列，并给出数码管显示内容
		}
		while(row_2==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;									//按键声音结束
//扫描第三列	
	if(row_3==0)							//扫描第三列P42是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_3==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=14;						//识别第三列，并给出数码管显示内容
		}
		while(row_3==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;
//扫描第四列	
	if(row_4==0)							//扫描第四列P44是否被按下
	{
		delay10ms();						//延时10ms，消抖
		if(row_4==0)						//重新检测
		{			
			//fmq=0;
			alarm_buzzer();			//按键音
			seg_key=13;						//识别第四列，并给出数码管显示内容
		}
		while(row_4==0){display_seg();}					//在按键按下时，让程序停在这里，等待弹起
	}//fmq=1;	

	line_4=1;							//第四行扫描检测完毕，将P22置1
		///////数码管显示内容准备///////
		SEG_1=16;SEG_2=16;SEG_3=16;SEG_4=16;SEG_5=16;SEG_6=16;SEG_7=16;SEG_8=16;//8个数码管显示单元送无数据数//
		SEG_2=seg_key;//利用1位数码管显示键盘数字或字符//,可调可调整到数码管的任意位置	
}
///////////////////////////////////////////////////
//函数名称:void baojing(void)
//函数功能:报警函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void baojing(void)
{
unsigned  int baojing_i;
		  for(baojing_i=100;baojing_i>0;baojing_i--)
		   {
		    P14=~P14;               //P1_0取反//
			delay500us();            //延时500us//
		   }
}




///////////////////////////////////////////////////
//函数名称:void display(void)
//函数功能:显示函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void display(void)      //LED显示函数
{
//////////////持续显示程序/////////////////////////////
	P1=~count;///////将计数值送P0口利用LED指示灯以BCD码（8421码）显示
	delay02s();
//////////////闪烁显示函数/////////////////////////////
/*
	P0=~count;///////将计数值送P0口利用LED指示灯以BCD码（8421码）显示
	delay02s();///延时0.2s
	P0=0xff;///////P0口8条线全送高电平，目的熄灭所有LED指示灯
	delay02s();///延时0.2s
*/
}
///////////////////////////////////////////////////
//函数名称:void display_seg(void)
//函数功能:数码管显示函数
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void display_seg(void)      //显示函数
{
	
//SEG_8=seg_key%10;
//SEG_7=seg_key/10;	
/////////1位显示//////////////////////
P0=table[SEG_1];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x10;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
/////////2位显示//////////////////////
P0=table[SEG_2];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x20;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
/////////3位显示//////////////////////
P0=table[SEG_3];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x40;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
/////////4位显示//////////////////////
P0=table[SEG_4];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x80;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
/////////5位显示//////////////////////
P0=table[SEG_5];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x01;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
/////////6位显示//////////////////////
P0=table[SEG_6];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x02;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
/////////7位显示//////////////////////
P0=table[SEG_7];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x04;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
/////////8位显示//////////////////////
P0=table[SEG_8];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x08;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
}
///////////////////////////////////////////////////
//函数名称:void alarm_buzzer(void)
//函数功能:报警子程序
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void alarm_buzzer(void)		//按键音
{ 		
		unsigned char k; /////局部变量定义
		for(k=248;k>0;k--) ////内层循环
		{
		fmq=1;
		delay50us();
		fmq=0;
		delay50us();
		}
		fmq=1;
} 
void ceshiyin(void)
{
			unsigned char k; /////局部变量定义
		for(k=248;k>0;k--) ////内层循环
		{
		fmq=1;
		delay10ms();
		fmq=0;
		delay10ms();
		}
		fmq=1;
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
		unsigned int k; /////局部变量定义

			for(k=50;k>0;k--) ////内层循环
				{
				;
				}

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
//		unsigned char i,j,k; /////局部变量定义
//		unsigned char j,k; /////局部变量定义
		unsigned char k; /////局部变量定义
//  		for(i=20;i>0;i--)   //最外层循环
//			{
//		    	for(j=20;j>0;j--)   // 上层循环
//					{
				  		for(k=248;k>0;k--) ////内层循环
							{
							;
							}
//					}
//			}
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
//函数名称:void jiafa(void)
//函数功能:报警函数
//入口参数:jsq_1,jsq_2
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void jsq_jiafa(void)
{
/*
//////////////进入加法////////////////
	if(seg_key==14)&&(jsq==5)
	{
	jiafa_flag1=1;
	}
SEG_1=
//////////////获取加法第一个数////////////////
	if(jiafa_flag==1)
	{
	jsq_1=seg_key;
	}
//////////////获取加法键号////////////////
	if(jiafa_flag==1)&&(seg_key==15)
	{
	jiafa_flag2=1;	
	}
//////////////获取加法第二个数////////////////
	if(jiafa_flag2==1)
	{
	jsq_2=seg_key;
	}
//////////////获取等号键////////////////
	if(jiafa_flag==1)&&(jiafa_flag==2)(seg_key==11)
	{
	jiafa_flag3=1;	
	}
//////////////实现加法运算////////////////
	if(jiafa_flag3==1)
	{
	sum=jsq_1+jsq_2;
	}

*/
} 
void display_ceshi(void)     //数码管测试函数 
{
	unsigned char i;
	unsigned int j;
	count=110;
	  for(i=0;i<10;i++)
	   {
	   count=count-11;
		  for(j=0;j<800;j++)
		   {
		   display_seg1();
		   }
		}
}
void display_seg1(void)      //4个数码管显示函数
{
//count=17;
//////////////显示数据处理程序/////////////////////////////
shiwei=count/10;//得到10位数据
gewei=count%10;//得到个位数据


/////////十位显示//////////////////////
P0=table[shiwei];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x40;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    delay500us();
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();
/////////个位显示//////////////////////
P0=table[gewei];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x80;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    delay500us();
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();

}
void display_ceshi_8(void)     //8个数码管测试函数 
{
	unsigned int i,k;
	unsigned int j=10;
for(k=0;k<10;k++)
{
j=j-1;

	  for(i=0;i<400;i++)//i越小 自检时间越短
	   {

		SEG_1=j;SEG_2=j;SEG_3=j;SEG_4=j;SEG_5=j;SEG_6=j;SEG_7=j;SEG_8=j;

		display_seg();     //8个调用数码管显示函数

		}
}

}
void display_seg_8(void)      //8个数码管显示函数
{
//count=123;
//SEG_1=4;
//SEG_2=3;
//SEG_3=8;
//SEG_4=9;
//SEG_5=7;
//SEG_6=2;
//SEG_7=5;
//SEG_8=6;
//////////////显示数据处理程序/////////////////////////////
///*
//SEG_5=count/1000;//得到1000位数据
//SEG_6=(count%1000)/100;//得到个位数据100
//SEG_7=((count%1000)%100)/10;//得到10位数据
//SEG_8=((count%1000)%100)%10;//得到个位数据

//*/
//////////////显示程序/////////////////////////////

/////////1位显示//////////////////////
P0=table[SEG_1];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x01;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us

/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();

/////////2位显示//////////////////////
P0=table[SEG_2];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x02;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();

/////////3位显示//////////////////////
P0=table[SEG_3];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x04;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();
/////////4位显示//////////////////////
P0=table[SEG_4];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x08;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();
/////////5位显示//////////////////////
P0=table[SEG_5];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x10;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();
/////////6位显示//////////////////////
P0=table[SEG_6];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x20;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();
/////////7位显示//////////////////////
P0=table[SEG_7];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x40;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();
/////////8位显示//////////////////////
P0=table[SEG_8];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存
P0=~0x80;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
     delay500us();///延时500us
/////////黑屏显示//////////////////////
P0=table[17];//////数据从P0口送出到IC7---74HC573锁存器
P26=0;P26=1;P26=0;///IC7数据锁存//////clk信号
P0=~0x00;//////位码从P0口送出到IC6---74HC573锁存器
P27=0;P27=1;P27=0;///IC6位码数据锁存
	//delay10ms();///延时10ms
    //delay500us();


}