#ifndef DIGGLER_CONTENT_IMAGE_FORMAT_HPP
#define DIGGLER_CONTENT_IMAGE_FORMAT_HPP

#include "../../platform/FourCC.hpp"

namespace diggler {
namespace content {
namespace Image {

using Format = FourCC;

namespace ImageFormats {
  constexpr Format
    FLIF = MakeFourCC('F', 'L', 'I', 'F'),
    JPEG = MakeFourCC('J', 'P', 'E', 'G'),
    PNG  = MakeFourCC('P', 'N', 'G', '\0');
}

}
}
}

#endif /* DIGGLER_CONTENT_IMAGE_FORMAT_HPP */
