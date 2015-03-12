/**
 * bmp_lib.c file implement Tiny6410.
 * Author: qinfei 2015.03.11
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/*Created By qinfei*/
#include <debug.h>
#include <bmp_lib.h>

int bmp_open(bmp_t *bmp, char *bmpn)
{
    int ret;
	bmp_file_header_t fhr;	/*文件信息头(14字节)*/
	bmp_info_header_t ihr;	/*位图信息头(40字节)*/

	dbg("Going into bmp_open function!\n");

	if (-1 == (bmp->fd=open(bmpn, O_RDONLY)))
	{
        err("Error: cannot open framebuffer device.\n");
        _exit(EXIT_FAILURE);
    }
	dbg("Open done!\n");

	read(bmp->fd, &fhr, sizeof(bmp_file_header_t));/*读取文件信息头*/
	read(bmp->fd, &ihr, sizeof(bmp_info_header_t));/*读取位图信息头*/

	bmp->width = char_to_int(ihr.width);
	bmp->height = char_to_int(ihr.height);
	bmp->bitcount = char_to_int(ihr.bitcount);
	bmp->size = (bmp->width * bmp->height * bmp->bitcount) / 8;

	dbg("bmp->width = %d\n", bmp->width);
	dbg("bmp->height = %d\n", bmp->height);
	dbg("bmp->bitcount = %d\n", bmp->bitcount);
	dbg("bmp->siz = %d\n", bmp->size);

	bmp->data = malloc(bmp->size);
	ret = read(bmp->fd, bmp->data, bmp->size);/*读取实际的图像数据 */
	bmp->curp = (rgb_32_t*)bmp->data;/*指向当前图像有效数据，即指向图像的起始数据 */

	return 0;
}

void bmp_close(bmp_t *bmp)
{
	close(bmp->fd);
	free(bmp->data);
}

