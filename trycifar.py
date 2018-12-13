from uwnet import *

def softmax_model():
    l = [make_connected_layer(3072, 10, SOFTMAX)]
    return make_net(l)

def neural_net():
    l = [   make_connected_layer(3072, 32, LRELU),
            make_connected_layer(32, 10, SOFTMAX)]
    return make_net(l)

def conv_net():
    # How many operations are needed for a forard pass through this network?
    # Your answer: 
    # YSS DONE!
    # number of operations is the sum of convultion operations and max pool operations:
    # - layer 1 = 32 * 32 *  3 *  8 * 3**2 + (32/2) * (32/2) *  8 = 223232
    # - layer 2 = 16 * 16 *  8 * 16 * 3**2 + (16/2) * (16/2) * 16 = 295936
    # - layer 3 =  8 *  8 * 16 * 32 * 3**2 + ( 8/2) * ( 8/2) * 32 = 295424
    # - layer 4 =  4 *  4 * 32 * 64 * 3**2 + ( 4/2) * ( 4/2) * 64 = 295168
    # - layer 5 =  2 *  2 * 64 * 10 = 2560
    # total = 1112320
    # Performance
    # - training accuracy: 71%
    # - test accuracy:     66%
    l = [   make_convolutional_layer(32, 32, 3, 8, 3, 1, LRELU), # layer 1 - conv
            make_maxpool_layer(32, 32, 8, 3, 2), # layer 1 - max
            make_convolutional_layer(16, 16, 8, 16, 3, 1, LRELU), # layer 2 - conv
            make_maxpool_layer(16, 16, 16, 3, 2), # layer 2 - max
            make_convolutional_layer(8, 8, 16, 32, 3, 1, LRELU), # layer 3 - conv
            make_maxpool_layer(8, 8, 32, 3, 2), # layer 3 - max
            make_convolutional_layer(4, 4, 32, 64, 3, 1, LRELU), # layer 4 - conv
            make_maxpool_layer(4, 4, 64, 3, 2), # layer 4 - max
            make_connected_layer(256, 10, SOFTMAX)] # layer 5
    return make_net(l)

def your_net():
    # Define your network architecture here. It should have 5 layers. How many operations does it need for a forward pass?
    # It doesn't have to be exactly the same as conv_net but it should be close.
    # YSS
    # Performance
    # - training accuracy: 55%
    # - test accuracy:     49%
    l = [  make_connected_layer(3072, 369, LRELU), # layer 1
           make_connected_layer(369, 256, LRELU), # layer 2
           make_connected_layer(256, 128, LRELU), # layer 3
           make_connected_layer(128, 64, LRELU), # layer 4
           make_connected_layer(64, 10, SOFTMAX)] # layer 5
    return make_net(l)

print("loading data...")
train = load_image_classification_data("cifar/cifar.train", "cifar/cifar.labels")
test  = load_image_classification_data("cifar/cifar.test", "cifar/cifar.labels")
print("done")
print

print("making model...")
batch = 128
iters = 5000
rate = .01
momentum = .9
decay = .005

m = your_net() # m = conv_net() # YSS DONE!
print("training...")
train_image_classifier(m, train, batch, iters, rate, momentum, decay)
print("done")
print

print("evaluating model...")
print("training accuracy: %f", accuracy_net(m, train))
print("test accuracy:     %f", accuracy_net(m, test))

# How accurate is the fully connected network vs the convnet when they use similar number of operations?
# Why are you seeing these results? Speculate based on the information you've gathered and what you know about DL and ML.
# Your answer:
# 
# YSS DONE!
# A convolutional network outperforms a fully connected network with similar depth and number of
# operations in the forward pass. Because of weight sharing in convultional networks we are 
# dealing with a much smaller number of parameters. As a result, for a limited amount of data
# our model can be better trained (lower variance).

