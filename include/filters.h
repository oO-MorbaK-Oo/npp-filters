#ifndef FILTERS_H
#define FILTERS_H
#pragma once
#include "parameter_helpers.h"
#include <ImagesNPP.h>

namespace filters
{
    void execute(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);

    void box(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void sobel_h(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void sobel_v(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void roberts_down(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void roberts_up(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void laplace(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void gauss(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void highpass(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void lowpass(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void sharpen(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
    void wiener(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst);
}

#endif // FILTERS_H