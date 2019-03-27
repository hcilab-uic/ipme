// Adapted from:
// http://dlib.net/dnn_metric_learning_ex.cpp.html

#include <iostream>
#include <vector>

#include "dlib/dnn.h"

int main() try {
    using namespace dlib;

    // The API for doing metric learning is very similar to the API for
    // multi-class classification.  In fact, the inputs are the same, a bunch of
    // labeled objects.  So here we create our dataset.  We make up some simple
    // vectors and label them with the integers 1,2,3,4.  The specific values of
    // the integer labels don't matter.
    std::vector<dlib::matrix<double, 0, 1>> samples;
    std::vector<unsigned long> labels;

    // class 1 training vectors
    samples.push_back({1, 0, 0, 0, 0, 0, 0, 0});
    labels.push_back(1);
    samples.push_back({0, 1, 0, 0, 0, 0, 0, 0});
    labels.push_back(1);

    // class 2 training vectors
    samples.push_back({0, 0, 1, 0, 0, 0, 0, 0});
    labels.push_back(2);
    samples.push_back({0, 0, 0, 1, 0, 0, 0, 0});
    labels.push_back(2);

    // class 3 training vectors
    samples.push_back({0, 0, 0, 0, 1, 0, 0, 0});
    labels.push_back(3);
    samples.push_back({0, 0, 0, 0, 0, 1, 0, 0});
    labels.push_back(3);

    // class 4 training vectors
    samples.push_back({0, 0, 0, 0, 0, 0, 1, 0});
    labels.push_back(4);
    samples.push_back({0, 0, 0, 0, 0, 0, 0, 1});
    labels.push_back(4);

    // Make a network that simply learns a linear mapping from 8D vectors to 2D
    // vectors.
    using net_type = loss_metric<fc<2, input<matrix<double, 0, 1>>>>;
    net_type net;
    dnn_trainer<net_type> trainer(net);
    trainer.set_learning_rate(0.1);

    // It should be emphasized out that it's really important that each
    // mini-batch contain multiple instances of each class of object.  This is
    // because the metric learning algorithm needs to consider pairs of objects
    // that should be close as well as pairs of objects that should be far apart
    // during each training step.  Here we just keep training on the same small
    // batch so this constraint is trivially satisfied.
    while(trainer.get_learning_rate() >= 1e-4)
        trainer.train_one_step(samples, labels);

    // Wait for training threads to stop
    trainer.get_net();
    std::cout << "done training" << std::endl;

    // Run all the samples through the network to get their 2D vector
    // embeddings.
    std::vector<matrix<float, 0, 1>> embedded = net(samples);

    // Print the embedding for each sample to the screen.  If you look at the
    // outputs carefully you should notice that they are grouped together in 2D
    // space according to their label.
    for(size_t i = 0; i < embedded.size(); ++i)
        std::cout << "label: " << labels[i] << "\t" << trans(embedded[i]);

    // Now, check if the embedding puts things with the same labels near each
    // other and things with different labels far apart.
    int num_right = 0;
    int num_wrong = 0;
    for(size_t i = 0; i < embedded.size(); ++i) {
        for(size_t j = i + 1; j < embedded.size(); ++j) {
            if(labels[i] == labels[j]) {
                // The loss_metric layer will cause things with the same label
                // to be less than net.loss_details().get_distance_threshold()
                // distance from each other.  So we can use that distance value
                // as our testing threshold for "being near to each other".
                if(length(embedded[i] - embedded[j]) <
                   net.loss_details().get_distance_threshold())
                    ++num_right;
                else
                    ++num_wrong;
            } else {
                if(length(embedded[i] - embedded[j]) >=
                   net.loss_details().get_distance_threshold())
                    ++num_right;
                else
                    ++num_wrong;
            }
        }
    }

    std::cout << "num_right: " << num_right << std::endl;
    std::cout << "num_wrong: " << num_wrong << std::endl;
} catch(std::exception& e) {
    std::cerr << e.what() << std::endl;
}
