#include <iostream>
#include <string>
using namespace std;
#define page 32 //虚寸页数 
#define page_byte 1024 //每页大小1KB 
#define memory_byte 16384 //16K的主存
/****************************************************
	虚拟内存： 
		32页 * 1KB 有效15位 
		XXX  XXXX  XXXX  XXXX
	    ----- | --------------- 
	     5位  | 10位
	     页号 | 地址 
	主存：
	    16KB 有效14位 
	 	XX  XXXX  XXXX  XXXX
		---- | ---------------
		4位  | 10位
		块号 |地址
	对应关系：
		分配的页号 对应   块号  可以转换
		分配的页号 未对应 块号  缺页，需要调入内存
	 	超过分配的页号          越界 
****************************************************/ 
//10进制转二进制位数 
int dec_to_bit(int num)
{
	unsigned int i=0; 
	for(;num/2!=0;num/=2,i++);
	return i;
}
//计算物理和逻辑内存所占的十进制大小 
const int logical_byte=page*page_byte; 
const int physical_byte=memory_byte; 
//计算物理和逻辑内存有效的二进制位数
const int logical_bit=dec_to_bit(logical_byte);
const int physical_bit=dec_to_bit(memory_byte); 
//计算页号和块号所占二进制位数
const int page_bit=dec_to_bit(page);
const int block_bit=4; //一般认为块号10进制，用4个二进制数表示 
//输入16进制/10进制字符串转成2进制字符串 
char * hex_or_dec_to_bin(string num,int system)
{
	//count可以计数转换之后2进制位数 
	if(system!=10&&system!=16)
	{//转换进制输入错误 
		cout<<"Please enter the correct system to convert."<<endl;
		exit (-1);
	} 
	unsigned int len=num.size()*4;//用4位2进制表示一位16进制/10进制 
	char *bin=new char[len];//新建一个字符串
	for(unsigned int i=0;i<len;i++)
		*(bin+i)='0';//用0覆盖 
	long long dec=strtoll(num.c_str(),NULL,system);//16/10进制字符串转longlong 
	unsigned int index=len-1;//索引下标，长度-1 
	for(;dec/2!=0;dec/=2,index--)
	{//除二取余转二进制 
		*(bin+index)=dec%2+'0'; // 使用ch+'0' 将整形转成字符 
	}
	*(bin+index)=(dec%2+'0'); 
	*(bin+len)='\0';//添加截止符 
	return bin;
} 
//二进制字串转10进制  
long long bin_to_dec(char * bin)
{
	long long dex=0;
	unsigned int bin_len=0;
	for(;*(bin+bin_len)!='\0';bin_len++);
	int index=bin_len-1;
	for(int e=1;index>=0;index--,e*=2)
	{ //使用ch-'0' 将字符转成整形	
		dex=dex+(*(bin+index)-'0')*e;
	}
	return dex;
}
char * check_page(char *bin)
{
	unsigned int len=0;
	for(;*(bin+len)!='\0';len++);//计算二进制字符串长度 
	unsigned int page_start_index=len-logical_bit;//页号起始索引位置=二进制字符串长度-逻辑地址有效长度
	char *page_bin=new char[page_bit+1];//页号二进制字符串，多加一位是为了放截至符
	for(unsigned int i=page_start_index,j=0;i<=page_bit;i++,j++)
		*(page_bin+j)=*(bin+i); 
	*(page_bin+page_bit+1)='\0'; 
	int page_dec=bin_to_dec(page_bin);
	int block_dec;
	delete page_bin;
	if (page_dec == 0)
	{//判断页号所对应的块号
		block_dec = 5;
	}
	else if (page_dec == 1)
	{
		block_dec = 10;
	}
	else if (page_dec == 2)
	{
		block_dec = 4;
	}
	else if (page_dec == 3)
	{
		block_dec = 7;
	}
	else if (page_dec > 5)
	{//判断是否越界
		cout<<"Exceeds the virtual memory range"<<endl; 
		exit (-2);
	}
	else
	{//判断是否缺页 
		cout<<"Page fault"<<endl; 
		exit (-3);
	}
	cout<<"Page:"<<page_dec<<" "<<"Block:"<<block_dec<<endl; 
	char *block_bin=new char[block_bit]; 
	for(unsigned int i=0;i<block_bit;i++)
	{
		*(block_bin+i)='0';
	}
	unsigned int index=block_bit-1;
	for(;block_dec/2!=0;block_dec/=2,index--)
	{//除二取余转二进制 
		*(block_bin+index)=block_dec%2+'0'; // 使用ch+'0' 将整形转成字符 
	}
	*(block_bin+index)=(block_dec%2+'0'); 
	*(block_bin+block_bit)='\0';//添加截止符
	return block_bin; 
}

void logical_to_physical(char *bin,char *block)
{
	unsigned int len=0;
	for(;*(bin+len)!='\0';len++);//计算二进制字符串长度 
	unsigned int block_start_index=len-physical_bit;//块号起始索引位置=二进制字符串长度-物理地址有效长度
	for(unsigned int i=0;*(block+i)!='\0';i++,block_start_index++)
	{//将块号复制到二进制字符串上 
		*(bin+block_start_index)=*(block+i);
	}
}


void bin_output(char *bin)
{
	for(int i=0;*(bin+i)!='\0';i++)
	{
		if(i%4==0)//四位输出一个空格 
			cout<<" ";
		cout<<*(bin+i);
	}
	cout<<endl;
}

int main(void)
{
	string input;
	cout<<"Input a HEX adress: ";
	cin>>input; //输入16进制字符串 
	char *bin=hex_or_dec_to_bin(input,16); 
	cout<<"Logical address:";
	bin_output(bin);
	char *block=check_page(bin);
	logical_to_physical(bin,block); 
	cout<<"Physical address:";
	bin_output(bin);
	cout<<hex<<bin_to_dec(bin);
	delete bin,block; 
	return 0;
}
