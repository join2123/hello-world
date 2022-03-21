
#include"STC15F2K60S2.H"
#define uchar unsigned char
#define uint unsigned int

uchar count  = 0;	//第二个按键计数
uchar count1 = 0;  //加减计数

//led定义
sbit L1=P1^0;//////定义第1个指示灯的名称为L1 
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

sbit fmq = P3^2;//蜂鸣器

void key(void);          //按键函数
void display(void);      //显示函数
void delay10ms(void);	 //延时10ms
void alarm_buzzer(void); //蜂鸣器电路
void delay5ms(void);	 //延时10ms
void delay50us(void);    //延时50us



void main()
{
	P3 = 0xff;				//初始化P3口准备读取按键（按键输入）
	while(1)
	{
		key();
		//display();
	}
}

///////////////////////////////////////////////////
//函数名称:void delay10ms(void)
//函数功能:延时5/10ms秒的子程序
//入口参数:
//出口参数:
//函数说明:
///////////////////////////////////////////////////
void delay10ms(void)		//延时10ms的子程序//
{ 		
		unsigned char j,k; /////局部变量定义
		for(j=20;j>0;j--)   // 上层循环
		{
			for(k=248;k>0;k--); ////内层循环
		}		
}  

void delay5ms(void)		//延时5ms的子程序//
{ 		
		unsigned char j,k; /////局部变量定义
		for(j=10;j>0;j--)   // 上层循环
		{
			for(k=248;k>0;k--); ////内层循环
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
	if(key_1==0||key_2==0||key_3==0||key_4==0)/////////如果按键按下
	{
	  delay10ms();///////延时10ms
	  if(key_1==0||key_2==0||key_3==0||key_4==0)///////按键的确按下
	  {
		  //按1第一个灯亮灭
			if(key_1==0)
			{
				L1 = ~L1;
				//alarm_buzzer();
			}
			//二计数并显示bcd
			if(key_2==0)
			{
				count++;///////按键计数器计数
				if(count >= 10){count = 0;}//////控制计数器最大值;16即为16进制;20即为20进制;但最大计数255，因为count定义的是unsigend char
				P1=~count;				   ///////将计数值送P0口利用LED指示灯以BCD码（8421码）显示


				//计数大于5 第八位灯亮（报警）
				if(count > 5)
				{
					L8 = 0;
					//fmq=~fmq;
					//alarm_buzzer();
				}
			}
			//3和4实现加减法（改109行count1）
			if(key_3==0||key_4==0)
			{	
				if(key_3 ==0 )
				{
					alarm_buzzer();//按键音
					count1++;
					if(count1 >= 10)
					{
						count1 = 9;						
					}
				}
				if(key_4 == 0)
				{
					alarm_buzzer();
					if(count1 > 0)
					{
						count1--;
					}
					else
						count1 = 0;
				}
				P1=~count1;
			}	
		while(key_1==0||key_2==0||key_3==0||key_4==0){;}///////按键等待抬起
		}	
	}
}

void alarm_buzzer(void)		//蜂鸣器控制（按键音）//
{ 		
	unsigned char k; /////局部变量定义
	for(k=248;k>0;k--) ////内层循环
	{
		fmq=0;
		delay50us();
		fmq=1;
		delay50us();			
	}
}  

void delay50us(void)		//延时5s的子程序//
{ 		
	unsigned int k; /////局部变量定义
	for(k=50;k>0;k--); ////内层循环
}

void display(void)         //显示
{
	P1 = ~count;
}

