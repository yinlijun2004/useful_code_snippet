#include <stdio.h>
#include <stdint.h>
#include <math.h>

typedef struct _BinaryBitmap
{
	uint32_t width;
	uint32_t height;
	uint8_t* buffer;
	uint32_t line_width;
}BinaryBitmap;

BinaryBitmap* binary_bitmap_create(uint32_t width, uint32_t height)
{
	BinaryBitmap* thiz;

	if(width == 0 || height == 0)
	{
		return NULL;
	}
	if(width % 8 != 0)
	{
		return NULL;
	}
	thiz = malloc(sizeof(BinaryBitmap));
	if(thiz == NULL) 
	{
		return NULL;
	}
	thiz->width = width / 8;
	thiz->height = height;
	thiz->buffer = (uint8_t *)malloc(width * height);
	if(thiz->buffer == NULL)
	{
		free(thiz);
		
		return NULL;
	}

	memset(thiz->buffer, 0x0, width * height);
	return thiz;
}

void binary_bitmap_fill_pixel(BinaryBitmap* thiz, int32_t x, int32_t y)
{
	int32_t index = 0;
	if(thiz == NULL || x < 0 || y < 0)
	{
		return;
	}
	if(x / 8 >= thiz->width || y >= thiz->height)
	{
		//printf("binary_bitmap_fill_pixel invalid pixel:(%d %d)\r\n", x, y);
		return;
	}
	index = (uint32_t)((y * thiz->width * 8 + x) / 8);
	//printf("set point (%d, %d) => (%d, %d)\r\n", x, y, index, x%8);
	thiz->buffer[index] |= (0x80 >> (x % 8));
}

void binary_bitmap_fill_pixel_with_line_width(BinaryBitmap* thiz, uint32_t x, uint32_t y, int32_t line_width)
{
	int32_t right, left, top, buttom;

	if(line_width <= 0)
	{
		line_width = 1;
	}
	right = line_width >> 1;
	left = -1 * right;
	top = line_width >> 1;;
	buttom = -top;
	for(; left <= right; left++)
	{
		for(top = line_width >> 1; top >= buttom; top--)
		{
			//printf("set point (%d, %d)\r\n", x + left, y + top);
			binary_bitmap_fill_pixel(thiz, x + left, y + top);
		}
	}
}

void swap(int32_t *x, int32_t *y)
{
	int32_t temp = *x;
	*x = *y;
	*y = temp;
}

void binary_bitmap_draw_line(BinaryBitmap *thiz, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    int32_t deltax, deltay, error, ystep, x, y;
	int32_t steep = abs(y1 - y0) > abs(x1 - x0);
	//printf("%s (%d, %d) line to (%d, %d)\r\n", __func__, x0, y0, x1, y1);
	if(steep)
	{
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	if(x0 > x1)
	{
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	deltax = x1 - x0;
	deltay = abs(y1 - y0);
	error = deltax / 2;
	y = y0;
	if(y0 < y1)
	{
		ystep = 1;
	}
	else 
	{
		ystep = -1;
	}
	//printf("%s %d\r\n", __func__, __LINE__);
	for(x = x0; x <= x1; x++) 
	{
		
		if(steep) 
		{
			binary_bitmap_fill_pixel_with_line_width(thiz, y, x, 3);
		}
		else
		{	
			binary_bitmap_fill_pixel_with_line_width(thiz, x, y, 3);
		}
		error = error - deltay;
		if(error < 0)
		{
			y = y + ystep;
			error = error + deltax;
		}
	} 
}


void print_binary(uint8_t n)
{
	uint8_t i = 0;
	while(i < 8) 
	{
		if((uint8_t)(n & 0x80) > 0) 
		{
			printf("1");
		}
		else 
		{
			printf("0");
		}
		n <<= 1;
		i++;
	}
}

void binary_bitmap_print(BinaryBitmap *thiz)
{
	int32_t i, j;
	
	for(i = 0; i < thiz->height; i++)
	{
		for(j = 0; j <thiz->width; j++)
		{
			print_binary(thiz->buffer[thiz->width * i + j]);
		}
		printf("\r\n");
	}
}

void binary_bitmap_destroy(BinaryBitmap *thiz)
{
	if(thiz != NULL)
	{
		free(thiz->buffer);
		free(thiz);
	}
}

/////////////////////////
//end BinaryBitmap
/////////////////////////


static int32_t test_binary_bitmap()
{
	uint32_t i = 0;
	uint32_t j = 0;
	BinaryBitmap *signature;
	uint8_t *out_buffer = NULL;

	uint32_t line_width = 1;
	uint32_t width = 24;
	uint32_t height = 12;
	signature = binary_bitmap_create(width, height);
	if(signature == NULL)
	{
	 	//Trace("debug", "binary_bitmap_create failed\r\n");
		return -1;
	}
	binary_bitmap_draw_line(signature, 0, 0, width , 0);
	binary_bitmap_draw_line(signature, width - 1, 0, width - 1, height - 1);
	binary_bitmap_draw_line(signature, width - 1, height - 1, 0, height - 1);
	binary_bitmap_draw_line(signature, 0, height - 1, 0, 0);
	binary_bitmap_draw_line(signature, 0, 0, width - 1, height - 1);
	binary_bitmap_draw_line(signature, width - 1, 0, 0, height - 1);
	binary_bitmap_print(signature);

	binary_bitmap_destroy(signature);
	return 0;
}
int main()
{
   /* 我的第一个 C 程序 */
   test_binary_bitmap();
   
   return 0;
}