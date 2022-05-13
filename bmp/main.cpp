#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "libbmp.h"

int main()
{
	bmp_img img;
	bmp_img img_p;


	size_t w = 5;
	size_t h = 3;


	char dst[20] = "1.bmp";


	bmp_img_init_df(&img, w, h);
	bmp_img_init_df(&img_p, w+4, h+4);


	// Draw a checkerboard pattern:
	//bmp_img_read(&img, "0.bmp");
	printf("Stage 1 : start...\n");

	// 矩阵的向量分配
	int i = 1;
	for (size_t y = 0, x; y < h; y++)
	{
		for (x = 0; x <w ; x++)
		{
				bmp_pixel_init(&img.img_pixels[y][x], i, i, i);
				i++;
		}
	}

	bmp_img_write(&img, dst);
	bmp_value_print(&img);

	printf("Stage 1 : Successful!\n");
	printf("Stage 2 : start...\n");
	bmp_padding_2_repeat(&img, &img_p);
	bmp_value_print(&img_p);
	printf("Stage 2 : Successful!\n");

	bmp_img_free(&img);
	bmp_img_free(&img_p);


	return 0;
}