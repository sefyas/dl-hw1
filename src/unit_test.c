#include <stdio.h>
#include "matrix.h"
#include "image.h"

void print_image(image im);
matrix im2col(image im, int size, int stride);
void col2im(matrix col, int size, int stride, image im);
matrix forward_maxpool_layer(int width, int height, int channels, int size, int stride, matrix in);
void backward_maxpool_layer(matrix in, matrix delta, int width, int height, int channels, int size, int stride, matrix prev_delta);

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

    int row = 2; int col = 32;
    matrix in = make_matrix(row, col);
    in.data[0] = 111; in.data[1] = 112; in.data[2] = 113; in.data[3] = 114;
    in.data[4] = 121; in.data[5] = 122; in.data[6] = 123; in.data[7] = 124;
    in.data[8] = 131; in.data[9] = 132; in.data[10] = 133; in.data[11] = 134;
    in.data[12] = 141; in.data[13] = 142; in.data[14] = 143; in.data[15] = 144;
    in.data[16] = 211; in.data[17] = 212; in.data[18] = 213; in.data[19] = 214;
    in.data[20] = 221; in.data[21] = 222; in.data[22] = 223; in.data[23] = 224;
    in.data[24] = 231; in.data[25] = 232; in.data[26] = 233; in.data[27] = 234;
    in.data[28] = 241; in.data[29] = 242; in.data[30] = 243; in.data[31] = 244;
    int ind;
    for(ind = 0; ind < col; ind++) {
        in.data[ind + col] = in.data[ind] * -1;
    }
    //print_matrix(in);
    int width = 4; int height = 4; int channels = 2; size = 3; stride = 2;
    matrix out = forward_maxpool_layer(width, height, channels, size, stride, in);
    print_matrix(out);

    matrix prev_delta = make_matrix(row, col);
    col =  ((width-1)/stride + 1) * ((height-1)/stride + 1) * channels;
    matrix delta = random_matrix(row, col, 1);
    print_matrix(delta);
    backward_maxpool_layer(in, delta, width, height, channels, size, stride, prev_delta);
    print_matrix(prev_delta);
   
    free_image(im);
    free_matrix(im_col);
    free_image(im2);
    free_matrix(in);
    free_matrix(out);
    free_matrix(prev_delta);
    free_matrix(delta);
    
    return 0;
}

void print_image(image im)
{
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
                /*
                row_lower_range = y - size / 2;
                row_upper_range = y + (size - 1) / 2;
                col_lower_range = x - size / 2;
                col_upper_range = x + (size - 1) / 2;
                */
                row_lower_range = y - (size - 1) / 2;
                row_upper_range = y + size / 2;
                col_lower_range = x - (size - 1) / 2;
                col_upper_range = x + size / 2;
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
            /*
            y_ = y + index_ / size - size / 2;
            x_ = x + index_ % size - size / 2;
            */
            y_ = y + index_ / size - (size - 1) / 2;
            x_ = x + index_ % size - (size - 1) / 2;
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

matrix forward_maxpool_layer(int width, int height, int channels, int size, int stride, matrix in)
{
    int outw = (width-1)/stride + 1;
    int outh = (height-1)/stride + 1;
    matrix out = make_matrix(in.rows, outw*outh*channels);

    //printf("from %2dx%2d input to %2dx%2d output\n", height, width, outh, outw);

    // TODO: 6.1 - iterate over the input and fill in the output with max values
    // YSS TO-DO test
    int im, ch, x, y, x_, y_;
    int row_lower_range, row_upper_range, col_lower_range, col_upper_range;
    int maxpool_row, maxpool_col;
    int index;
    float max;
    for(im = 0; im < in.rows; im++) {
        for (ch = 0; ch < channels; ch++) {
            for (y = 0; y < height; y += stride) {
                for (x = 0; x < width; x += stride) {
                    // find the index of pixel (y, x) of channel c of the 
                    // current sample in input matrix to initialize the max
                    // value with it
                    index = im * channels * width * height
                            + ch * width * height 
                            + y * width 
                            + x;
                    max = in.data[index];
                    //printf("channel %2d; filter at (%2d, %2d) = %2.0f; index %2d --- ", ch, y, x, max, index);

                    // find the range of row and column of pixels that fall 
                    // under the filter when it's applied to pixel (y, x) of 
                    // channel c of image (pixel at row y and column x)
                    /*
                    row_lower_range = y - size / 2;
                    row_upper_range = y + (size - 1) / 2;
                    col_lower_range = x - size / 2;
                    col_upper_range = x + (size - 1) / 2;
                    */
                    row_lower_range = y - (size - 1) / 2;
                    row_upper_range = y + size / 2;
                    col_lower_range = x - (size - 1) / 2;
                    col_upper_range = x + size / 2;
                    //printf("rows (%2d-%2d) and cols (%2d-%2d)\n", row_lower_range,
                    //                                              row_upper_range,
                    //                                              col_lower_range,
                    //                                              col_upper_range);

                    for (y_ = row_lower_range; y_ <= row_upper_range; y_++) {
                        if(y_ < 0 || y_ >= height)
                            continue;
                        for (x_ = col_lower_range; x_ <= col_upper_range; x_++) {
                            if(x_ < 0 || x_ >= width)
                                continue;
                            // find the index of pixel (y_, x_) of channel c 
                            // of the current sample in input matrix to compare
                            // it against max (if it is within height and width)
                            index = im * channels * width * height
                                    + ch * width * height 
                                    + y_ * width 
                                    + x_;
                            //printf("cell (%2d, %2d) = %2.0f; index %2d", y_, x_, in.data[index], index);
                            // update the max if needed
                            if(max < in.data[index]) {
                                max = in.data[index];
                                //printf(" --- max updated");
                            }
                            //printf("\n");
                        }
                    }

                    // find the row and column of output where the max value 
                    // should be placed
                    maxpool_row = im;
                    maxpool_col = ch * outw * outh 
                                  + (y / stride) * outw
                                  + (x / stride);
                    index = maxpool_row * channels * outw * outh 
                            + maxpool_col;
                    //printf("output at (%2d, %2d) = %2.0f; index %2d\n", (y / stride), (x / stride), max, index);
                    out.data[index] = max;
                }
            }
        }
    }

    return out;
}

void backward_maxpool_layer(matrix in, matrix delta, int width, int height, int channels, int size, int stride, matrix prev_delta)
{

    int outw = (width-1)/stride + 1;
    int outh = (height-1)/stride + 1;

    // TODO: 6.2 - find the max values in the input again and fill in the
    // corresponding delta with the delta from the output. This should be
    // similar to the forward method in structure.
    // YSS TO-DO test
    int im, ch, x, y, x_, y_;
    int row_lower_range, row_upper_range, col_lower_range, col_upper_range;
    int maxpool_row, maxpool_col;
    int index;
    float max, max_delta;
    int max_y, max_x;
    for(im = 0; im < in.rows; im++) {
        for (ch = 0; ch < channels; ch++) {
            for (y = 0; y < height; y += stride) {
                for (x = 0; x < width; x += stride) {

                    // find the index of pixel (y, x) of channel c of the 
                    // current sample in input matrix to initialize the max
                    // value with it
                    index = im * channels * width * height
                            + ch * width * height 
                            + y * width 
                            + x;
                    max = in.data[index];
                    // the coordinates of the max are initialized as the 
                    // pixel where maxpooling is applied to
                    max_y = y;
                    max_x = x;
                    
                    // find the range of row and column of pixels that fall 
                    // under the filter when it's applied to pixel (y, x) of 
                    // channel c of image (pixel at row y and column x)
                    /*
                    row_lower_range = y - size / 2;
                    row_upper_range = y + (size - 1) / 2;
                    col_lower_range = x - size / 2;
                    col_upper_range = x + (size - 1) / 2;
                    */
                    row_lower_range = y - (size - 1) / 2;
                    row_upper_range = y + size / 2;
                    col_lower_range = x - (size - 1) / 2;
                    col_upper_range = x + size / 2;
                    
                    for (y_ = row_lower_range; y_ <= row_upper_range; y_++) {
                        if(y_ < 0 || y_ >= height)
                            continue;
                        for (x_ = col_lower_range; x_ <= col_upper_range; x_++) {
                            if(x_ < 0 || x_ >= width)
                                continue;
                            // find the index of pixel (y_, x_) of channel c 
                            // of the current sample in input matrix to compare
                            // it against max (if it is within height and width)
                            index = im * channels * width * height
                                    + ch * width * height 
                                    + y_ * width 
                                    + x_;
                            // update the max and its coordinates if needed
                            if(max < in.data[index]){
                                max = in.data[index];
                                max_y = y_;
                                max_x = x_;
                            }
                        }
                    }
                    
                    // find the row and column of output/delta matrix where
                    // max value is associated with
                    maxpool_row = im;
                    maxpool_col = ch * outw * outh 
                                  + (y / stride) * outw
                                  + (x / stride);
                    index = maxpool_row * channels * outw * outh 
                            + maxpool_col;
                    max_delta = delta.data[index];

                    // find the index of the input matrix for the max value
                    index = im * channels * width * height
                            + ch * width * height 
                            + max_y * width 
                            + max_x;
                    prev_delta.data[index] += max_delta;
                }
            }
        }
    }
}