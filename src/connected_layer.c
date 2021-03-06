#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "uwnet.h"

// Add bias terms to a matrix
// matrix m: partially computed output of layer
// matrix b: bias to add in (should only be one row!)
void forward_bias(matrix m, matrix b)
{
    assert(b.rows == 1);
    assert(m.cols == b.cols);
    int i,j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            m.data[i*m.cols + j] += b.data[j];
        }
    }
}

// Calculate bias updates from a delta matrix
// matrix delta: error made by the layer
// matrix db: delta for the biases
void backward_bias(matrix delta, matrix db)
{
    int i, j;
    for(i = 0; i < delta.rows; ++i){
        for(j = 0; j < delta.cols; ++j){
            db.data[j] += delta.data[i*delta.cols + j];
        }
    }
}

// Run a connected layer on input
// layer l: pointer to layer to run
// matrix in: input to layer
// returns: the same layer, modified after running
matrix forward_connected_layer(layer l, matrix in)
{
    // TODO: 3.1 - run the network forward
    // YSS DONE!
    matrix out = matmul(in, l.w);
    forward_bias(out, l.b);
    activate_matrix(out, l.activation);

    // Saving our input and output and making a new delta matrix to hold errors
    // Probably don't change this
    l.in[0] = in;
    free_matrix(l.out[0]);
    l.out[0] = out;
    free_matrix(l.delta[0]);
    l.delta[0] = make_matrix(out.rows, out.cols);
    return out;
}

// Run a connected layer backward
// layer l: layer to run
// matrix delta: 
void backward_connected_layer(layer l, matrix prev_delta)
{
    matrix in    = l.in[0];
    matrix out   = l.out[0];
    matrix delta = l.delta[0];

    // TODO: 3.2
    // delta is the error made by this layer, dL/dout
    // First modify in place to be dL/d(in*w+b) using the gradient of activation
    // YSS DONE!
    gradient_matrix(out, l.activation, delta);
    
    // Calculate the updates for the bias terms using backward_bias
    // The current bias deltas are stored in l.db
    // YSS DONE!
    backward_bias(delta, l.db);

    // Then calculate dL/dw. Use axpy to add this dL/dw into any previously stored
    // updates for our weights, which are stored in l.dw
    // YSS DONE! 
    matrix in_t = transpose_matrix(in);
    matrix dl_dw = matmul(in_t, delta);
    axpy_matrix(1 , dl_dw, l.dw);
    free_matrix(in_t);
    free_matrix(dl_dw);
    // QUESTION: I'm not sure if I have this right

    if(prev_delta.data){
        // Finally, if there is a previous layer to calculate for,
        // calculate dL/d(in). Again, using axpy, add this into the current
        // value we have for the previous layers delta, prev_delta.
        // YSS DONE!
        matrix w_t = transpose_matrix(l.w);
        matrix dl_dx = matmul(delta, w_t);
        axpy_matrix(1, dl_dx, prev_delta);
        free_matrix(w_t);
        free_matrix(dl_dx);
        // QUESTION: I'm not sure if I have this right
    }
}

// Update 
void update_connected_layer(layer l, float rate, float momentum, float decay)
{
    // TODO
    // YSS DONE! 
    // TO-DO consider applying momentum to biases; it probably works without it too
    axpy_matrix(-1 * decay, l.w, l.dw);
    axpy_matrix(rate, l.dw, l.w);
    scal_matrix(momentum, l.dw);
}

layer make_connected_layer(int inputs, int outputs, ACTIVATION activation)
{
    layer l = {0};
    l.w  = random_matrix(inputs, outputs, sqrtf(2.f/inputs));
    l.dw = make_matrix(inputs, outputs);
    l.b  = make_matrix(1, outputs);
    l.db = make_matrix(1, outputs);
    l.in = calloc(1, sizeof(matrix));
    l.out = calloc(1, sizeof(matrix));
    l.delta = calloc(1, sizeof(matrix));
    l.activation = activation;
    l.forward  = forward_connected_layer;
    l.backward = backward_connected_layer;
    l.update   = update_connected_layer;
    return l;
}

