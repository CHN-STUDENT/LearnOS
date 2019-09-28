#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#define MAX_LINE 80 
/* 80 chars per line, per command, should be enough. ÿ��ָ������80���ַ� */
#define MAX_COMMANDS 5 
/* size of history ������ʷ��ʾ5��*/
char history[MAX_COMMANDS][MAX_LINE];
char display_history[MAX_COMMANDS][MAX_LINE];
int command_count = 0;
//������=�������%�����ʷ������
void addtohistory(char inputBuffer[]) 
{/* Add the most recent command to the history. �������������ӵ���ʷ��¼��*/
    int i = 0;
    // add the command to history ����������ӵ���ʷ��¼
    strcpy(history[command_count % MAX_COMMANDS], inputBuffer);
    // add the display-style command to history ��Ҫ��ʾ��������ӵ���ʷ��
    // char *strcpy(char *dst, const char *src); srcΪԭ�ַ��� dstΪҪ���Ƶ��ַ���
    while (inputBuffer[i] != '\n' && inputBuffer[i] != '\0') 
    {//ѭ�������ַ���
        display_history[command_count % MAX_COMMANDS][i] = inputBuffer[i]; //���ǰ����뻺�����鶫������ʾ��ʷ����
        i++;
    }
    display_history[command_count % MAX_COMMANDS][i] = '\0'; //��ʾ��ʷ����ӽ�ֹ��
    ++command_count;//�������+1
    return;
}
/** 
* The setup function below will not return any value, but it will just: read
* in the next command line; separate it into distinct arguments (using blanks as
* delimiters), and set the args array entries to point to the beginning of what
* will become null-terminated, C-style strings. 
* setup�������˻��ȡ����Կո��������ָ����������᷵���κ�ֵ������args��������������ΪC���Կ��ַ���β�ַ�����
*/
int setup(char inputBuffer[], char *args[],int *background)
{
    int length,i, start,ct, command_number;
    //�ַ����ȣ�����inputBuffer�����ѭ����������һ�����������ʼ����λ�ã��ڲ���args[]���������� ������������������
    /* # of characters in the command line */
    /*loop index for accessing inputBuffer array */
    /* index where beginning of next command parameter is */
    /* index of where to place the next parameter into args[] */
    /*index of requested command number */
    ct = 0; //�ڲ���args[]����������������0
    /*read what the user enters on the command line */
    do 
    {
        printf("osh>");
        fflush(stdout);//����ע������������
        length = read(STDIN_FILENO,inputBuffer,MAX_LINE); 
        //ssize_t read(int fd, void *buf, size_t count); ����ֵ���ɹ����ض�ȡ���ֽ�����������-1������errno������ڵ�read֮ǰ�ѵ����ļ�ĩβ�������read����0
        //���ļ���������ȡ���MAX_LINE�����뻺��������
    }
    while (inputBuffer[0] == '\n'); /* swallow newline characters �Ի��з�����*/
    /**
    * 0 is the system predefined file descriptor for stdin (standard input),
    * which is the user's screen in this case. inputBuffer by itself is the
    * same as &inputBuffer[0], i.e. the starting address of where to store
    * the command that is read, and length holds the number of characters
    * read in. inputBuffer is not a null terminated C-string. 
    */
    //0��stdin����׼���룩��ϵͳԤ�����ļ���������inputBuffer������&inputBuffer[0]��ͬ�����洢λ�õ���ʼ��ַ����ȡ���ص����ַ������ȣ������ǿյ�C���ַ���
    start = -1; //��һ�����������ʼ��������������-1
    if (length == 0)
        exit(0); //�����Ϊ0�˳�
    /* ^d was entered, end of user command stream */
    /*** the <control><d> signal interrupted the read system call 
    * if the process is in the read() system call, read returns -1
    * However, if this occurs, errno is set to EINTR. We can check this value
    * and disregard the -1 value 
    * ������������������Ctrl+D �ж϶�ȡ�����Ὣerrno���EINTER���Ϳ��Բ����ж�û�ж�ȡ�κ��ַ���read��������-1��
    */
    if ( (length < 0) && (errno != EINTR) ) 
    {     
        //�������������ֱ�ӱ����˳�
        perror("error reading the command");
        exit(-1);          
        /* terminate with error code of -1 */
    }
    /* Check if they are using history ��������Ƿ�ʹ����ʷ��¼*/
    if (inputBuffer[0] == '!') 
    {//���뻺���һ���ַ���!
        if (command_count == 0) 
        {//��������ʷ����
            printf("No history\n");
            return 1;
        }
        else if (inputBuffer[1] == '!') 
        {
            //���뻺��ڶ����ַ���!     
            //��������!!
            // restore the previous command �ָ���һ��������
            strcpy(inputBuffer,history[(command_count -1) % MAX_COMMANDS]); //����һ�������������ʷ���鸴�Ƹ����뻺������
            length = strlen(inputBuffer) + 1;
        }
        else if (isdigit(inputBuffer[1])) 
        {     //���뻺��ڶ����ַ���!   
            //��������!n  nΪ����
            /* retrieve the nth command ������n������ */
            command_number = atoi(&inputBuffer[1]); 
            strcpy(inputBuffer,history[command_number]);
            length = strlen(inputBuffer) + 1;
        }
    }
    addtohistory(inputBuffer);/*** Add the command to the history ��������ӵ���ʷ��¼�� */
    /*** Parse the contents of inputBuffer ����inputBuffer������ */
    for (i=0;i<length;i++) 
    { 
        /* examine every character in the inputBuffer ���inputBuffer�е�ÿ���ַ�*/
        switch (inputBuffer[i])
        {
            case ' '://��Ҳ�ǿո��һ�֣������'\t'�ǿո�ת���
            case '\t' : /* argument separators �����ָ������ո�*/
                if(start != -1)
                {
                    args[ct] = &inputBuffer[start]; /* set up pointer ȡֵ����������*/
                    ct++;//������������+1
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string C�������ֹ�ַ� */
                start = -1; //������һ�����������ʼ��������������-1
                break; //�Ҿ����������continue��������break
            case '\n': /* should be the final char examined ��������ַ� */
                if(start != -1)
                {
                    args[ct] = &inputBuffer[start];  * set up pointer ȡֵ����������*/   
                    ct++;//������������+1
                }
                inputBuffer[i] = '\0';//������һ�����������ʼ��������������-1
                args[ct] = NULL; /* no more arguments to this command ������û�и���Ĳ��� */ 
                //NULL��C��Ӧ����0����ζ����������������
                break;
            default : /* some other character �����ַ� */
                if (start == -1)
                    start = i;//���������ʼ������������
                if (inputBuffer[i] == '&') 
                {    
                    /// command & ��shell��Ϊ�ó���ת����̨����
                    *background = 1;//�����־������Ϊ1
                    inputBuffer[i-1] = '\0';//����������ţ����ַ�����ֹ���żӵ����뻺������command����Ŀո�
                }
        } /* end of switch */
    } /* end of for */
    /*** If we get &,don't enter it in the args array �ڲ��������к���& */
    // command & ��shell��Ϊ�ó���ת����̨����
    if (*background)         //???�о�������
        args[--ct] = NULL; 
    args[ct] = NULL; /* no more arguments to this command ������û�и���Ĳ��� */ 
    //NULL��C��Ӧ����0����ζ����������������
    /* just in case the input line was > 80 */
    return 1;
} /* end of setup routine */
int main(void)
{
    pid_t child;
    child = fork(); /* creates a duplicate process! ������������*/
    switch (child) 
    {
        //����
    }
    return 0;
}
