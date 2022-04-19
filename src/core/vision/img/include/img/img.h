#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

#include <cassert>

#include "png.h"

namespace kioku
{
namespace vision
{

enum class ChannelOrder : std::uint8_t
{
    kRGB,
    kBGR,
    kRGBA,
    kBGRA,
    kOther,
};

constexpr std::size_t getChannelCount(ChannelOrder ordering)
{
    std::size_t channel_count{0U};
    if (ordering == ChannelOrder::kRGB or ordering == ChannelOrder::kBGR)
    {
        channel_count = 3;
    }
    else if (ordering == ChannelOrder::kRGBA or ordering == ChannelOrder::kBGRA)
    {
        channel_count = 4;
    }
    return channel_count;
}

enum class StoragePolicy : std::uint8_t
{
    kLinear,
    kPlanar
};

template <std::size_t width, std::size_t height, typename DataType, ChannelOrder order, StoragePolicy method, std::size_t custom_channel_count = 0U>
class Image
{
    static_assert(std::is_arithmetic<DataType>::value, "Only arithmetic types are allowed for data type.");
    static_assert((width > 0U && height > 0U), "An image must have at least 1 pixel of width and height.");
    // static_assert(getChannelCount(order) );

    static constexpr std::size_t channels{(custom_channel_count != 0U) ? custom_channel_count : getChannelCount(order)};
    static constexpr std::size_t kNumElements{width*height*channels};
    static constexpr std::size_t kNumBytes{kNumElements*sizeof(DataType)};


    template <std::size_t target_width, std::size_t target_height>
    Image<target_width, target_height, DataType, order, method> resize();

    template <std::size_t crop_width, std::size_t crop_height>
    Image<crop_width, crop_height, DataType, order, method> makeCrop();

    template <std::size_t channel_idx>
    Image<width, height, 1U, DataType, order, method> slice()
    {
        // toPlanar()[channel_idx];
    }

    template <typename TargetDataype, ChannelOrder target_order, StoragePolicy target_policy>
    Image<width, height, TargetDataype, target_order, target_policy> convertTo();

    // todo
    // rotate(float const angle);
    // mirrorHorizontal();
    // mirrorVertical();

    // png/jpeg for the following. system? custom?
    void write(std::string const& path, std::string const& file_name);
    void read(std::string const& path, std::string const& file_name)
    {

    }

    static Image fromData(DataType const* const data, std::size_t const data_size)
    {
        static_cast<void>(data);
        assert(data_size == kNumElements);
    }

    template <typename Container>
    static Image fromContainer(Container const& container)
    {
        assert(container.size() == kNumElements);
    }
};

template <std::size_t width, std::size_t height, std::size_t channels, typename DataType>
using ImageRGB = Image<width, height, channels, DataType, ChannelOrder::kRGB, StoragePolicy::kLinear>;

template <std::size_t width, std::size_t channels, std::size_t height, typename DataType>
using PlanarImageRGB = Image<width, height, channels, DataType, ChannelOrder::kRGB, StoragePolicy::kPlanar>;


}  // namespace vision
}  // namespace kioku