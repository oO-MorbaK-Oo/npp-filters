#include "filters.h"
#include <cuda_runtime.h>
#include <npp.h>
#include <helper_cuda.h>
#include <Exceptions.h>

namespace filters
{
    void execute(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getFilterType() == "box")
        {
            box(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "sobel_h")
        {
            sobel_h(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "sobel_v")
        {
            sobel_v(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "roberts_up")
        {
            roberts_up(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "roberts_down")
        {
            roberts_down(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "laplace")
        {
            laplace(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "gauss")
        {
            gauss(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "highpass")
        {
            highpass(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "lowpass")
        {
            lowpass(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "sharpen")
        {
            sharpen(parameters, oDeviceSrc, oDeviceDst);
        }
        else if (parameters.getFilterType() == "wiener")
        {
            wiener(parameters, oDeviceSrc, oDeviceDst);
        }
    }



    void box(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterBox_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getMaskSize(), parameters.getAnchor()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterBoxBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getMaskSize(), parameters.getAnchor(), parameters.getBorderType()));
        }
    }

    void sobel_h(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterSobelHoriz_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterSobelHorizBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getBorderType()));
        }
    }

    void sobel_v(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterSobelVert_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterSobelVertBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getBorderType()));
        }
    }

    void roberts_down(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterRobertsDown_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterRobertsDownBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getBorderType()));
        }
    }

    void roberts_up(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterRobertsUp_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterRobertsUpBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getBorderType()));
        }
    }

    void laplace(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterLaplace_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterLaplaceBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize(), parameters.getBorderType()));
        }
    }

    void gauss(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterGauss_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterGaussBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize(), parameters.getBorderType()));
        }
    }

    void highpass(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterHighPass_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterHighPassBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize(), parameters.getBorderType()));
        }
    }

    void lowpass(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterLowPass_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterLowPassBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getNppiMaskSize(), parameters.getBorderType()));
        }
    }

    void sharpen(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        if (parameters.getBorderType() == NPP_BORDER_NONE)
        {
            NPP_CHECK_NPP(nppiFilterSharpen_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI()));
        }
        else
        {
            NPP_CHECK_NPP(nppiFilterSharpenBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
                oDeviceDst.data(), oDeviceDst.pitch(),
                parameters.getSizeROI(), parameters.getBorderType()));
        }
    }

    void wiener(const Parameters& parameters, const npp::ImageNPP_8u_C3& oDeviceSrc, npp::ImageNPP_8u_C3& oDeviceDst)
    {
        const Npp32f* noise = parameters.getNoise();
        Npp32f aNoise[3] = { noise[0], noise[1], noise[2] };
        NPP_CHECK_NPP(nppiFilterWienerBorder_8u_C3R(
            oDeviceSrc.data(), oDeviceSrc.pitch(), parameters.getSrcSize(), parameters.getSrcOffset(),
            oDeviceDst.data(), oDeviceDst.pitch(),
            parameters.getSizeROI(), parameters.getMaskSize(), parameters.getAnchor(), aNoise, parameters.getBorderType()));
    }
}