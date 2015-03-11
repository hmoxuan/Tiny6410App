/**
 * adc.c file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#include <stdlib.h>

/*Created By qinfei*/
#include <debug.h>

/*adc�豸�ļ�������*/
static int fd;

/*adc��ʼ��:���豸�ļ�*/
int adc_Init(void)
{
	int ret = 0;
	dbg("Going into adc_Init function!\n");

	fd = open("/dev/adc", 0);
	if (fd < 0)
	{
		perror("open ADC device !");
		return (ret = -1);
	}

	dbg("Successfully Open /dev/adc!\n");
	return ret;
}

/*adc�ر��豸�ļ�:�ͷ���Դ*/
void adc_Destroy(void)
{
	dbg("Going into adc_Destroy function!\n");

	close(fd);
}

/*adcӦ�ÿ���:ʵ��adc�����Ӧ���߼�����*/
void adc_AppCtl(void)
{
	int i = 0;
	char buffer[30];
	int len ;
	int value;

	dbg("Going into adc_AppCtl function!\n");
	printf( "\nAdc Test !\n" );
	printf( "After output 20 times, Exit this program!\n" );

	while (1)
	{
		//i++;//�����20����ֵֵʱ���˳����Գ���

		/*1.��ȡADת��ֵ*/
		len = read(fd, buffer, sizeof(buffer) - 1);
		if (len > 0)
		{
			buffer[len] = '\0';
			sscanf(buffer, "%d", &value);
			printf("ADC Value: %d\n", value);
			i++;//�����20����ֵֵʱ���˳����Գ���
			printf("i = %d\n", i);
		}
		else
		{
			perror("read ADC device !");
			exit(1);
		}
		if (i % 5 == 0)
			printf("\n");//ÿ���5�У���ӡһ�λ��У�
		/*��ʱ1s,��ȡ��һ������*/
		sleep(1);//�ȴ� 1s

		/*2.�˳�*/
		if (i >= 20)
		{
			dbg("Going out from adc_AppCtl function!\n");
			adc_Destroy();
			exit(0);
		}
	}
}
