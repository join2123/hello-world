
#include"STC15F2K60S2.H"
#define uchar unsigned char
#define uint unsigned int

uchar count  = 0;	//�ڶ�����������
uchar count1 = 0;  //�Ӽ�����

//led����
sbit L1=P1^0;//////�����1��ָʾ�Ƶ�����ΪL1 
sbit L2=P1^1;//////�����2��ָʾ�Ƶ�����ΪL2 
sbit L3=P1^2;//////�����3��ָʾ�Ƶ�����ΪL3  
sbit L4=P1^3;//////�����4��ָʾ�Ƶ�����ΪL4 
sbit L5=P1^4;//////�����5��ָʾ�Ƶ�����ΪL5  
sbit L6=P1^5;//////�����6��ָʾ�Ƶ�����ΪL6 
sbit L7=P1^6;//////�����7��ָʾ�Ƶ�����ΪL7  
sbit L8=P1^7;//////�����8��ָʾ�Ƶ�����ΪL8 



//��������
sbit key_1=P3^4;//////�����1������ΪKEY_1
sbit key_2=P3^5;//////�����1������ΪKEY_2
sbit key_3=P4^2;//////�����1������ΪKEY_3
sbit key_4=P4^4;//////�����1������ΪKEY_4

sbit fmq = P3^2;//������

void key(void);          //��������
void display(void);      //��ʾ����
void delay10ms(void);	 //��ʱ10ms
void alarm_buzzer(void); //��������·
void delay5ms(void);	 //��ʱ10ms
void delay50us(void);    //��ʱ50us



void main()
{
	P3 = 0xff;				//��ʼ��P3��׼����ȡ�������������룩
	while(1)
	{
		key();
		//display();
	}
}

///////////////////////////////////////////////////
//��������:void delay10ms(void)
//��������:��ʱ5/10ms����ӳ���
//��ڲ���:
//���ڲ���:
//����˵��:
///////////////////////////////////////////////////
void delay10ms(void)		//��ʱ10ms���ӳ���//
{ 		
		unsigned char j,k; /////�ֲ���������
		for(j=20;j>0;j--)   // �ϲ�ѭ��
		{
			for(k=248;k>0;k--); ////�ڲ�ѭ��
		}		
}  

void delay5ms(void)		//��ʱ5ms���ӳ���//
{ 		
		unsigned char j,k; /////�ֲ���������
		for(j=10;j>0;j--)   // �ϲ�ѭ��
		{
			for(k=248;k>0;k--); ////�ڲ�ѭ��
		}		
}  
///////////////////////////////////////////////////
//��������:void key(void)
//��������:��������
//��ڲ���:
//���ڲ���:
//����˵��:
///////////////////////////////////////////////////
void key(void)      //��������
{
	if(key_1==0||key_2==0||key_3==0||key_4==0)/////////�����������
	{
	  delay10ms();///////��ʱ10ms
	  if(key_1==0||key_2==0||key_3==0||key_4==0)///////������ȷ����
	  {
		  //��1��һ��������
			if(key_1==0)
			{
				L1 = ~L1;
				//alarm_buzzer();
			}
			//����������ʾbcd
			if(key_2==0)
			{
				count++;///////��������������
				if(count >= 10){count = 0;}//////���Ƽ��������ֵ;16��Ϊ16����;20��Ϊ20����;��������255����Ϊcount�������unsigend char
				P1=~count;				   ///////������ֵ��P0������LEDָʾ����BCD�루8421�룩��ʾ


				//��������5 �ڰ�λ������������
				if(count > 5)
				{
					L8 = 0;
					//fmq=~fmq;
					//alarm_buzzer();
				}
			}
			//3��4ʵ�ּӼ�������109��count1��
			if(key_3==0||key_4==0)
			{	
				if(key_3 ==0 )
				{
					alarm_buzzer();//������
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
		while(key_1==0||key_2==0||key_3==0||key_4==0){;}///////�����ȴ�̧��
		}	
	}
}

void alarm_buzzer(void)		//���������ƣ���������//
{ 		
	unsigned char k; /////�ֲ���������
	for(k=248;k>0;k--) ////�ڲ�ѭ��
	{
		fmq=0;
		delay50us();
		fmq=1;
		delay50us();			
	}
}  

void delay50us(void)		//��ʱ5s���ӳ���//
{ 		
	unsigned int k; /////�ֲ���������
	for(k=50;k>0;k--); ////�ڲ�ѭ��
}

void display(void)         //��ʾ
{
	P1 = ~count;
}

