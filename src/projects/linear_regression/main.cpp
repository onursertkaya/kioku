#include <iostream>
#include <tuple>

#include "dataset/dataset.h"
#include "loss/loss.h"
#include "math/mat_n.h"
#include "math/vec_n.h"

constexpr float dlr{1E-2F};

namespace kioku
{
namespace ml
{

template <typename T, uint32_t In, uint32_t Out>
class FFNLayer
{
 public:
    explicit FFNLayer(T learning_rate) :
        in_(0.0F),
        in_grad_(0.0F),
        W_(1.0F),
        b_(0.0F),
        out_(0.0F),
        learning_rate_{learning_rate}
    {
    }

    auto forward(core::matN<T, 1U, In> input)
    {
        in_ = input;
        out_ = in_.dot(W_) + b_;

        return out_;
    }

    auto backward(core::matN<T, 1U, Out> const& out_grad)
    {
        auto W_grad{in_.transpose().dot(out_grad)};  // In x 1  . 1 x Out  > In x Out
        const auto& b_grad{out_grad};

        auto in_grad{out_grad.dot(W_.transpose())};  // 1 x Out . Out x In > 1 x In

        // store
        in_grad_ = in_grad;

        // update
        W_ = W_ - W_grad * learning_rate_;
        b_ = b_ - b_grad * learning_rate_;

        // todo: return grad for previous layer to use
    }

    core::matN<T, In, Out> const& getWeights() { return W_; }
    core::matN<T, 1U, Out> const& getBias() { return b_; }

 private:
    core::matN<T, 1U, In> in_;
    core::matN<T, 1U, In> in_grad_;
    core::matN<T, In, Out> W_;
    core::matN<T, 1U, Out> b_;
    core::matN<T, 1U, Out> out_;
    T learning_rate_;
};

template <typename T>
std::tuple<core::matN<T, 100U, 1U>, core::matN<T, 100U, 1U>, core::matN<T, 100U, 1U>>
generateNoisyDataset(T const a, T const b, T const noise_stddev)
{
    core::matN<T, 100U, 1U> in_data(0.0F);
    core::matN<T, 100U, 1U> out_data(0.0F);
    core::matN<T, 100U, 1U> truth_data(0.0F);

    // y = ax + b + Noise
    for (uint32_t k{0U}; k < 100U; ++k)
    {
        auto in{static_cast<T>(k) * 0.1F - 5.0F};  // -5.0 ~ 5.0
        auto out{in * a + b};
        in_data[k][0U] = in;
        out_data[k][0U] = out;
        truth_data[k][0U] = out;
    }

    // add noise
    for (uint32_t k{0U}; k < 100U; ++k)
    {
        out_data[k] = out_data[k] + core::vecN<T, 1U>::random(0.0F, noise_stddev, k);
    }
    return {in_data, out_data, truth_data};
}

// template <typename T, uint32_t In, uint32_t Out, typename... Arch>
template <typename T, uint32_t In, uint32_t Out, uint32_t NumLayers = 1U>
class FeedForwardNetwork
{
 public:
    explicit FeedForwardNetwork(T learning_rate) :
        layers_{FFNLayer<T, In, Out>{learning_rate}}
    {
    }

    // void step(Datapoint const& datapoint)
    void step(core::matN<T, 1U, In> in_point, core::matN<T, 1U, Out> truth_point, bool print)
    {
        auto out = layers_[0U].forward(in_point);

        auto loss_vec{l2_dist(out, truth_point)};
        auto derivative_vec{l2_dist_derivative(out, truth_point)};

        if (print)
        {
            std::cout << "loss value: " << loss_vec.sum() << std::endl;
        }

        layers_[0U].backward(derivative_vec);
    }

    // void train(uint32_t epochs, Dataset const& dataset)
    void train(uint32_t epochs)
    {
        constexpr float w{-10.0F};
        constexpr float b{22.0F};
        auto dataset = generateNoisyDataset(w, b, 4.0F);
        auto in_set = std::get<0>(dataset);
        auto out_set = std::get<1>(dataset);

        std::cout << in_set << std::endl;
        std::cout << "=============" << std::endl;
        std::cout << out_set << std::endl;

        while (current_epoch_ < epochs)
        {
            std::cout << "[" << current_epoch_ << " / " << epochs << "]" << std::endl;
            for (uint32_t datapoint_idx{0U}; datapoint_idx < 100U; ++datapoint_idx)
            {
                auto in_point = core::matN<T, 1U, In>{in_set[datapoint_idx]};
                auto out_point = core::matN<T, 1U, Out>{out_set[datapoint_idx]};
                bool p = (datapoint_idx == 0U);
                step(in_point, out_point, p);
            }
            ++current_epoch_;
        }

        std::cout << "final weights" << std::endl;
        std::cout << layers_[0U].getWeights() << std::endl;

        std::cout << "expected weights" << std::endl;
        std::cout << w << std::endl;

        std::cout << "final bias" << std::endl;
        std::cout << layers_[0U].getBias() << std::endl;

        std::cout << "expected bias" << std::endl;
        std::cout << b << std::endl;
    }

    // template <typename Vector>
    // void predict(Vector input)
    // {

    // }

    // static FeedForwardNetwork deserialize(std::string& path)
    // {
    //     FeedForwardNetwork retval;
    //     // fixme, implement
    //     return retval;
    // }

    // void serialize()
    // {

    // }

 private:
    // fixme, calculate in and out at compile time for each layer???
    // different types. should I use tuple then?
    // std::array<_FFNLayer<T, In, Out>, sizeof...(Arch)> layers_;
    std::array<FFNLayer<T, In, Out>, NumLayers> layers_;
    uint32_t current_epoch_{0U};
};

// template <typename T, uint32_t In, uint32_t Out>
// using LinearRegression = FeedForwardNetwork<T, In, Out, 1U>;

}  // namespace ml
}  // namespace kioku

int main()
{
    using T = float;
    kioku::ml::FeedForwardNetwork<T, 1, 1, 1> lr{dlr};

    lr.train(50);

    // kioku::ml::Dataset<T, 3, 2> dset{"/path/to/dataset.txt"};

    return 0;
}
