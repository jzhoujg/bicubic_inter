#include <stdio.h>
#include <stdlib.h>

#ifndef __LIBBMP_H__
#define __LIBBMP_H__

#define BMP_MAGIC 19778

#define BMP_GET_PADDING(a) ((a) % 4)

enum bmp_error
{
	BMP_FILE_NOT_OPENED = -4,
	BMP_HEADER_NOT_INITIALIZED,
	BMP_INVALID_FILE,
	BMP_ERROR,
	BMP_OK = 0
};

typedef struct _bmp_header
{
	unsigned int   bfSize;
	unsigned int   bfReserved;
	unsigned int   bfOffBits;

	unsigned int   biSize;
	int            biWidth;
	int            biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int   biCompression;
	unsigned int   biSizeImage;
	int            biXPelsPerMeter;
	int            biYPelsPerMeter;
	unsigned int   biClrUsed;
	unsigned int   biClrImportant;
} bmp_header;

typedef struct _bmp_pixel
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
} bmp_pixel;

// This is faster than a function call
#define BMP_PIXEL(r,g,b) ((bmp_pixel){(b),(g),(r)})

typedef struct _bmp_img
{
	bmp_header   img_header;
	bmp_pixel** img_pixels;
} bmp_img;

// BMP_HEADER
void            bmp_header_init_df(bmp_header*,
	const int,
	const int);

enum bmp_error  bmp_header_write(const bmp_header*,
	FILE*);

enum bmp_error  bmp_header_read(bmp_header*,
	FILE*);

// BMP_PIXEL
void            bmp_pixel_init(bmp_pixel*,
	const unsigned char,
	const unsigned char,
	const unsigned char);

// BMP_IMG
void            bmp_img_alloc(bmp_img*);
void            bmp_img_init_df(bmp_img*,
	const int,
	const int);
void            bmp_img_free(bmp_img*);

enum bmp_error  bmp_img_write(const bmp_img*,
	const char*);

enum bmp_error  bmp_img_read(bmp_img*,
	const char*);

void
bmp_header_init_df(bmp_header* header,
	const int   width,
	const int   height)
{
	header->bfSize = (sizeof(bmp_pixel) * width + BMP_GET_PADDING(width))
		* abs(height);
	header->bfReserved = 0;
	header->bfOffBits = 54;
	header->biSize = 40;
	header->biWidth = width;
	header->biHeight = height;
	header->biPlanes = 1;
	header->biBitCount = 24;
	header->biCompression = 0;
	header->biSizeImage = 0;
	header->biXPelsPerMeter = 0;
	header->biYPelsPerMeter = 0;
	header->biClrUsed = 0;
	header->biClrImportant = 0;
}

enum bmp_error
	bmp_header_write(const bmp_header* header,
		FILE* img_file)
{
	if (header == NULL)
	{
		return BMP_HEADER_NOT_INITIALIZED;
	}
	else if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}

	// Since an adress must be passed to fwrite, create a variable!
	const unsigned short magic = BMP_MAGIC;
	fwrite(&magic, sizeof(magic), 1, img_file);

	// Use the type instead of the variable because its a pointer!
	fwrite(header, sizeof(bmp_header), 1, img_file);
	return BMP_OK;
}

enum bmp_error
	bmp_header_read(bmp_header* header,
		FILE* img_file)
{
	if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}

	// Since an adress must be passed to fread, create a variable!
	unsigned short magic;

	// Check if its an bmp file by comparing the magic nbr:
	if (fread(&magic, sizeof(magic), 1, img_file) != 1 ||
		magic != BMP_MAGIC)
	{
		return BMP_INVALID_FILE;
	}

	if (fread(header, sizeof(bmp_header), 1, img_file) != 1)
	{
		return BMP_ERROR;
	}

	return BMP_OK;
}

// BMP_PIXEL

void
bmp_pixel_init(bmp_pixel* pxl,
	const unsigned char  red,
	const unsigned char  green,
	const unsigned char  blue)
{
	pxl->red = red;
	pxl->green = green;
	pxl->blue = blue;
}

// BMP_IMG

void
bmp_img_alloc(bmp_img* img)
{
	const size_t h = abs(img->img_header.biHeight);

	// Allocate the required memory for the pixels:
	img->img_pixels = (bmp_pixel **)malloc(sizeof(bmp_pixel*) * h);

	for (size_t y = 0; y < h; y++)
	{
		img->img_pixels[y] = (bmp_pixel*)malloc(sizeof(bmp_pixel) * img->img_header.biWidth);
	}
}

void
bmp_img_init_df(bmp_img* img,
	const int  width,
	const int  height)
{
	// INIT the header with default values:
	bmp_header_init_df(&img->img_header, width, height);
	bmp_img_alloc(img);
}

void
bmp_img_free(bmp_img* img)
{
	const size_t h = abs(img->img_header.biHeight);

	for (size_t y = 0; y < h; y++)
	{
		free(img->img_pixels[y]);
	}
	free(img->img_pixels);
}

enum bmp_error
	bmp_img_write(const bmp_img* img,
		const char* filename)
{
	FILE* img_file = fopen(filename, "wb");

	if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}

	// NOTE: This way the correct error code could be returned.
	const enum bmp_error err = bmp_header_write(&img->img_header, img_file);

	if (err != BMP_OK)
	{
		// ERROR: Could'nt write the header!
		fclose(img_file);
		return err;
	}

	// Select the mode (bottom-up or top-down):
	const size_t h = abs(img->img_header.biHeight);
	const size_t offset = (img->img_header.biHeight > 0 ? h - 1 : 0);

	// Create the padding:
	const unsigned char padding[3] = { '\0', '\0', '\0' };

	// Write the content:
	for (size_t y = 0; y < h; y++)
	{
		// Write a whole row of pixels to the file:
		fwrite(img->img_pixels[abs(int(offset - y))], sizeof(bmp_pixel), img->img_header.biWidth, img_file);

		// Write the padding for the row!
		fwrite(padding, sizeof(unsigned char), BMP_GET_PADDING(img->img_header.biWidth), img_file);
	}

	// NOTE: All good!
	fclose(img_file);
	return BMP_OK;
}

enum bmp_error
	bmp_img_read(bmp_img* img,
		const char* filename)
{
	FILE* img_file = fopen(filename, "rb");

	if (img_file == NULL)
	{
		return BMP_FILE_NOT_OPENED;
	}

	// NOTE: This way the correct error code can be returned.
	const enum bmp_error err = bmp_header_read(&img->img_header, img_file);

	if (err != BMP_OK)
	{
		// ERROR: Could'nt read the image header!
		fclose(img_file);
		return err;
	}

	bmp_img_alloc(img);

	// Select the mode (bottom-up or top-down):
	const size_t h = abs(img->img_header.biHeight);
	const size_t offset = (img->img_header.biHeight > 0 ? h - 1 : 0);
	const size_t padding = BMP_GET_PADDING(img->img_header.biWidth);

	// Needed to compare the return value of fread
	const size_t items = img->img_header.biWidth;

	// Read the content:
	for (size_t y = 0; y < h; y++)
	{
		// Read a whole row of pixels from the file:
		if (fread(img->img_pixels[abs(int(offset - y))], sizeof(bmp_pixel), items, img_file) != items)
		{
			fclose(img_file);
			return BMP_ERROR;
		}

		// Skip the padding:
		fseek(img_file, padding, SEEK_CUR);
	}

	// NOTE: All good!
	fclose(img_file);
	return BMP_OK;
}




enum bmp_error bmp_value_print(bmp_img* img)

{

	printf("\n");
	size_t h = img->img_header.biHeight;
	size_t w = img->img_header.biWidth;

	if (h == 0 || w == 0)
	{
		return BMP_ERROR;
	}

	printf("The height is :%d\n", int(h));
	printf("The width is £º%d\n", int(w));
	printf("The Matrix(r) is : \n\t");

	for (size_t y = 0, x; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			printf("%d\t", int(img->img_pixels[y][x].red));

		}
		printf("\n\t");

	}


	printf("\n");
	return BMP_OK;

}

void bmp_value_copy(bmp_img* img_p, size_t y_p, size_t x_p, bmp_img* img_o, size_t y_o, size_t x_o)
{

	img_p->img_pixels[y_p][x_p].red = img_o->img_pixels[y_o][x_o].red;
	img_p->img_pixels[y_p][x_p].blue = img_o->img_pixels[y_o][x_o].blue;
	img_p->img_pixels[y_p][x_p].green = img_o->img_pixels[y_o][x_o].green;

	return;

}

void bmp_padding_2_repeat(bmp_img* img_o, bmp_img* img_p)

{

	int width = int (img_o->img_header.biWidth);
	int height = int (img_o->img_header.biHeight);


	for (size_t y = 0, x; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			//img_p->img_pixels[y + 2][x + 2].red = img_o->img_pixels[y][x].red;
			//img_p->img_pixels[y + 2][x + 2].blue = img_o->img_pixels[y][x].blue;
			//img_p->img_pixels[y + 2][x + 2].green = img_o->img_pixels[y][x].green;

			bmp_value_copy(img_p, y + 2, x + 2, img_o, y, x);

		}

	}

	// corner
	// 
	//(0,0)
	bmp_value_copy(img_p, 0, 0, img_o, 0, 0);
	bmp_value_copy(img_p, 1, 0, img_o, 0, 0);
	bmp_value_copy(img_p, 0, 1, img_o, 0, 0);
	bmp_value_copy(img_p, 1, 1, img_o, 0, 0);

	//(0,w)
	bmp_value_copy(img_p, 0, width+2, img_o, 0, width-1);
	bmp_value_copy(img_p, 1, width+2, img_o, 0, width-1);
	bmp_value_copy(img_p, 0, width+3, img_o, 0, width-1);
	bmp_value_copy(img_p, 1, width+3, img_o, 0, width-1);



	//(h,0)
	bmp_value_copy(img_p, height + 2, 0, img_o, height - 1, 0);
	bmp_value_copy(img_p, height + 3, 0, img_o, height - 1, 0);
	bmp_value_copy(img_p, height + 2, 1, img_o, height - 1, 0);
	bmp_value_copy(img_p, height + 3, 1, img_o, height - 1, 0);


	//(h,w)
	bmp_value_copy(img_p, height + 2, width + 2, img_o, height - 1, width - 1);
	bmp_value_copy(img_p, height + 3, width + 2, img_o, height - 1, width - 1);
	bmp_value_copy(img_p, height + 2, width + 3, img_o, height - 1, width - 1);
	bmp_value_copy(img_p, height + 3, width + 3, img_o, height - 1, width - 1);


// upper



	for (size_t x = 0; x < width; x++)
		{
			//img_p->img_pixels[y + 2][x + 2].red = img_o->img_pixels[y][x].red;
			//img_p->img_pixels[y + 2][x + 2].blue = img_o->img_pixels[y][x].blue;
			//img_p->img_pixels[y + 2][x + 2].green = img_o->img_pixels[y][x].green;

			bmp_value_copy(img_p, 0, x + 2, img_o, 0, x);
			bmp_value_copy(img_p, 1, x + 2, img_o, 0, x);
		}

// down
	for (size_t x = 0; x < width; x++)
	{
		//img_p->img_pixels[y + 2][x + 2].red = img_o->img_pixels[y][x].red;
		//img_p->img_pixels[y + 2][x + 2].blue = img_o->img_pixels[y][x].blue;
		//img_p->img_pixels[y + 2][x + 2].green = img_o->img_pixels[y][x].green;

		bmp_value_copy(img_p, height + 2, x + 2, img_o, height - 1, x);
		bmp_value_copy(img_p, height + 3, x + 2, img_o, height - 1, x);

	}

// left 
	for (size_t y = 0; y < height; y++)
	{
		//img_p->img_pixels[y + 2][x + 2].red = img_o->img_pixels[y][x].red;
		//img_p->img_pixels[y + 2][x + 2].blue = img_o->img_pixels[y][x].blue;
		//img_p->img_pixels[y + 2][x + 2].green = img_o->img_pixels[y][x].green;

		bmp_value_copy(img_p, y+2, 0, img_o, y, 0);
		bmp_value_copy(img_p, y+2, 1, img_o, y, 0);

	}

// right 

	for (size_t y = 0; y < height; y++)
	{
		//img_p->img_pixels[y + 2][x + 2].red = img_o->img_pixels[y][x].red;
		//img_p->img_pixels[y + 2][x + 2].blue = img_o->img_pixels[y][x].blue;
		//img_p->img_pixels[y + 2][x + 2].green = img_o->img_pixels[y][x].green;

		bmp_value_copy(img_p, y + 2, width + 2, img_o, y, width - 1);
		bmp_value_copy(img_p, y + 2, width + 3, img_o, y, width - 1);

	}








	return;

}



#endif /* __LIBBMP_H__ */
