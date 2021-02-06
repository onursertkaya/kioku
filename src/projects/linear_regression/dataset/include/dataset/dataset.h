#ifndef SRC_PROJECTS_LINEAR_REGRESSION_DATASET_INCLUDE_DATASET_DATASET_H
#define SRC_PROJECTS_LINEAR_REGRESSION_DATASET_INCLUDE_DATASET_DATASET_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "dstruct/static_array.h"
#include "math/vec_n.h"

namespace kioku
{
namespace ml
{

template <typename T, uint32_t In, uint32_t Out>
struct DataPoint
{
    core::vecN<T, In> data;
    core::vecN<T, Out> label;
};

template <typename T, uint32_t NumSamples, uint32_t In, uint32_t Out>
class Dataset
{
 public:
    explicit Dataset(std::string const& path) : path_{path}
    {
        std::fstream file_stream{path};
        std::string line;
        while (std::getline(file_stream, line))
        {
            std::stringstream line_stream{line};
            std::string entry;
            while (std::getline(line_stream, entry, ','))
            {
                if (entry.find("Iris") != std::string::npos)
                {
                    T num{std::stof(entry)};
                }
            }
            std::cout << line << std::endl;
        }
    }

    DataPoint<T, In, Out> const& next()
    {
        if (current_data_point_ < data_points_.size())
        {
            ++current_data_point_;
            return data_points_[current_data_point_];
        }
        // fixme, early return is faulty logic???

        current_data_point_ = 0U;
    }

 private:
    std::string path_;
    uint32_t current_data_point_{0U};
    core::StaticArray<DataPoint<T, In, Out>, NumSamples> data_points_{};
};

}  // namespace ml
}  // namespace kioku

#endif  // SRC_PROJECTS_LINEAR_REGRESSION_DATASET_INCLUDE_DATASET_DATASET_H
