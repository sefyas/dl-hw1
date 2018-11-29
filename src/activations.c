#include <assert.h>
#include <math.h>
#include "uwnet.h"

// Run an activation function on each element in a matrix,
// modifies the matrix in place
// matrix m: Input to activation function
// ACTIVATION a: function to run
void activate_matrix(matrix m, ACTIVATION a)
{
    int i, j, index;
    double val, sum;
    for(i = 0; i < m.rows; ++i){
        sum = 0;
        for(j = 0; j < m.cols; ++j){
            index = i * m.cols + j;
            val = m.data[index];
            if(a == LOGISTIC) {
                // TODO
                // YSS DONE!
                val = 1 / (1 + exp(val));
           } else if (a == RELU) {
                // TODO
                // YSS DONE!
                if(val < 0)
                    val = 0;
            } else if (a == LRELU) {
                // TODO
                // YSS DONE!
                if(val < 0)
                    val = 0.1 * val;
            } else if (a == SOFTMAX) {
                // TODO
                // YSS DONE!
                val = exp(val);
            }
            m.data[index] = val;
            sum += m.data[index];
        }
        if (a == SOFTMAX) {
            // TODO: have to normalize  by sum if we are using SOFTMAX
            // YSS DONE!
            for(j = 0; j < m.cols; j++) {
                index = i * m.cols + j;
                m.data[index] /= sum;
            }
        }
    }
}

// Calculates the gradient of an activation function and multiplies it into
// the delta for a layer
// matrix m: an activated layer output
// ACTIVATION a: activation function for a layer
// matrix d: delta before activation gradient
void gradient_matrix(matrix m, ACTIVATION a, matrix d)
{
    int i, j, index;
    double val;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            index = i * m.cols + j;
            val = m.data[index];
            // TODO: multiply the correct element of d by the gradient
            // YSS DONE!
            if (a == LOGISTIC) {
                val = val * (1 - val);
            
            } else if (a == RELU) {
                if(val > 0)
                    val = 1;
                else
                    val = 0; // val should be zero if not positive
            } else if (a == LRELU) {
                if(val > 0)
                    val = 1;
                else
                    val = 0.1;
            } else if (a == SOFTMAX) {
                val = 1;
            }
            d.data[index] = d.data[index] * val;
        }
    }
}
