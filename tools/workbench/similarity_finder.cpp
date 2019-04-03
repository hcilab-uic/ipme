#include "similarity_finder.h"

#include <QDebug>
#include <dlib/dnn.h>
#include <random>
#include <unordered_set>

#include "utils/logger.h"

namespace ipme::wb {
Similarity_finder::Similarity_finder(
    const ipme::wb::Frame_collection& collection)
    : frames_{collection}
{
}

void Similarity_finder::find_similar(size_t range_begin, size_t range_end)
{
    qDebug() << "finding similarity to range " << range_begin << "-"
             << range_end;

    ///////////////////////////// setup data //////////////////////////////////
    // The strategy:
    // Use the selected range to create data of label SIMILAR
    // Then randomly pick the range-span number of frames from rest of the data
    // and assign them label DIFFERENT. These in total constitute the training
    // set. Use all of the data outside the original begin and end as input data
    // for prediction

    int span = range_end - range_begin + 1;

    static constexpr size_t label_similar = 1;
    static constexpr size_t label_different = 2;

    Matrix_type x_train;
    Label_vector y_train;

    std::unordered_set<size_t> training_set;

    for(size_t i = range_begin; i < range_end; ++i) {
        training_set.insert(i);
        x_train.push_back(generate_row(frames_[i]));
        y_train.push_back(label_similar);
    }

    std::random_device rd;
    std::mt19937 gen{rd()};

    DEBUG() << "uniform integer distribution [" << 0 << ", " << frames_.size()
            << ")";

    std::uniform_int_distribution<> dist{0, static_cast<int>(frames_.size())};

    // Randomly select span number of frames from rest of the data-set
    int count{0};
    while(count < span) {
        int random_index = dist(gen);
        if(training_set.find(random_index) != end(training_set)) {
            continue;
            DEBUG() << "random index " << random_index
                    << " already exists, skipping";
        }

        DEBUG() << "random index " << random_index
                << " does not already exist, adding to collection of "
                   "different_label set";

        x_train.push_back(generate_row(frames_[random_index]));
        y_train.push_back(label_different);
        training_set.insert(random_index);

        ++count;

        DEBUG() << "number of different labels added " << count
                << " (span=" << span << ")";
    }

    // Collect all of the data not in the range to as prediction data
    Matrix_type x_predict;
    for(size_t i = 0; i < range_begin; ++i) {
        x_predict.push_back(generate_row(frames_[i]));
    }

    for(size_t i = range_end; i < frames_.size(); ++i) {
        x_predict.push_back(generate_row(frames_[i]));
    }
    ///////////////////////// finish data setup ////////////////////////////////

    //    auto[x_train, y_train, x_predict] = split_data(begin, end);

    DEBUG() << "x_train: " << x_train.size()
            << " samples, y_train: " << y_train.size()
            << " labels, x_predict: " << x_predict.size() << " data points";

    // setup dlib
    using namespace dlib;
    using net_type = loss_metric<fc<2, input<Row_type>>>;
    net_type net;
    dnn_trainer<net_type> trainer{net};
    trainer.set_learning_rate(0.1);

    DEBUG() << "beginning training";
    for(size_t iteration_count = 0; trainer.get_learning_rate() >= 1e-4;
        ++iteration_count) {
        trainer.train_one_step(x_train, y_train);

        if(iteration_count % 1000 == 0) {
            DEBUG() << iteration_count << " training iterations completed";
        }
    }

    //    DEBUG() << "training complete in " << iteration_count << "
    //    iterations";
    DEBUG() << "waiting for training threads to stop";
    trainer.get_net();
    DEBUG() << "finished training";

    // check training accuracy
    const auto result_train = net(x_train);
    //    for(const auto& e : result_train) {
    //        DEBUG() << e;
    //    }
    const auto train_threshold = net.loss_details().get_distance_threshold();
    size_t num_right{0};
    for(size_t i = 0; i < y_train.size(); ++i) {
        for(size_t j = 0; j < y_train.size(); ++j) {
            auto diff = length(result_train[i] - result_train[j]);
            if(y_train[i] == y_train[j]) {
                if(diff < train_threshold) {
                    ++num_right;
                }
            } else {
                if(diff >= train_threshold) {
                    ++num_right;
                }
            }
        }
    }

    double train_accuracy =
        static_cast<double>(num_right) / (y_train.size() * y_train.size());
    DEBUG() << "training accuracy " << train_accuracy * 100 << "%";

    auto result_predict = net(x_predict);
    const size_t result_predict_size = result_predict.size();
    const auto distance_threshold = net.loss_details().get_distance_threshold();

    DEBUG() << "embedded_size " << result_predict_size;
    DEBUG() << "distance_threshold " << distance_threshold;

    size_t num_size_below{0};
    for(size_t i = 0; i < result_predict_size; ++i) {
        for(size_t j = 0; j < result_predict_size; ++j) {
            auto diff = length(result_predict[i] - result_predict[j]);
            if(diff < distance_threshold) {
                ++num_size_below;
            }
        }
    }

    DEBUG() << num_size_below << " results were below threshold";
}

std::tuple<Similarity_finder::Matrix_type, Similarity_finder::Label_vector,
           Similarity_finder::Matrix_type>
Similarity_finder::split_data(size_t range_begin, size_t range_end)
{
    // The strategy:
    // Use the selected range to create data of label SIMILAR
    // Then randomly pick the range-span number of frames from rest of the data
    // and assign them label DIFFERENT. These in total constitute the training
    // set. Use all of the data outside the original begin and end as input data
    // for prediction

    size_t span = range_end - range_begin + 1;

    static constexpr size_t label_similar = 1;
    static constexpr size_t label_different = 2;

    Matrix_type x_train;
    Label_vector y_train;

    std::unordered_set<size_t> training_set;

    for(size_t i = range_begin; i < range_end; ++i) {
        training_set.insert(i);
        x_train.push_back(generate_row(frames_[i]));
        y_train.push_back(label_similar);
    }

    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dist{0, static_cast<int>(frames_.size())};

    // Randomly select span number of frames from rest of the data-set
    size_t count{0};
    while(count < span) {
        int random_index = dist(gen);
        if(training_set.find(random_index) != end(training_set)) {
            continue;
        }

        x_train.push_back(generate_row(frames_[random_index]));
        y_train.push_back(label_different);
        training_set.insert(random_index);

        ++count;
    }

    // Collect all of the data not in the range to as prediction data
    Matrix_type x_predict;
    for(size_t i = 0; i < range_begin; ++i) {
        x_predict.push_back(generate_row(frames_[i]));
    }

    for(size_t i = range_end; i < frames_.size(); ++i) {
        x_predict.push_back(generate_row(frames_[i]));
    }

    return {x_train, y_train, x_predict};
}

void fill_row(const ::ipme::wb::Frame::motion_capture_object& object,
              Similarity_finder::Row_type& row, size_t& idx)
{
    auto pose = object.pose();

    row.set_size(1, idx + 7);

    row(idx++) = pose.position().x();
    row(idx++) = pose.position().y();
    row(idx++) = pose.position().z();

    row(idx++) = pose.orientation().w();
    row(idx++) = pose.orientation().x();
    row(idx++) = pose.orientation().y();
    row(idx++) = pose.orientation().z();
}

Similarity_finder::Row_type Similarity_finder::generate_row(const Frame& frame)
{
    Row_type row;
    size_t idx{0};

    std::for_each(begin(frame.persons), end(frame.persons),
                  [&row, &idx](auto person) { fill_row(person, row, idx); });
    std::for_each(begin(frame.devices), end(frame.devices),
                  [&row, &idx](auto device) { fill_row(device, row, idx); });

    return row;
}

} // namespace ipme::wb
