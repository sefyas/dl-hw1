#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "uwnet.h"

// Add bias terms to a matrix
// matrix m: partially computed output of layer
// matrix b: bias to add in (should only be one row!)
void forward_convolutional_bias(matrix m, matrix b)
{
    assert(b.rows == 1);
    assert(m.cols % b.cols == 0);
    int spatial = m.cols / b.cols;
    int i,j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            m.data[i*m.cols + j] += b.data[j/spatial];
        }
    }
}

// Calculate bias updates from a delta matrix
// matrix delta: error made by the layer
// matrix db: delta for the biases
void backward_convolutional_bias(matrix delta, matrix db)
{
    assert(db.rows == 1);
    assert(delta.cols % db.cols == 0);
    int spatial = delta.cols / db.cols;
    int i,j;
    for(i = 0; i < delta.rows; ++i){
        for(j = 0; j < delta.cols; ++j){
            db.data[j/spatial] += delta.data[i*delta.cols + j];
        }
    }
}

// Make a column matrix out of an image
// image im: image to process
// int size: kernel size for convolution operation
// int stride: stride for convolution
// returns: column matrix
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
    int im2col_row, im2col_col;
    int index;
    float val;
    for (ch = 0; ch < im.c; ch++) {
        for (y = 0; y < im.h; y += stride) {
            for (x = 0; x < im.w; x += stride) {
            	
            	// find the range of row and column of pixels that fall 
            	// under the filter when it's applied to pixel (y, x) of 
            	// channel c of image (pixel at row y and column x)
                /*
                // for size % 2 == 0, pixel (y, x) is at bottom right of the center
                row_lower_range = y - size / 2;
                row_upper_range = y + (size - 1) / 2;
                col_lower_range = x - size / 2;
                col_upper_range = x + (size - 1) / 2;
                */
                // for size % 2 == 0, pixel (y, x) is at top left of the center
                row_lower_range = y - (size - 1) / 2;
                row_upper_range = y + size / 2;
                col_lower_range = x - (size - 1) / 2;
                col_upper_range = x + size / 2;

                // find the corresponding (starting) row and column of 
                // im2col where the pixels should be organized
                im2col_col = y / stride * outw + x / stride;
                im2col_row = ch * size * size;

                // copy over the pixel values in im2col
                for (y_ = row_lower_range; y_ <= row_upper_range; y_++) {
                    for (x_ = col_lower_range; x_ <= col_upper_range; x_++) {
                        index = im2col_row * cols + im2col_col;
                        if(x_ < 0 || x_ >= im.w || y_ < 0 || y_ >= im.h)
                            val = 0;
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

// The reverse of im2col, add elements back into image
// matrix col: column matrix to put back into image
// int size: kernel size
// int stride: convolution stride
// image im: image to add elements back into
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
    	
    	// find the pixel where the filter was applied 
        y = (im2col_col / outw) * stride;
        x = (im2col_col % outw) * stride;

        for(im2col_row = 0; im2col_row < rows; im2col_row++) {

        	// for each item in im2col find the corresponding pixel
        	// coordinate and channel it's associated with
            ch = im2col_row / (size * size);
            index_ = im2col_row % (size * size);
            /*
            // for size % 2 == 0, pixel (y, x) is at bottom right of the center
            y_ = y + index_ / size - size / 2;
            x_ = x + index_ % size - size / 2;
            */
            // for size % 2 == 0, pixel (y, x) is at top left of the center
            y_ = y + index_ / size - (size - 1) / 2;
            x_ = x + index_ % size - (size - 1) / 2;
            index = im2col_row * cols + im2col_col;

            // add the value at im2col to the current pixel value
            if(x_ < 0 || x_ >= im.w || y_ < 0 || y_ >= im.h)
                val = 0;
            else {
                val = col.data[index];
                set_pixel(im, x_, y_, ch, get_pixel(im, x_, y_, ch) + val);
            }
        }
    }

}

// Run a convolutional layer on input
// layer l: pointer to layer to run
// matrix in: input to layer
// returns: the result of running the layer
matrix forward_convolutional_layer(layer l, matrix in)
{
    int i, j;
    int outw = (l.width-1)/l.stride + 1;
    int outh = (l.height-1)/l.stride + 1;
    matrix out = make_matrix(in.rows, outw*outh*l.filters);
    for(i = 0; i < in.rows; ++i){
        image example = float_to_image(in.data + i*in.cols, l.width, l.height, l.channels);
        matrix x = im2col(example, l.size, l.stride);
        matrix wx = matmul(l.w, x);
        for(j = 0; j < wx.rows*wx.cols; ++j){
            out.data[i*out.cols + j] = wx.data[j];
        }
        free_matrix(x);
        free_matrix(wx);
    }
    forward_convolutional_bias(out, l.b);
    activate_matrix(out, l.activation);

    l.in[0] = in;
    free_matrix(l.out[0]);
    l.out[0] = out;
    free_matrix(l.delta[0]);
    l.delta[0] = make_matrix(out.rows, out.cols);
    return out;
}

// Run a convolutional layer backward
// layer l: layer to run
// matrix prev_delta: error term for the previous layer
void backward_convolutional_layer(layer l, matrix prev_delta)
{
    matrix in    = l.in[0];
    matrix out   = l.out[0];
    matrix delta = l.delta[0];

    int outw = (l.width-1)/l.stride + 1;
    int outh = (l.height-1)/l.stride + 1;

    gradient_matrix(out, l.activation, delta);
    backward_convolutional_bias(delta, l.db);
    int i;
    matrix wt = transpose_matrix(l.w);
    for(i = 0; i < in.rows; ++i){
        image example = float_to_image(in.data + i*in.cols, l.width, l.height, l.channels);
        image dexample = float_to_image(prev_delta.data + i*in.cols, l.width, l.height, l.channels);
        assert(in.cols == l.width*l.height*l.channels);

        delta.rows = l.filters;
        delta.cols = outw*outh;
        delta.data = l.delta[0].data + i*delta.rows*delta.cols;

        matrix x = im2col(example, l.size, l.stride);
        matrix xt = transpose_matrix(x);
        matrix dw = matmul(delta, xt);
        axpy_matrix(1, dw, l.dw);
        
        if(prev_delta.data){
            matrix col = matmul(wt, delta);
            col2im(col, l.size, l.stride, dexample);
            free_matrix(col);
        }
        free_matrix(x);
        free_matrix(xt);
        free_matrix(dw);
    }
    free_matrix(wt);

}

// Update convolutional layer
// layer l: layer to update
// float rate: learning rate
// float momentum: momentum term
// float decay: l2 regularization term
void update_convolutional_layer(layer l, float rate, float momentum, float decay)
{
    // TODO: 5.3 Update the weights, similar to the connected layer.
    axpy_matrix(-1 * decay, l.w, l.dw);
    axpy_matrix(rate, l.dw, l.w);
    scal_matrix(momentum, l.dw);
}

// Make a new convolutional layer
// int w: width of input image
// int h: height of input image
// int c: number of channels
// int size: size of convolutional filter to apply
// int stride: stride of operation
layer make_convolutional_layer(int w, int h, int c, int filters, int size, int stride, ACTIVATION activation)
{
    layer l = {0};
    l.width = w;
    l.height = h;
    l.channels = c;
    l.filters = filters;
    l.size = size;
    l.stride = stride;
    l.w  = random_matrix(filters, size*size*c, sqrtf(2.f/(size*size*c)));
    l.dw = make_matrix(filters, size*size*c);
    l.b  = make_matrix(1, filters);
    l.db = make_matrix(1, filters);
    l.in = calloc(1, sizeof(matrix));
    l.out = calloc(1, sizeof(matrix));
    l.delta = calloc(1, sizeof(matrix));
    l.activation = activation;
    l.forward  = forward_convolutional_layer;
    l.backward = backward_convolutional_layer;
    l.update   = update_convolutional_layer;
    return l;
}

