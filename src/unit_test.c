#include <stdio.h>
#include "matrix.h"
#include "image.h"

void print_image(image im);
matrix im2col(image im, int size, int stride);
void col2im(matrix col, int size, int stride, image im);
int main(int argc, char **argv)
{

    int imw = 3; int imh = 3; int imc = 1;
    image im = make_image(imw, imh, imc);
    im.data[0] = 0; im.data[1] = 1; im.data[2] = 2; im.data[3] = 3;
    im.data[4] = 4; im.data[5] = 5; im.data[6] = 6; im.data[7] = 7;
    im.data[8] = 8; //im.data[9] = 9; im.data[10] = 10; im.data[11] = 11;
    //im.data[12] = 12; im.data[13] = 13; im.data[14] = 14; im.data[15] = 15;
    print_image(im);

    int size = 2; int stride = 2;
    matrix im_col = im2col(im, size, stride);
    print_matrix(im_col);

    image im2 = make_image(imw, imh, imc);
    col2im(im_col, size, stride, im2);
    print_image(im2);
   
    free_image(im);
    free_matrix(im_col);
    free_image(im2);
    
    return 0;
}

void print_image(image im) {
	int c, x, y;
    for(c = 0; c < im.c; c++) {
    	for(y = 0; y < im.h; y++) {
    		for(x = 0; x < im.h; x++)
    			printf("%2.0f ", get_pixel(im, x, y, c));
    		printf("\n");
    	}
    	printf("--------\n");
    }
}

matrix im2col(image im, int size, int stride)
{
    int outw = (im.w-1)/stride + 1;
    int outh = (im.h-1)/stride + 1;
    int rows = im.c*size*size;
    int cols = outw * outh;
    matrix col = make_matrix(rows, cols);

    // TODO: 5.1 - fill in the column matrix
    // YSS DONE!
    int ch, x, y, x_, y_;
    int row_lower_range, row_upper_range, col_lower_range, col_upper_range;
    int im2col_col, im2col_row;
    int index;
    float val;
    for (ch = 0; ch < im.c; ch++) {
        for (y = 0; y < im.h; y += stride) {
            for (x = 0; x < im.w; x += stride) {
                row_lower_range = y - size / 2;
                row_upper_range = y + (size - 1) / 2;
                col_lower_range = x - size / 2;
                col_upper_range = x + (size - 1) / 2;
                im2col_col = y / stride * outw + x / stride;
                im2col_row = ch * size * size;
                for (y_ = row_lower_range; y_ <= row_upper_range; y_++) {
                    for (x_ = col_lower_range; x_ <= col_upper_range; x_++) {
                        index = im2col_row * cols + im2col_col;
                        if(x_ < 0 || x_ >= im.w || y_ < 0 || y_ >= im.h)
                            val = -1;
                        else
                            val = get_pixel(im, x_, y_, ch);
                        col.data[index] = val;
                        im2col_row++;
                    }
                }
            }
        }
    }

    return col;
}

void col2im(matrix col, int size, int stride, image im)
{
    int outw = (im.w-1)/stride + 1;
    int outh = (im.h-1)/stride + 1;
    int rows = im.c*size*size;
    int cols = outw * outh;

    // TODO: 5.2 - add values into image im from the column matrix
    // YSS DONE!
    int im2col_row, im2col_col;
    int ch, x, y, x_, y_;
    int index, index_;
    float val;
    for(im2col_col = 0; im2col_col < cols; im2col_col++) {
        y = (im2col_col / outw) * stride;
        x = (im2col_col % outw) * stride;
        for(im2col_row = 0; im2col_row < rows; im2col_row++) {
            ch = im2col_row / (size * size);
            index_ = im2col_row % (size * size);
            y_ = y + index_ / size - size / 2;
            x_ = x + index_ % size - size / 2;
            index = im2col_row * cols + im2col_col;
            if(x_ < 0 || x_ >= im.w || y_ < 0 || y_ >= im.h)
                val = 0;
            else {
                val = col.data[index];
                set_pixel(im, x_, y_, ch, get_pixel(im, x_, y_, ch) + val);
            }
        }
    }

}