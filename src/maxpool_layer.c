#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include "uwnet.h"


// Run a maxpool layer on input
// layer l: pointer to layer to run
// matrix in: input to layer
// returns: the result of running the layer
matrix forward_maxpool_layer(layer l, matrix in)
{
    int outw = (l.width-1)/l.stride + 1;
    int outh = (l.height-1)/l.stride + 1;
    matrix out = make_matrix(in.rows, outw*outh*l.channels);

    // TODO: 6.1 - iterate over the input and fill in the output with max values
    // YSS DONE!
    int im, ch, x, y, x_, y_;
    int row_lower_range, row_upper_range, col_lower_range, col_upper_range;
    int maxpool_row, maxpool_col;
    int index;
    float max;
    for(im = 0; im < in.rows; im++) {
        for (ch = 0; ch < l.channels; ch++) {
            for (y = 0; y < l.height; y += l.stride) {
                for (x = 0; x < l.width; x += l.stride) {
                    // find the index of pixel (y, x) of channel c of the 
                    // current sample in input matrix to initialize the max
                    // value with it
                    index = im * l.channels * l.width * l.height
                            + ch * l.width * l.height 
                            + y * l.width 
                            + x;
                    max = in.data[index];

                    // find the range of row and column of pixels that fall 
                    // under the filter when it's applied to pixel (y, x) of 
                    // channel c of image (pixel at row y and column x)
                    /*
                    // for size % 2 == 0, pixel (y, x) is at bottom right of the center
                    row_lower_range = y - l.size / 2;
                    row_upper_range = y + (l.size - 1) / 2;
                    col_lower_range = x - l.size / 2;
                    col_upper_range = x + (l.size - 1) / 2;
                    */
                    // for size % 2 == 0, pixel (y, x) is at top left of the center
                    row_lower_range = y - (l.size - 1) / 2;
                    row_upper_range = y + l.size / 2;
                    col_lower_range = x - (l.size - 1) / 2;
                    col_upper_range = x + l.size / 2;

                    for (y_ = row_lower_range; y_ <= row_upper_range; y_++) {
                        if(y_ < 0 || y_ >= l.height)
                            continue;
                        for (x_ = col_lower_range; x_ <= col_upper_range; x_++) {
                            if(x_ < 0 || x_ >= l.width)
                                continue;
                            // find the index of pixel (y_, x_) of channel c 
                            // of the current sample in input matrix to compare
                            // it against max (if it is within height and width)
                            index = im * l.channels * l.width * l.height
                                    + ch * l.width * l.height 
                                    + y_ * l.width 
                                    + x_;
                            // update the max if needed
                            if(max < in.data[index])
                                max = in.data[index];
                        }
                    }

                    // find the row and column of output where the max value 
                    // should be placed
                    maxpool_row = im;
                    maxpool_col = ch * outw * outh 
                                  + (y / l.stride) * outw
                                  + (x / l.stride);
                    index = maxpool_row * l.channels * outw * outh 
                            + maxpool_col;
                    out.data[index] = max;
                }
            }
        }
    }

    l.in[0] = in;
    free_matrix(l.out[0]);
    l.out[0] = out;
    free_matrix(l.delta[0]);
    l.delta[0] = make_matrix(out.rows, out.cols);
    return out;
}

// Run a maxpool layer backward
// layer l: layer to run
// matrix prev_delta: error term for the previous layer
void backward_maxpool_layer(layer l, matrix prev_delta)
{
    matrix in    = l.in[0];
    //matrix out   = l.out[0];
    // QUESTION: I have assumed out and delta have the same dimensions.
    //           Is this correct? If so I don't need out
    matrix delta = l.delta[0];
    // QUESTION: in ignoring output I have assumed that we don't multiply the
    // error (in delta) and output (in out). Is this correct?


    int outw = (l.width-1)/l.stride + 1;
    int outh = (l.height-1)/l.stride + 1;

    // TODO: 6.2 - find the max values in the input again and fill in the
    // corresponding delta with the delta from the output. This should be
    // similar to the forward method in structure.
    // YSS DONE!
    int im, ch, x, y, x_, y_;
    int row_lower_range, row_upper_range, col_lower_range, col_upper_range;
    int maxpool_row, maxpool_col;
    int index;
    float max, max_delta;
    int max_y, max_x;
    for(im = 0; im < in.rows; im++) {
        for (ch = 0; ch < l.channels; ch++) {
            for (y = 0; y < l.height; y += l.stride) {
                for (x = 0; x < l.width; x += l.stride) {

                    // find the index of pixel (y, x) of channel c of the 
                    // current sample in input matrix to initialize the max
                    // value with it
                    index = im * l.channels * l.width * l.height
                            + ch * l.width * l.height 
                            + y * l.width 
                            + x;
                    max = in.data[index];
                    // the coordinates of the max are initialized at the 
                    // pixel where maxpooling is applied to
                    max_y = y;
                    max_x = x;
                    
                    // find the range of row and column of pixels that fall 
                    // under the filter when it's applied to pixel (y, x) of 
                    // channel c of image (pixel at row y and column x)
                    /*
                    // for size % 2 == 0, pixel (y, x) is at bottom right of the center
                    row_lower_range = y - l.size / 2;
                    row_upper_range = y + (l.size - 1) / 2;
                    col_lower_range = x - l.size / 2;
                    col_upper_range = x + (l.size - 1) / 2;
                    */
                    // for size % 2 == 0, pixel (y, x) is at top left of the center
                    row_lower_range = y - (l.size - 1) / 2;
                    row_upper_range = y + l.size / 2;
                    col_lower_range = x - (l.size - 1) / 2;
                    col_upper_range = x + l.size / 2;
                    
                    for (y_ = row_lower_range; y_ <= row_upper_range; y_++) {
                        if(y_ < 0 || y_ >= l.height)
                            continue;
                        for (x_ = col_lower_range; x_ <= col_upper_range; x_++) {
                            if(x_ < 0 || x_ >= l.width)
                                continue;
                            // find the index of pixel (y_, x_) of channel c 
                            // of the current sample in input matrix to compare
                            // it against max (if it is within height and width)
                            index = im * l.channels * l.width * l.height
                                    + ch * l.width * l.height 
                                    + y_ * l.width 
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
                                  + (y / l.stride) * outw
                                  + (x / l.stride);
                    index = maxpool_row * l.channels * outw * outh 
                            + maxpool_col;
                    max_delta = delta.data[index];

                    // find the index of the input matrix for the max value
                    index = im * l.channels * l.width * l.height
                            + ch * l.width * l.height 
                            + max_y * l.width 
                            + max_x;
                    prev_delta.data[index] += max_delta;
                }
            }
        }
    }
}

// Update maxpool layer
// Leave this blank since maxpool layers have no update
void update_maxpool_layer(layer l, float rate, float momentum, float decay)
{
}

// Make a new maxpool layer
// int w: width of input image
// int h: height of input image
// int c: number of channels
// int size: size of maxpool filter to apply
// int stride: stride of operation
layer make_maxpool_layer(int w, int h, int c, int size, int stride)
{
    layer l = {0};
    l.width = w;
    l.height = h;
    l.channels = c;
    l.size = size;
    l.stride = stride;
    l.in = calloc(1, sizeof(matrix));
    l.out = calloc(1, sizeof(matrix));
    l.delta = calloc(1, sizeof(matrix));
    l.forward  = forward_maxpool_layer;
    l.backward = backward_maxpool_layer;
    l.update   = update_maxpool_layer;
    return l;
}

