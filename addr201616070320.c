#include <stdio.h> 
#include <stdlib.h>
#define page 32 //���ҳ�� 
#define page_byte 1024 //ÿҳ��С1KB 
#define memory_byte 16384 //16K������
/****************************************************
	�����ڴ棺 
		32ҳ * 1KB ��Ч15λ 
		XXX  XXXX  XXXX  XXXX
	    ----- | --------------- 
	     5λ  | 10λ
	     ҳ�� | ��ַ 
	���棺
	    16KB ��Ч14λ 
	 	XX  XXXX  XXXX  XXXX
		---- | ---------------
		4λ  | 10λ
		��� |��ַ
	��Ӧ��ϵ��
		�����ҳ�� ��Ӧ   ���  ����ת��
		�����ҳ�� δ��Ӧ ���  ȱҳ����Ҫ�����ڴ�
	 	���������ҳ��          Խ�� 
****************************************************/ 
//10����ת������λ�� 
int dec_to_bit(int num)
{
	unsigned int i=0; 
	for(;num/2!=0;num/=2,i++);
	return i;
}
//10/16�����ִ�ת�������ִ� 
char * hex_or_dec_to_bin(char *num,int system)
{
	if(system!=10&&system!=16)
	{//ת������������� 
		printf("Please enter the correct system to convert.");
		exit (-1);
	} 
	unsigned int len=0;
	for(;*(num+len)!='\0';len++);//����������ַ������� 
	unsigned int bit_len=len*4;//��4λ2���Ʊ�ʾһλ16����/10����
	char *bin=(char *)malloc(bit_len);//�½�һ���ַ���
	for(unsigned int i=0;i<bit_len;i++)
		*(bin+i)='0';//��0���� 
	long long dec=strtoll(num,NULL,system);//16/10�����ַ���תlonglong 
	unsigned int index=bit_len-1;//�����±꣬����-1 
	for(;dec/2!=0;dec/=2,index--)
	{//����ȡ��ת������ 
		*(bin+index)=dec%2+'0'; // ʹ��ch+'0' ������ת���ַ� 
	}
	*(bin+index)=(dec%2+'0'); 
	*(bin+bit_len)='\0';//��ӽ�ֹ�� 
	return bin;
}
//�������ִ���� 
void bin_output(char *bin)
{
	for(int i=0;*(bin+i)!='\0';i++)
	{
		if(i%4==0)//��λ���һ���ո� 
			printf(" ");
		printf("%c",*(bin+i));
	}
	printf("\n");
} 
//�������ִ�ת10����  
long long bin_to_dec(char * bin)
{
	long long dex=0;
	unsigned int bin_len=0;
	for(;*(bin+bin_len)!='\0';bin_len++);
	int index=bin_len-1;
	for(int e=1;index>=0;index--,e*=2)
	{ //ʹ��ch-'0' ���ַ�ת������	
		dex=dex+(*(bin+index)-'0')*e;
	}
	return dex;
} 
char * check_page(char *bin,int logical_bit,int page_bit,int block_bit)
{
	unsigned int len=0;
	for(;*(bin+len)!='\0';len++);//����������ַ������� 
	unsigned int page_start_index=len-logical_bit;//ҳ����ʼ����λ��=�������ַ�������-�߼���ַ��Ч����
	char *page_bin=(char *)malloc(page_bit+1);//ҳ�Ŷ������ַ��������һλ��Ϊ�˷Ž�����
	for(unsigned int i=page_start_index,j=0;i<=page_bit;i++,j++)
		*(page_bin+j)=*(bin+i); 
	*(page_bin+page_bit+1)='\0'; 
	int page_dec=bin_to_dec(page_bin);
	int block_dec;
	free (page_bin);
	if (page_dec == 0)
	{//�ж�ҳ������Ӧ�Ŀ��
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
	{//�ж��Ƿ�Խ��
		printf("Exceeds the virtual memory range\n"); 
		exit (-2);
	}
	else
	{//�ж��Ƿ�ȱҳ 
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
	{//����ȡ��ת������ 
		*(block_bin+index)=block_dec%2+'0'; // ʹ��ch+'0' ������ת���ַ� 
	}
	*(block_bin+index)=(block_dec%2+'0'); 
	*(block_bin+block_bit)='\0';//��ӽ�ֹ��
	return block_bin; 
}
//���������ַ 
void logical_to_physical(char *bin,char *block,int physical_bit)
{
	unsigned int len=0;
	for(;*(bin+len)!='\0';len++);//����������ַ������� 
	unsigned int block_start_index=len-physical_bit;//�����ʼ����λ��=�������ַ�������-�����ַ��Ч����
	for(unsigned int i=0;*(block+i)!='\0';i++,block_start_index++)
	{//����Ÿ��Ƶ��������ַ����� 
		*(bin+block_start_index)=*(block+i);
	}
}
int main(void)
{
	//����������߼��ڴ���ռ��ʮ���ƴ�С 
	const int logical_byte=page*page_byte; 
	const int physical_byte=memory_byte; 
	//����������߼��ڴ���Ч�Ķ�����λ��
	const int logical_bit=dec_to_bit(logical_byte);
	const int physical_bit=dec_to_bit(memory_byte); 
	//����ҳ�źͿ����ռ������λ��
	const int page_bit=dec_to_bit(page);
	const int block_bit=4; //һ����Ϊ���10���ƣ���4������������ʾ 
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
