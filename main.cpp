
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <math.h>
#include "libbmp.h"

#ifndef _UNISTD_H
#define _UNISTD_H
#endif _UNISTD_H
#include <pthread.h>

using namespace std;
#pragma comment(lib, "pthreadVC2.lib")



// 完成函数动态空间分配
// 完成多线程的数量为两位数
// 设计结构体满足线程的输入要求

//double temp_r[2160][3840];
//double temp_g[2160][3840];
//double temp_b[2160][3840];




double dabs(double c)

{
	if (c >= 0) return c;
	return -1.0 * c;
}



double* getWeight(double c, double a = -0.5)
{
	//c就是u和v，横坐标和纵坐标的输出计算方式一样
	double temp[4];

	temp[0] = c + 1;
	temp[1] = c;
	temp[2] = 1 - c;
	temp[3] = 2 - c;

	//printf("%f \t", float(temp[3]));

	//y(x) = (a+2)|x|*|x|*|x| - (a+3)|x|*|x| + 1   |x|<=1
	//y(x) = a|x|*|x|*|x| - 5a|x|*|x| + 8a|x| - 4a  1<|x|<2

	double t = 1.0;

	double weight[4];


	weight[0] = t * (a * pow(dabs(temp[0]), 3) - 5 * a * pow(dabs(temp[0]), 2) + 8 * a * dabs(temp[0]) - 4 * a);
	weight[1] = t * (a + 2) * pow(dabs(temp[1]), 3) - (a + 3) * pow(dabs(temp[1]), 2) + 1;
	weight[2] = t * (a + 2) * pow(dabs(temp[2]), 3) - (a + 3) * pow(dabs(temp[2]), 2) + 1;
	weight[3] = t * (a * pow(dabs(temp[3]), 3) - 5 * a * pow(dabs(temp[3]), 2) + 8 * a * dabs(temp[3]) - 4 * a);

	// double sum = pow(weight[0], 2) + pow(weight[1], 2) + pow(weight[2], 2) + pow(weight[3], 2);

	//printf("%f \t", sum);

	return weight;
}

void bicubic_inter(bmp_img in, uint32_t width_in, uint32_t height_in, bmp_img* out, uint32_t width_out, uint32_t height_out, uint32_t num_instance)
{
	// Draw a checkerboard pattern:

	printf("Stage 1(PADDING) : START...\n");
	bmp_img img_p;
	bmp_img_init_df(&img_p, width_in + 4, height_in + 4);
	bmp_padding_2_repeat(&in, &img_p);

	bmp_img_write(&img_p, "results/padding_test_0.bmp");

	//bmp_value_print(&in);
	//bmp_value_print(& img_p);

	printf("Stage 1(PADDING) : SUCCESSFUL!\n");
	printf("Stage 2(inter) : START...\n");


	double* Y;
	double* X;
	double u_y, u_x;


	double temp_r;
	double temp_g;
	double temp_b;

	for (size_t y = 0, x; y < height_out; y++)
	{
		//double src_y = (y + 0.5) / 4 -0.5 ;
		////if (src_y < 0) src_y = 0;
		////if (src_y > height_out - 1) src_y = height_out - 1;
		//src_y += 2;


		
		//double src_y = (y + 0.5) / 4 + 0.25;
		//size_t y1 = floor((y + 0.5) / 4 + 0.25) + 1;

		double src_y = (y + 0.5) / 4 - 0.5 ;
		size_t y1 = floor((y + 0.5) / 4 - 0.5) + 2;

		// size_t y1 = floor(src_y);
		size_t y0 = y1 - 1;
		size_t y2 = y1 + 1;
		size_t y3 = y1 + 2;

		u_y = src_y - double(y1) + 2;

		Y = getWeight(u_y);

		for (x = 0; x < width_out; x++)
		{

			double src_x = (x + 0.5) / 4 -0.5;
			size_t x1 = floor((x + 0.5) / 4 - 0.5) + 2;


			// size_t y1 = floor(src_y);
			size_t x0 = x1 - 1;
			size_t x2 = x1 + 1;
			size_t x3 = x1 + 2;


			const unsigned char up = 255;
			const unsigned char down = 0;



			u_x = src_x - double(x1) + 2 ;
			X = getWeight(u_x);

			temp_r =
				  double(Y[0] * X[0] * int(img_p.img_pixels[y0][x0].red)) + double(Y[0] * X[1] * int(img_p.img_pixels[y0][x1].red)) + double(Y[0] * X[2] * int(img_p.img_pixels[y0][x2].red)) + double(Y[0] * X[3] * int(img_p.img_pixels[y0][x3].red))
				+ double(Y[1] * X[0] * int(img_p.img_pixels[y1][x0].red)) + double(Y[1] * X[1] * int(img_p.img_pixels[y1][x1].red)) + double(Y[1] * X[2] * int(img_p.img_pixels[y1][x2].red)) + double(Y[1] * X[3] * int(img_p.img_pixels[y1][x3].red))
				+ double(Y[2] * X[0] * int(img_p.img_pixels[y2][x0].red)) + double(Y[2] * X[1] * int(img_p.img_pixels[y2][x1].red)) + double(Y[2] * X[2] * int(img_p.img_pixels[y2][x2].red)) + double(Y[2] * X[3] * int(img_p.img_pixels[y2][x3].red))
				+ double(Y[3] * X[0] * int(img_p.img_pixels[y3][x0].red)) + double(Y[3] * X[1] * int(img_p.img_pixels[y3][x1].red)) + double(Y[3] * X[2] * int(img_p.img_pixels[y3][x2].red)) + double(Y[3] * X[3] * int(img_p.img_pixels[y3][x3].red))
				;


			if (temp_r > 255) temp_r = 255;
			if (temp_r < 0) temp_r = 0;

			out->img_pixels[y][x].red = unsigned char(temp_r);


			temp_b =
				  double(Y[0] * X[0] * int(img_p.img_pixels[y0][x0].blue)) + double(Y[0] * X[1] * int(img_p.img_pixels[y0][x1].blue)) + double(Y[0] * X[2] * int(img_p.img_pixels[y0][x2].blue)) + double(Y[0] * X[3] * int(img_p.img_pixels[y0][x3].blue))
				+ double(Y[1] * X[0] * int(img_p.img_pixels[y1][x0].blue)) + double(Y[1] * X[1] * int(img_p.img_pixels[y1][x1].blue)) + double(Y[1] * X[2] * int(img_p.img_pixels[y1][x2].blue)) + double(Y[1] * X[3] * int(img_p.img_pixels[y1][x3].blue))
				+ double(Y[2] * X[0] * int(img_p.img_pixels[y2][x0].blue)) + double(Y[2] * X[1] * int(img_p.img_pixels[y2][x1].blue)) + double(Y[2] * X[2] * int(img_p.img_pixels[y2][x2].blue)) + double(Y[2] * X[3] * int(img_p.img_pixels[y2][x3].blue))
				+ double(Y[3] * X[0] * int(img_p.img_pixels[y3][x0].blue)) + double(Y[3] * X[1] * int(img_p.img_pixels[y3][x1].blue)) + double(Y[3] * X[2] * int(img_p.img_pixels[y3][x2].blue)) + double(Y[3] * X[3] * int(img_p.img_pixels[y3][x3].blue));
			
			/*
			out_p->img_pixels[y][x].blue = Y[0] * X[0] * img_p.img_pixels[y0][x0].blue + Y[0] * X[1] * img_p.img_pixels[y0][x1].blue + Y[0] * X[2] * img_p.img_pixels[y0][x2].blue + Y[0] * X[3] * img_p.img_pixels[y0][x3].blue
				+ Y[1] * X[0] * img_p.img_pixels[y1][x0].blue + Y[1] * X[1] * img_p.img_pixels[y1][x1].blue + Y[1] * X[2] * img_p.img_pixels[y1][x2].blue + Y[1] * X[3] * img_p.img_pixels[y1][x3].blue
				+ Y[2] * X[0] * img_p.img_pixels[y2][x0].blue + Y[2] * X[1] * img_p.img_pixels[y2][x1].blue + Y[2] * X[2] * img_p.img_pixels[y2][x2].blue + Y[2] * X[3] * img_p.img_pixels[y2][x3].blue
				+ Y[3] * X[0] * img_p.img_pixels[y3][x0].blue + Y[3] * X[1] * img_p.img_pixels[y3][x1].blue + Y[3] * X[2] * img_p.img_pixels[y3][x2].blue + Y[3] * X[3] * img_p.img_pixels[y3][x3].blue;
			*/

			if (temp_b > up) temp_b = 255;
			if (temp_b < down)temp_b = 0;

			out->img_pixels[y][x].blue = temp_b;

			temp_g =
				double(Y[0] * X[0] * int(img_p.img_pixels[y0][x0].green)) + double(Y[0] * X[1] * int(img_p.img_pixels[y0][x1].green)) + double(Y[0] * X[2] * int(img_p.img_pixels[y0][x2].green)) + double(Y[0] * X[3] * int(img_p.img_pixels[y0][x3].green))
				+ double(Y[1] * X[0] * int(img_p.img_pixels[y1][x0].green)) + double(Y[1] * X[1] * int(img_p.img_pixels[y1][x1].green)) + double(Y[1] * X[2] * int(img_p.img_pixels[y1][x2].green)) + double(Y[1] * X[3] * int(img_p.img_pixels[y1][x3].green))
				+ double(Y[2] * X[0] * int(img_p.img_pixels[y2][x0].green)) + double(Y[2] * X[1] * int(img_p.img_pixels[y2][x1].green)) + double(Y[2] * X[2] * int(img_p.img_pixels[y2][x2].green)) + double(Y[2] * X[3] * int(img_p.img_pixels[y2][x3].green))
				+ double(Y[3] * X[0] * int(img_p.img_pixels[y3][x0].green)) + double(Y[3] * X[1] * int(img_p.img_pixels[y3][x1].green)) + double(Y[3] * X[2] * int(img_p.img_pixels[y3][x2].green)) + double(Y[3] * X[3] * int(img_p.img_pixels[y3][x3].green));


			if (temp_g > up) temp_g = 255;
			if (temp_g < down) temp_g = 0;

			out->img_pixels[y][x].green = temp_g;


		}
		printf("Processing rows %d ... \n", int(y));

	}

	printf("Stage 2 : SUCCESSFUL!\n");




	printf("Stage 3(OUTPUT) : START...\n");


	bmp_img_write(out, "results/1.bmp");



	printf("Stage 3(OUTPUT) : SUCCESSFUL\n");


	bmp_img_free(&img_p);
	return;
}







void* multi_inter(void* vargp)
{


	// Draw a checkerboard pattern:
	uint32_t width_in = 960;
	uint32_t height_in = 540;
	uint32_t width_out = 3840;
	uint32_t height_out = 2160;



	int s = * (int*) vargp;
	int i = s;// i 是 目前的项目的序号

	bmp_img in;
	bmp_img out;
	bmp_img* out_p = &out;
	size_t ex = 4;


	char symbol[20] = "0123456789";
	char tar_dir[30] = "target/"; // 目录名
	char res_dir[30] = "multires/";
	char tail_1[10] = "0.bmp\0";

	char tail_2[10] = "10.bmp\0";
	//char res[30] = "multires/0.bmp";

	//res[9] = symbol[i];



	char* ami = (char*)malloc(strlen(tar_dir) + strlen(tail_1));
	char* res = (char*)malloc(strlen(res_dir) + strlen(tail_1));
	strcpy(ami, tar_dir);
	strcpy(res, res_dir);

	//strcat(ami,  & symbol[i]);


	if (i < 10)
	{	
		tail_1[0] = symbol[i];
		strcat(ami, tail_1);
		strcat(res, tail_1);
	}
	else {

		tail_2[0] = symbol[i / 10];
		tail_2[1] = symbol[i % 10] ;
		strcat(ami, tail_2);
		strcat(res, tail_2);
	}



	//strcat(res, &symbol[i]);

	// char ami[30] = "target/1.bmp" ;


	/*
	if (i < 10)
	{
		ami = "target/1.bmp";
		ami[7] = symbol[i];
 }
*/

	bmp_img_init_df(&in, width_in, height_in);
	bmp_img_init_df(&out, width_in * ex, height_in * ex);


	bmp_img_read(&in, ami);


	printf("%s : \t",ami);
	printf("Stage 1(PADDING) : START...\n");






	bmp_img img_p;
	bmp_img_init_df(&img_p, width_in + 4, height_in + 4);
	bmp_padding_2_repeat(&in, &img_p);

	// bmp_img_write(&img_p, "results/padding_test_0.bmp");

	//bmp_value_print(&in);
	//bmp_value_print(& img_p);

	printf("Stage 1(PADDING) : SUCCESSFUL!\n");
	printf("Stage 2(inter) : START...\n");




	double* Y;
	double* X;
	double u_y, u_x;


	double temp_r;
	double temp_g;
	double temp_b;




	for (size_t y = 0, x; y < height_out; y++)
	{
		//double src_y = (y + 0.5) / 4 -0.5 ;
		////if (src_y < 0) src_y = 0;
		////if (src_y > height_out - 1) src_y = height_out - 1;
		//src_y += 2;



		double src_y = (y + 0.5) / 4 + 0.25;
		size_t y1 = floor((y + 0.5) / 4 + 0.25) + 1;





		// size_t y1 = floor(src_y);
		size_t y0 = y1 - 1;
		size_t y2 = y1 + 1;
		size_t y3 = y1 + 2;

		u_y = src_y - double(y1) + 1;

		Y = getWeight(u_y);



		for (x = 0; x < width_out; x++)
		{
			double src_x = (x + 0.5) / 4 + 0.25;
			size_t x1 = floor((x + 0.5) / 4 + 0.25) + 1;


			// size_t y1 = floor(src_y);
			size_t x0 = x1 - 1;
			size_t x2 = x1 + 1;
			size_t x3 = x1 + 2;


			const unsigned char up = 255;
			const unsigned char down = 0;

			u_x = src_x - double(x1) + 1;
			X = getWeight(u_x);

			/*
			out_p->img_pixels[y][x].red = Y[0] * X[0] * img_p.img_pixels[y0][x0].red + Y[0] * X[1] * img_p.img_pixels[y0][x1].red + Y[0] * X[2] * img_p.img_pixels[y0][x2].red + Y[0] * X[3] * img_p.img_pixels[y0][x3].red
				+ Y[1] * X[0] * img_p.img_pixels[y1][x0].red + Y[1] * X[1] * img_p.img_pixels[y1][x1].red + Y[1] * X[2] * img_p.img_pixels[y1][x2].red + Y[1] * X[3] * img_p.img_pixels[y1][x3].red
				+ Y[2] * X[0] * img_p.img_pixels[y2][x0].red + Y[2] * X[1] * img_p.img_pixels[y2][x1].red + Y[2] * X[2] * img_p.img_pixels[y2][x2].red + Y[2] * X[3] * img_p.img_pixels[y2][x3].red
				+ Y[3] * X[0] * img_p.img_pixels[y3][x0].red + Y[3] * X[1] * img_p.img_pixels[y3][x1].red + Y[3] * X[2] * img_p.img_pixels[y3][x2].red + Y[3] * X[3] * img_p.img_pixels[y3][x3].red
				;
*/

			temp_r =
				double(Y[0] * X[0] * int(img_p.img_pixels[y0][x0].red)) + double(Y[0] * X[1] * int(img_p.img_pixels[y0][x1].red)) + double(Y[0] * X[2] * int(img_p.img_pixels[y0][x2].red)) + double(Y[0] * X[3] * int(img_p.img_pixels[y0][x3].red))
				+ double(Y[1] * X[0] * int(img_p.img_pixels[y1][x0].red)) + double(Y[1] * X[1] * int(img_p.img_pixels[y1][x1].red)) + double(Y[1] * X[2] * int(img_p.img_pixels[y1][x2].red)) + double(Y[1] * X[3] * int(img_p.img_pixels[y1][x3].red))
				+ double(Y[2] * X[0] * int(img_p.img_pixels[y2][x0].red)) + double(Y[2] * X[1] * int(img_p.img_pixels[y2][x1].red)) + double(Y[2] * X[2] * int(img_p.img_pixels[y2][x2].red)) + double(Y[2] * X[3] * int(img_p.img_pixels[y2][x3].red))
				+ double(Y[3] * X[0] * int(img_p.img_pixels[y3][x0].red)) + double(Y[3] * X[1] * int(img_p.img_pixels[y3][x1].red)) + double(Y[3] * X[2] * int(img_p.img_pixels[y3][x2].red)) + double(Y[3] * X[3] * int(img_p.img_pixels[y3][x3].red))
				;


			if (temp_r > 255) temp_r = 255;
			if (temp_r < 0) temp_r = 0;

			out_p->img_pixels[y][x].red = unsigned char(temp_r);


			temp_b =
				double(Y[0] * X[0] * int(img_p.img_pixels[y0][x0].blue)) + double(Y[0] * X[1] * int(img_p.img_pixels[y0][x1].blue)) + double(Y[0] * X[2] * int(img_p.img_pixels[y0][x2].blue)) + double(Y[0] * X[3] * int(img_p.img_pixels[y0][x3].blue))
				+ double(Y[1] * X[0] * int(img_p.img_pixels[y1][x0].blue)) + double(Y[1] * X[1] * int(img_p.img_pixels[y1][x1].blue)) + double(Y[1] * X[2] * int(img_p.img_pixels[y1][x2].blue)) + double(Y[1] * X[3] * int(img_p.img_pixels[y1][x3].blue))
				+ double(Y[2] * X[0] * int(img_p.img_pixels[y2][x0].blue)) + double(Y[2] * X[1] * int(img_p.img_pixels[y2][x1].blue)) + double(Y[2] * X[2] * int(img_p.img_pixels[y2][x2].blue)) + double(Y[2] * X[3] * int(img_p.img_pixels[y2][x3].blue))
				+ double(Y[3] * X[0] * int(img_p.img_pixels[y3][x0].blue)) + double(Y[3] * X[1] * int(img_p.img_pixels[y3][x1].blue)) + double(Y[3] * X[2] * int(img_p.img_pixels[y3][x2].blue)) + double(Y[3] * X[3] * int(img_p.img_pixels[y3][x3].blue));

			/*
			out_p->img_pixels[y][x].blue = Y[0] * X[0] * img_p.img_pixels[y0][x0].blue + Y[0] * X[1] * img_p.img_pixels[y0][x1].blue + Y[0] * X[2] * img_p.img_pixels[y0][x2].blue + Y[0] * X[3] * img_p.img_pixels[y0][x3].blue
				+ Y[1] * X[0] * img_p.img_pixels[y1][x0].blue + Y[1] * X[1] * img_p.img_pixels[y1][x1].blue + Y[1] * X[2] * img_p.img_pixels[y1][x2].blue + Y[1] * X[3] * img_p.img_pixels[y1][x3].blue
				+ Y[2] * X[0] * img_p.img_pixels[y2][x0].blue + Y[2] * X[1] * img_p.img_pixels[y2][x1].blue + Y[2] * X[2] * img_p.img_pixels[y2][x2].blue + Y[2] * X[3] * img_p.img_pixels[y2][x3].blue
				+ Y[3] * X[0] * img_p.img_pixels[y3][x0].blue + Y[3] * X[1] * img_p.img_pixels[y3][x1].blue + Y[3] * X[2] * img_p.img_pixels[y3][x2].blue + Y[3] * X[3] * img_p.img_pixels[y3][x3].blue;
			*/

			if (temp_b > up) temp_b = 255;
			if (temp_b < down)temp_b = 0;

			out_p->img_pixels[y][x].blue = temp_b;

			temp_g =
				double(Y[0] * X[0] * int(img_p.img_pixels[y0][x0].green)) + double(Y[0] * X[1] * int(img_p.img_pixels[y0][x1].green)) + double(Y[0] * X[2] * int(img_p.img_pixels[y0][x2].green)) + double(Y[0] * X[3] * int(img_p.img_pixels[y0][x3].green))
				+ double(Y[1] * X[0] * int(img_p.img_pixels[y1][x0].green)) + double(Y[1] * X[1] * int(img_p.img_pixels[y1][x1].green)) + double(Y[1] * X[2] * int(img_p.img_pixels[y1][x2].green)) + double(Y[1] * X[3] * int(img_p.img_pixels[y1][x3].green))
				+ double(Y[2] * X[0] * int(img_p.img_pixels[y2][x0].green)) + double(Y[2] * X[1] * int(img_p.img_pixels[y2][x1].green)) + double(Y[2] * X[2] * int(img_p.img_pixels[y2][x2].green)) + double(Y[2] * X[3] * int(img_p.img_pixels[y2][x3].green))
				+ double(Y[3] * X[0] * int(img_p.img_pixels[y3][x0].green)) + double(Y[3] * X[1] * int(img_p.img_pixels[y3][x1].green)) + double(Y[3] * X[2] * int(img_p.img_pixels[y3][x2].green)) + double(Y[3] * X[3] * int(img_p.img_pixels[y3][x3].green));


			if (temp_g > up) temp_g = 255;
			if (temp_g < down) temp_g = 0;

			out_p->img_pixels[y][x].green = temp_g;


		}

		// printf("Processing rows %d ... \n", int(y));

	}

	printf("Stage 2 : SUCCESSFUL!\n");




	printf("Stage 3(OUTPUT) : START...\n");


	bmp_img_write(out_p, res);

	printf("%s : \t",res);
	printf("Stage 3(OUTPUT) : SUCCESSFUL\n");
	bmp_img_free(&img_p);

	return 0;
}



void multi_set(uint32_t num_instance)

{
	int i;
	pthread_t tid;
	int* num;
	

	num = (int*)calloc(num_instance, sizeof(int));
	
	for (i = 0; i < num_instance; i++)
	{
		num[i] = i;
	}

	for (i = 0; i < num_instance; i++)


		pthread_create(&tid, NULL, multi_inter, &num[i]);

	pthread_exit(NULL);

	free(num);
	return;


}

int main()
{

	/*
	bmp_img img_in;
	bmp_img img_out;
	// Draw a checkerboard pattern:
	uint32_t width_in = 960;
	uint32_t height_in = 540;

	size_t ex = 4;

	bmp_img_init_df(&img_in, width_in, height_in);
	bmp_img_init_df(&img_out, width_in * ex, height_in * ex);
	bmp_img_read(&img_in, "target/1.bmp");

	bicubic_inter(img_in, width_in, height_in, &img_out, width_in * ex, height_in * ex, 1);

	//bmp_img_write(&img_out, "results/1.bmp");


	bmp_img_free(&img_out);
	bmp_img_free(&img_in);

	
	printf("exit!\n");
*/

	//printf("start mul!");


	multi_set(47);


	return 0;
}
