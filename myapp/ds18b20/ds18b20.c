/**
 * ds18b20.h file implement Tiny6410.
 * Author: qinfei 2015.03.10
 */
#include <stdlib.h>

/*Created By qinfei*/
#include <debug.h>

static int fd;//�ļ�������

/*ds18b20��ʼ��:���豸�ļ�*/
int ds18b20_Init(void)
{
	int ret = 0;
	dbg("Going into ds18b20_Init function!\n");

    fd = open("/dev/ds18b20", 0);
    if (fd < 0)
    {
        perror("open device failed !\n");
        return (ret = -1);
    }

    dbg("Successfully Open /dev/ds18b20 !\n");

    return ret;
}

/*ds18b20�ر��豸�ļ�:�ͷ���Դ*/
void ds18b20_Destroy(void)
{
	dbg("Going into ds18b20_Destroy function!\n");
	close(fd);
}

/*ds18b20Ӧ�ÿ���:ʵ��ds18b20�����Ӧ���߼�����*/
void ds18b20_AppCtl(void)
{
	int i = 0;
	unsigned char result[2];			//��ds18b20�����Ľ����result[0]��ŵͰ�λ
	unsigned char integer_value = 0;	//�������ֵ�ֵ
	float decimal_value;				//С�����ֵ�ֵ
	float temperature;					//�¶���ֵ

	dbg("Going into ds18b20_AppCtl function!\n");
	printf( "\nTemperature Test !\n" );
	printf( "After output 20 times temperature,Exit this program!\n" );

	while (1)
	{
		//i++;//�����20���¶�ֵʱ���˳��¶Ȳ��Գ���

		//1.��ȡ�¶�ֵ����result
		read(fd, &result, sizeof(result));

		//2.��ȡ�������ֵ�ֵ
		integer_value = ((result[0] & 0xf0) >> 4) | ((result[1] & 0x07) << 4);

		//3.��ȡС�����ֵ�ֵ(��ȷ��0.25��)
		decimal_value = 0.5 * ((result[0] & 0x0f) >> 3) + 0.25 * ((result[0] & 0x07) >> 2);

		//4.�����¶�ֵ
		temperature = (float)integer_value + decimal_value;
		printf("The temperature is %6.2f\n",temperature);
		printf("i = %d\n", i);
		if (i % 5 == 0)
			printf("\n");//ÿ���5�У���ӡһ�λ��У�

		/*��ʱ1s,��ȡ��һ������*/
		sleep(1);//�ȴ� 1s
		
		i++;//�����20���¶�ֵʱ���˳��¶Ȳ��Գ���

		//5.�˳�
		if (i >= 20)
		{
			dbg("Going out from ds18b20_AppCtl function!\n");
			ds18b20_Destroy();
			exit(0);
		}
	}
}

