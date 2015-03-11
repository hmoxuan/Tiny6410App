/**
 * pwm.c file implement Tiny6410.
 * Author: qinfei 2015.03.09
 */
#include <stdlib.h>

/*Created By qinfei*/
#include <debug.h>
#include <stringlib.h>

#define PWM_IOCTL_SET_FREQ 1	//��������,���ں���� ioctl �еĿ�������
#define PWM_IOCTL_STOP 2		//��������,���ں���� ioctl �еĿ�������
#define ESC_KEY 0x1b

/* leds�豸�ļ������� */
static int fd = -1;

/*pwm��ʼ��:���豸�ļ�*/
static void open_buzzer(void);

/*�ر�pwm�豸�ļ�:�ͷ���Դ*/
static void close_buzzer(void);

/*����pwmƵ��*/
static void set_buzzer_freq(int freq);

/*�ر� buzzer*/
static void stop_buzzer(void);


/*pwm��ʼ��:���豸�ļ�*/
static void open_buzzer(void)
{
	dbg("Going into open_buzzer function!\n");

    fd = open("/dev/pwm", 0);
    if (fd < 0) {
        perror("open pwm_buzzer device");
        exit(1);
    }
}

/*�ر�pwm�豸�ļ�:�ͷ���Դ*/
static void close_buzzer(void)
{
	dbg("Going into close_buzzer function!\n");

    if (fd >= 0) {
        ioctl(fd, PWM_IOCTL_STOP);//�ر�buzzer
        close(fd);
        fd = -1;
    }
}

/*����pwmƵ��*/
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

/* �ر� buzzer */
static void stop_buzzer(void)
{
	dbg("Going into stop_buzzer function!\n");

    int ret = ioctl(fd, PWM_IOCTL_STOP);
    if(ret < 0) {
        perror("stop the buzzer");
        exit(1);
    }
}

/*pwmӦ�ÿ���:ʵ��pwm�����Ӧ���߼�����*/
void pwm_AppCtl(void)
{
    int freq = 1000;/*pwm��Ƶ��*/
    int key;		/*����ļ�ֵ*/

    dbg("Going into pwm_AppCtl function!\n");

    /*1.pwm��ʼ��:���豸�ļ�*/
    open_buzzer();

    /*2.��ӡ��ʾ��Ϣ*/
    printf( "\nBUZZER TEST ( PWM Control )!\n" );
    printf( "Press +/- to increase/reduce the frequency of the BUZZER.\n" ) ;
    printf( "Press 'ESC' key to Exit this program.\n\n" );

    while(1)
    {
    	/*����pwmƵ��*/
        set_buzzer_freq(freq);
        printf( "\tFreq = %d\n", freq );

        /*����ļ�ֵ*/
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
				/*�ر� buzzer*/
				stop_buzzer();
				exit(0);

			default:
				break;
        }
    }
}
