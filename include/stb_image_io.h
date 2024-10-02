#ifndef STB_IMAGE_IO_H_
#define STB_IMAGE_IO_H_
#pragma once

#include <ImagesCPU.h>

namespace stb {

    // Load 8,24,32 bits image using stb_image
    // and return an npp::ImageCPU_8u_C3
    void loadImage(const std::string& rFileName, npp::ImageCPU_8u_C3& rImage);

    void saveImage(const std::string& rFileName, const npp::ImageCPU_8u_C3& rImage);
}

#endif //STB_IMAGE_IO_H_