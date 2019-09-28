#include <stdio.h> 
#include <stdlib.h>
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
//10/16进制字串转二进制字串 
char * hex_or_dec_to_bin(char *num,int system)
{
	if(system!=10&&system!=16)
	{//转换进制输入错误 
		printf("Please enter the correct system to convert.");
		exit (-1);
	} 
	unsigned int len=0;
	for(;*(num+len)!='\0';len++);//计算二进制字符串长度 
	unsigned int bit_len=len*4;//用4位2进制表示一位16进制/10进制
	char *bin=(char *)malloc(bit_len);//新建一个字符串
	for(unsigned int i=0;i<bit_len;i++)
		*(bin+i)='0';//用0覆盖 
	long long dec=strtoll(num,NULL,system);//16/10进制字符串转longlong 
	unsigned int index=bit_len-1;//索引下标，长度-1 
	for(;dec/2!=0;dec/=2,index--)
	{//除二取余转二进制 
		*(bin+index)=dec%2+'0'; // 使用ch+'0' 将整形转成字符 
	}
	*(bin+index)=(dec%2+'0'); 
	*(bin+bit_len)='\0';//添加截止符 
	return bin;
}
//二进制字串输出 
void bin_output(char *bin)
{
	for(int i=0;*(bin+i)!='\0';i++)
	{
		if(i%4==0)//四位输出一个空格 
			printf(" ");
		printf("%c",*(bin+i));
	}
	printf("\n");
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
char * check_page(char *bin,int logical_bit,int page_bit,int block_bit)
{
	unsigned int len=0;
	for(;*(bin+len)!='\0';len++);//计算二进制字符串长度 
	unsigned int page_start_index=len-logical_bit;//页号起始索引位置=二进制字符串长度-逻辑地址有效长度
	char *page_bin=(char *)malloc(page_bit+1);//页号二进制字符串，多加一位是为了放截至符
	for(unsigned int i=page_start_index,j=0;i<=page_bit;i++,j++)
		*(page_bin+j)=*(bin+i); 
	*(page_bin+page_bit+1)='\0'; 
	int page_dec=bin_to_dec(page_bin);
	int block_dec;
	free (page_bin);
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
		printf("Exceeds the virtual memory range\n"); 
		exit (-2);
	}
	else
	{//判断是否缺页 
		printf("Page fault\n"); 
		exit (-3);
	}
	printf("Page:");
	printf("%d",page_dec);
	printf(" Block:");
	printf("%d\n",block_dec); 
	char *block_bin=(char *)malloc(block_bit); 
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
//计算物理地址 
void logical_to_physical(char *bin,char *block,int physical_bit)
{
	unsigned int len=0;
	for(;*(bin+len)!='\0';len++);//计算二进制字符串长度 
	unsigned int block_start_index=len-physical_bit;//块号起始索引位置=二进制字符串长度-物理地址有效长度
	for(unsigned int i=0;*(block+i)!='\0';i++,block_start_index++)
	{//将块号复制到二进制字符串上 
		*(bin+block_start_index)=*(block+i);
	}
}
int main(void)
{
	//计算物理和逻辑内存所占的十进制大小 
	const int logical_byte=page*page_byte; 
	const int physical_byte=memory_byte; 
	//计算物理和逻辑内存有效的二进制位数
	const int logical_bit=dec_to_bit(logical_byte);
	const int physical_bit=dec_to_bit(memory_byte); 
	//计算页号和块号所占二进制位数
	const int page_bit=dec_to_bit(page);
	const int block_bit=4; //一般认为块号10进制，用4个二进制数表示 
	printf("Input a HEX adress:");
	char *input=(char *)malloc(logical_bit+4);
	gets(input);
	char *bin=hex_or_dec_to_bin(input,16);
	printf("Logical address:");
	bin_output(bin);
	char *block=check_page(bin,logical_bit,page_bit,block_bit);
	logical_to_physical(bin,block,physical_bit); 
	printf("Physical address:");
	bin_output(bin);
	printf("%x",bin_to_dec(bin));
	free (input);
	free (bin);
	free (block);
	return 0;
}
