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

void Similarity_finder::on_find_similar(size_t begin, size_t end)
{
    qDebug() << "finding similarity to range " << begin << "-" << end;

    static constexpr size_t min_range_size = 100;
    static constexpr size_t buffer_length = 10;

    if(end - begin < min_range_size) {
        throw std::runtime_error{"range needs to be at least " +
                                 std::to_string(min_range_size) + " frames"};
    }

    int new_begin = begin + buffer_length;
    int new_end = end - buffer_length;

    auto[x_train, y_train, x_predict] = split_data(new_begin, new_end);

    DEBUG() << "x_train: " << x_train.size()
            << " samples, y_train: " << y_train.size()
            << "labels, x_predict: " << x_predict.size() << " data points";
}

std::tuple<Similarity_finder::Matrix_type, Similarity_finder::Label_vector,
           Similarity_finder::Matrix_type>
Similarity_finder::split_data(size_t range_begin, size_t range_end)
{
    size_t range_size = range_end - range_begin + 1;

    static constexpr size_t label_similar = 1;
    static constexpr size_t label_different = 2;

    Matrix_type x_train;
    Label_vector y_train;

    std::unordered_set<size_t> included_indices;

    for(size_t i = range_begin; i < range_end; ++i) {
        included_indices.insert(i);
        x_train.push_back(generate_row(frames_[i]));
        y_train.push_back(label_similar);
    }

    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dist{0, static_cast<int>(frames_.size())};

    size_t count{0};

    while(true) {
        int random_index = dist(gen);
        if(included_indices.find(random_index) != end(included_indices)) {
            continue;
        }

        x_train.push_back(generate_row(frames_[random_index]));
        y_train.push_back(label_different);
        included_indices.insert(random_index);
        ++count;

        if(count >= range_size) {
            break;
        }
    }

    Matrix_type x_predict;
    for(size_t i = 0; i < frames_.size(); ++i) {
        if(included_indices.find(i) != end(included_indices)) {
            continue;
        }

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
