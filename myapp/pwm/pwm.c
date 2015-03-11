/**
 * pwm.c file implement Tiny6410.
 * Author: qinfei 2015.03.09
 */
#include <stdlib.h>

/*Created By qinfei*/
#include <debug.h>
#include <stringlib.h>

#define PWM_IOCTL_SET_FREQ 1	//定义宏变量,用于后面的 ioctl 中的控制命令
#define PWM_IOCTL_STOP 2		//定义宏变量,用于后面的 ioctl 中的控制命令
#define ESC_KEY 0x1b

/* leds设备文件描述符 */
static int fd = -1;

/*pwm初始化:打开设备文件*/
static void open_buzzer(void);

/*关闭pwm设备文件:释放资源*/
static void close_buzzer(void);

/*设置pwm频率*/
static void set_buzzer_freq(int freq);

/*关闭 buzzer*/
static void stop_buzzer(void);


/*pwm初始化:打开设备文件*/
static void open_buzzer(void)
{
	dbg("Going into open_buzzer function!\n");

    fd = open("/dev/pwm", 0);
    if (fd < 0) {
        perror("open pwm_buzzer device");
        exit(1);
    }
}

/*关闭pwm设备文件:释放资源*/
static void close_buzzer(void)
{
	dbg("Going into close_buzzer function!\n");

    if (fd >= 0) {
        ioctl(fd, PWM_IOCTL_STOP);//关闭buzzer
        close(fd);
        fd = -1;
    }
}

/*设置pwm频率*/
static void set_buzzer_freq(int freq)
{
	dbg("Going into set_buzzer_freq function!\n");

    //this IOCTL command is the key to set frequency
    int ret = ioctl(fd, PWM_IOCTL_SET_FREQ, freq);
    if(ret < 0) {
        perror("set the frequency of the buzzer");
        exit(1);
    }
}

/* 关闭 buzzer */
static void stop_buzzer(void)
{
	dbg("Going into stop_buzzer function!\n");

    int ret = ioctl(fd, PWM_IOCTL_STOP);
    if(ret < 0) {
        perror("stop the buzzer");
        exit(1);
    }
}

/*pwm应用控制:实现pwm具体的应用逻辑控制*/
void pwm_AppCtl(void)
{
    int freq = 1000;/*pwm的频率*/
    int key;		/*输入的键值*/

    dbg("Going into pwm_AppCtl function!\n");

    /*1.pwm初始化:打开设备文件*/
    open_buzzer();

    /*2.打印提示信息*/
    printf( "\nBUZZER TEST ( PWM Control )!\n" );
    printf( "Press +/- to increase/reduce the frequency of the BUZZER.\n" ) ;
    printf( "Press 'ESC' key to Exit this program.\n\n" );

    while(1)
    {
    	/*设置pwm频率*/
        set_buzzer_freq(freq);
        printf( "\tFreq = %d\n", freq );

        /*输入的键值*/
        key = getch();

        switch(key) {
			case '+':
				if( freq < 20000 )
					freq += 10;
				break;

			case '-':
				if( freq > 11 )
					freq -= 10 ;
				break;

			case ESC_KEY:
			case EOF:
				/*关闭 buzzer*/
				stop_buzzer();
				exit(0);

			default:
				break;
        }
    }
}
