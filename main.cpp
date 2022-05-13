#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "libbmp.h"



double dabs(double c)

{

	if (c >= 0) return c;
	return -1 * c;


}



double* getWeight(double c, double a = -0.55)
{
	//c就是u和v，横坐标和纵坐标的输出计算方式一样
	float temp[4];

	temp[0] = c + 1 ; 
	temp[1] = c;
	temp[2] = 1 - c; 
	temp[3] = 2 - c;

	//printf("%f \t", float(temp[3]));

	//y(x) = (a+2)|x|*|x|*|x| - (a+3)|x|*|x| + 1   |x|<=1
	//y(x) = a|x|*|x|*|x| - 5a|x|*|x| + 8a|x| - 4a  1<|x|<2
	double weight[4];
	weight[0] = (a * pow(dabs(temp[0]), 3) - 5 * a * pow(dabs(temp[0]), 2) + 8 * a * dabs(temp[0]) - 4 * a);
	weight[1] = (a + 2) * pow(dabs(temp[1]), 3) - (a + 3) * pow(dabs(temp[1]), 2) + 1 ;
	weight[2] = (a + 2) * pow(dabs(temp[2]), 3) - (a + 3) * pow(dabs(temp[2]), 2) + 1 ;
	weight[3] = (a * pow(dabs(temp[3]), 3) - 5 * a * pow(dabs(temp[3]), 2) + 8 * a * dabs(temp[3]) - 4 * a);






	// printf("%f \t", float(weight[2]));

	return weight;
}

void bicubic_inter(bmp_img in, uint32_t width_in, uint32_t height_in, bmp_img* out, uint32_t width_out, uint32_t height_out, uint32_t num_instance)
{
	// Draw a checkerboard pattern:

	printf("Stage 1(PADDING) : START...\n");
	bmp_img img_p;
	bmp_img_init_df(&img_p, width_in+4, height_in+4);
	bmp_padding_2_repeat(&in,&img_p);

	bmp_img_write(&img_p, "results/padding_test_0.bmp");

	//bmp_value_print(&in);
	//bmp_value_print(& img_p);

	printf("Stage 1(PADDING) : SUCCESSFUL!\n");
	printf("Stage 2(inter) : START...\n");


	double* Y;
	double* X;
	double u_y,u_x;

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

			out->img_pixels[y][x].red = Y[0] * X[0] * img_p.img_pixels[y0][x0].red + Y[0] * X[1] * img_p.img_pixels[y0][x1].red + Y[0] * X[2] * img_p.img_pixels[y0][x2].red + Y[0] * X[3] * img_p.img_pixels[y0][x3].red
									  + Y[1] * X[0] * img_p.img_pixels[y1][x0].red + Y[1] * X[1] * img_p.img_pixels[y1][x1].red + Y[1] * X[2] * img_p.img_pixels[y1][x2].red + Y[1] * X[3] * img_p.img_pixels[y1][x3].red
									  + Y[2] * X[0] * img_p.img_pixels[y2][x0].red + Y[2] * X[1] * img_p.img_pixels[y2][x1].red + Y[2] * X[2] * img_p.img_pixels[y2][x2].red + Y[2] * X[3] * img_p.img_pixels[y2][x3].red
									  + Y[3] * X[0] * img_p.img_pixels[y3][x0].red + Y[3] * X[1] * img_p.img_pixels[y3][x1].red + Y[3] * X[2] * img_p.img_pixels[y3][x2].red + Y[3] * X[3] * img_p.img_pixels[y3][x3].red
				;

			if (out->img_pixels[y][x].red > up) out->img_pixels[y][x].red = 255;
			if (out->img_pixels[y][x].red < down) out->img_pixels[y][x].red = 0;


			out->img_pixels[y][x].blue = Y[0] * X[0] * img_p.img_pixels[y0][x0].blue + Y[0] * X[1] * img_p.img_pixels[y0][x1].blue + Y[0] * X[2] * img_p.img_pixels[y0][x2].blue + Y[0] * X[3] * img_p.img_pixels[y0][x3].blue
									   + Y[1] * X[0] * img_p.img_pixels[y1][x0].blue + Y[1] * X[1] * img_p.img_pixels[y1][x1].blue + Y[1] * X[2] * img_p.img_pixels[y1][x2].blue + Y[1] * X[3] * img_p.img_pixels[y1][x3].blue
									   + Y[2] * X[0] * img_p.img_pixels[y2][x0].blue + Y[2] * X[1] * img_p.img_pixels[y2][x1].blue + Y[2] * X[2] * img_p.img_pixels[y2][x2].blue + Y[2] * X[3] * img_p.img_pixels[y2][x3].blue
									   + Y[3] * X[0] * img_p.img_pixels[y3][x0].blue + Y[3] * X[1] * img_p.img_pixels[y3][x1].blue + Y[3] * X[2] * img_p.img_pixels[y3][x2].blue + Y[3] * X[3] * img_p.img_pixels[y3][x3].blue;
			if (out->img_pixels[y][x].blue > up) out->img_pixels[y][x].blue = 255;
			if (out->img_pixels[y][x].blue < down) out->img_pixels[y][x].blue = 0;



			out->img_pixels[y][x].green = Y[0] * X[0] * img_p.img_pixels[y0][x0].green + Y[0] * X[1] * img_p.img_pixels[y0][x1].green + Y[0] * X[2] * img_p.img_pixels[y0][x2].green + Y[0] * X[3] * img_p.img_pixels[y0][x3].green
									    + Y[1] * X[0] * img_p.img_pixels[y1][x0].green + Y[1] * X[1] * img_p.img_pixels[y1][x1].green + Y[1] * X[2] * img_p.img_pixels[y1][x2].green + Y[1] * X[3] * img_p.img_pixels[y1][x3].green
									    + Y[2] * X[0] * img_p.img_pixels[y2][x0].green + Y[2] * X[1] * img_p.img_pixels[y2][x1].green + Y[2] * X[2] * img_p.img_pixels[y2][x2].green + Y[2] * X[3] * img_p.img_pixels[y2][x3].green
									    + Y[3] * X[0] * img_p.img_pixels[y3][x0].green + Y[3] * X[1] * img_p.img_pixels[y3][x1].green + Y[3] * X[2] * img_p.img_pixels[y3][x2].green + Y[3] * X[3] * img_p.img_pixels[y3][x3].green;
			if (out->img_pixels[y][x].green > up) out->img_pixels[y][x].green = 255;
			if (out->img_pixels[y][x].green < down) out->img_pixels[y][x].green = 0;


		}
		printf("Processing rows %d ... \n",int(y));

	}

	printf("Stage 2 : SUCCESSFUL!\n");

	printf("Stage 3(OUTPUT) : START...\n");


	bmp_img_write(out, "results/1.bmp");



	printf("Stage 3(OUTPUT) : SUCCESSFUL\n");


	bmp_img_free(&img_p);
	return;
}
int main()
{

	bmp_img img_in;
	bmp_img img_out;
	// Draw a checkerboard pattern:
	uint32_t width_in = 960;
	uint32_t height_in = 540;

	size_t ex = 4;

	bmp_img_init_df(&img_in, width_in, height_in);
	bmp_img_init_df(&img_out, width_in*ex, height_in*ex);
	bmp_img_read(&img_in, "target/1.bmp");

	bicubic_inter(img_in, width_in, height_in, &img_out, width_in * ex, height_in * ex,1);

	//bmp_img_write(&img_out, "results/1.bmp");


	bmp_img_free(&img_out);
	bmp_img_free(&img_in);


	
	printf("exit!\n");


	return 0;
}