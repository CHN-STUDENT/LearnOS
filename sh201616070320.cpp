#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#define MAX_LINE 80 
/* 80 chars per line, per command, should be enough. 每条指令限制80个字符 */
#define MAX_COMMANDS 5 
/* size of history 命令历史显示5条*/
char history[MAX_COMMANDS][MAX_LINE];
char display_history[MAX_COMMANDS][MAX_LINE];
int command_count = 0;
//索引号=命令计数%最大历史命令数
void addtohistory(char inputBuffer[]) 
{/* Add the most recent command to the history. 将最近的命令添加到历史记录中*/
    int i = 0;
    // add the command to history 将该命令添加到历史记录
    strcpy(history[command_count % MAX_COMMANDS], inputBuffer);
    // add the display-style command to history 将要显示的命令添加到历史中
    // char *strcpy(char *dst, const char *src); src为原字符串 dst为要复制的字符串
    while (inputBuffer[i] != '\n' && inputBuffer[i] != '\0') 
    {//循环复制字符串
        display_history[command_count % MAX_COMMANDS][i] = inputBuffer[i]; //这是把输入缓冲数组东西给显示历史数组
        i++;
    }
    display_history[command_count % MAX_COMMANDS][i] = '\0'; //显示历史数组加截止符
    ++command_count;//命令计数+1
    return;
}
/** 
* The setup function below will not return any value, but it will just: read
* in the next command line; separate it into distinct arguments (using blanks as
* delimiters), and set the args array entries to point to the beginning of what
* will become null-terminated, C-style strings. 
* setup函数除了会读取命令，以空格做参数分隔符，并不会返回任何值，但将args参数数组项设置为C的以空字符结尾字符数组
*/
int setup(char inputBuffer[], char *args[],int *background)
{
    int length,i, start,ct, command_number;
    //字符长度，访问inputBuffer数组的循环索引，下一个命令参数开始处的位置，在参数args[]数组中索引 ，请求的命令号命令编号
    /* # of characters in the command line */
    /*loop index for accessing inputBuffer array */
    /* index where beginning of next command parameter is */
    /* index of where to place the next parameter into args[] */
    /*index of requested command number */
    ct = 0; //在参数args[]数组中索引先设置0
    /*read what the user enters on the command line */
    do 
    {
        printf("osh>");
        fflush(stdout);//将标注输出流立即输出
        length = read(STDIN_FILENO,inputBuffer,MAX_LINE); 
        //ssize_t read(int fd, void *buf, size_t count); 返回值：成功返回读取的字节数，出错返回-1并设置errno，如果在调read之前已到达文件末尾，则这次read返回0
        //从文件输入流读取最大MAX_LINE到输入缓冲数组中
    }
    while (inputBuffer[0] == '\n'); /* swallow newline characters 以换行符结束*/
    /**
    * 0 is the system predefined file descriptor for stdin (standard input),
    * which is the user's screen in this case. inputBuffer by itself is the
    * same as &inputBuffer[0], i.e. the starting address of where to store
    * the command that is read, and length holds the number of characters
    * read in. inputBuffer is not a null terminated C-string. 
    */
    //0是stdin（标准输入）的系统预定义文件描述符，inputBuffer本身与&inputBuffer[0]相同，即存储位置的起始地址，读取返回的是字符串长度，它不是空的C的字符串
    start = -1; //下一个命令参数开始处的索引先设置-1
    if (length == 0)
        exit(0); //命令长度为0退出
    /* ^d was entered, end of user command stream */
    /*** the <control><d> signal interrupted the read system call 
    * if the process is in the read() system call, read returns -1
    * However, if this occurs, errno is set to EINTR. We can check this value
    * and disregard the -1 value 
    * 在输入过程中如果按下Ctrl+D 中断读取，将会将errno变成EINTER，就可以不用判断没有读取任何字符（read函数返回-1）
    */
    if ( (length < 0) && (errno != EINTR) ) 
    {     
        //处理错误的情况，直接报错退出
        perror("error reading the command");
        exit(-1);          
        /* terminate with error code of -1 */
    }
    /* Check if they are using history 检查他们是否使用历史记录*/
    if (inputBuffer[0] == '!') 
    {//输入缓冲第一个字符是!
        if (command_count == 0) 
        {//不存在历史命令
            printf("No history\n");
            return 1;
        }
        else if (inputBuffer[1] == '!') 
        {
            //输入缓冲第二个字符是!     
            //即命令是!!
            // restore the previous command 恢复上一个的命令
            strcpy(inputBuffer,history[(command_count -1) % MAX_COMMANDS]); //把上一个命令从命令历史数组复制给输入缓冲数组
            length = strlen(inputBuffer) + 1;
        }
        else if (isdigit(inputBuffer[1])) 
        {     //输入缓冲第二个字符是!   
            //即命令是!n  n为数字
            /* retrieve the nth command 检索第n个命令 */
            command_number = atoi(&inputBuffer[1]); 
            strcpy(inputBuffer,history[command_number]);
            length = strlen(inputBuffer) + 1;
        }
    }
    addtohistory(inputBuffer);/*** Add the command to the history 将命令添加到历史记录中 */
    /*** Parse the contents of inputBuffer 解析inputBuffer的内容 */
    for (i=0;i<length;i++) 
    { 
        /* examine every character in the inputBuffer 检查inputBuffer中的每个字符*/
        switch (inputBuffer[i])
        {
            case ' '://这也是空格的一种，下面的'\t'是空格转义符
            case '\t' : /* argument separators 参数分隔符（空格）*/
                if(start != -1)
                {
                    args[ct] = &inputBuffer[start]; /* set up pointer 取值给参数数组*/
                    ct++;//参数数组索引+1
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string C的数组截止字符 */
                start = -1; //重新下一个命令参数开始处的索引先设置-1
                break; //我觉得这可能是continue，而不是break
            case '\n': /* should be the final char examined 检查最后的字符 */
                if(start != -1)
                {
                    args[ct] = &inputBuffer[start];  * set up pointer 取值给参数数组*/   
                    ct++;//参数数组索引+1
                }
                inputBuffer[i] = '\0';//重新下一个命令参数开始处的索引先设置-1
                args[ct] = NULL; /* no more arguments to this command 该命令没有更多的参数 */ 
                //NULL在C中应该是0，意味着这个参数数组结束
                break;
            default : /* some other character 其他字符 */
                if (start == -1)
                    start = i;//命令参数开始处的索引更新
                if (inputBuffer[i] == '&') 
                {    
                    /// command & 在shell中为该程序转到后台运行
                    *background = 1;//这个标志变量变为1
                    inputBuffer[i-1] = '\0';//忽略这个符号，将字符串截止符号加到输入缓冲数组command后面的空格
                }
        } /* end of switch */
    } /* end of for */
    /*** If we get &,don't enter it in the args array 在参数数组中忽略& */
    // command & 在shell中为该程序转到后台运行
    if (*background)         //???感觉有问题
        args[--ct] = NULL; 
    args[ct] = NULL; /* no more arguments to this command 该命令没有更多的参数 */ 
    //NULL在C中应该是0，意味着这个参数数组结束
    /* just in case the input line was > 80 */
    return 1;
} /* end of setup routine */
int main(void)
{
    pid_t child;
    child = fork(); /* creates a duplicate process! 创建副本进程*/
    switch (child) 
    {
        //待填
    }
    return 0;
}
