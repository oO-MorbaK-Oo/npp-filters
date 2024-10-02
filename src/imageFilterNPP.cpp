/* Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#define WINDOWS_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#pragma warning(disable : 4819)
#endif


#include <vector>
#include <fstream>
#include <iostream>

#include <cuda_runtime.h>
#include <npp.h>

#include <helper_cuda.h>
#include <helper_string.h>

#include <ImagesNPP.h>

#include "stb_image_io.h"


bool printfNPPinfo(int argc, char* argv[])
{
    const NppLibraryVersion* libVer = nppGetLibVersion();

    printf("NPP Library Version %d.%d.%d\n", libVer->major, libVer->minor,
        libVer->build);

    int driverVersion, runtimeVersion;
    cudaDriverGetVersion(&driverVersion);
    cudaRuntimeGetVersion(&runtimeVersion);

    printf("  CUDA Driver  Version: %d.%d\n", driverVersion / 1000,
        (driverVersion % 100) / 10);
    printf("  CUDA Runtime Version: %d.%d\n", runtimeVersion / 1000,
        (runtimeVersion % 100) / 10);

    // Min spec is SM 1.0 devices
    bool bVal = checkCudaCapabilities(1, 0);
    return bVal;
}



int main(int argc, char* argv[])
{
    printf("%s Starting...\n\n", argv[0]);

    try
    {
        std::string sFilename;
        std::string sFilterType;
        std::string sBorderType;
        const std::vector<std::string> filterTypes = {
            "box",
            "sobel_h",
            "sobel_v",
            "roberts_up",
            "roberts_down",
            "laplace",
            "gauss",
            "highpass",
            "lowpass",
            "sharpen",
            "wiener",
        };

        const std::vector<std::string> borderTypes = {
            "none",
            //"constant",
            "replicate",
            //"wrap",
            //"mirror",
        };

        findCudaDevice(argc, (const char**)argv);

        if (printfNPPinfo(argc, argv) == false)
        {
            exit(EXIT_SUCCESS);
        }

        // Filter type
        char* arg = nullptr;
        if (checkCmdLineFlag(argc, (const char**)argv, "filter"))
        {
            getCmdLineArgumentString(argc, (const char**)argv, "filter", &arg);
        }

        if (arg)
        {
            sFilterType = arg;
        }
        else
        {
            sFilterType = filterTypes[0];
        }

        if (std::find(filterTypes.begin(), filterTypes.end(), sFilterType) == filterTypes.end())
        {
            sFilterType = filterTypes[0];
        }

        // Border type
        arg = nullptr;
        if (checkCmdLineFlag(argc, (const char**)argv, "border"))
        {
            getCmdLineArgumentString(argc, (const char**)argv, "border", &arg);
        }

        if (arg)
        {
            sBorderType = arg;
        }
        else
        {
            sBorderType = borderTypes[1];
        }

        if (std::find(borderTypes.begin(), borderTypes.end(), sBorderType) == borderTypes.end())
        {
            sBorderType = borderTypes[1];
        }

        // check border with filter compatibility
        if (sFilterType != "wiener") {
            if (sBorderType != "none" && sBorderType != "replicate") {
                std::cout << sFilterType << " filter support none or replicate border mode" << std::endl;
                return 0;
            }
        }
        else {
            if (sBorderType != "replicate") {
                std::cout << sFilterType << " filter support replicate border mode" << std::endl;
                return 0;
            }
        }

        arg = nullptr;
        if (checkCmdLineFlag(argc, (const char**)argv, "input"))
        {
            getCmdLineArgumentString(argc, (const char**)argv, "input", &arg);
        }
        else
        {
            arg = sdkFindFilePath("Lena.png", argv[0]);
        }

        if (arg)
        {
            sFilename = arg;
        }
        else
        {
            sFilename = "Lena.png";
        }

        // if we specify the filename at the command line, then we only test
        // sFilename[0].
        int file_errors = 0;
        std::ifstream infile(sFilename.data(), std::ifstream::in);
        if (infile.good())
        {
            std::cout << "npp-filters opened: <" << sFilename.data()
                << "> successfully!" << std::endl;
            file_errors = 0;
            infile.close();
        }
        else
        {
            std::cout << "npp-filters unable to open: <" << sFilename.data() << ">"
                << std::endl;
            file_errors++;
            infile.close();
        }

        if (file_errors > 0)
        {
            exit(EXIT_FAILURE);
        }

        std::string sResultFilename = sFilename;

        std::string::size_type dot = sResultFilename.rfind('.');

        if (dot != std::string::npos)
        {
            sResultFilename = sResultFilename.substr(0, dot);
        }

        sResultFilename += "_filter_" + sFilterType + "_" + sBorderType + ".png";

        if (checkCmdLineFlag(argc, (const char**)argv, "output"))
        {
            char* outputFilePath;
            getCmdLineArgumentString(argc, (const char**)argv, "output",
                &outputFilePath);
            sResultFilename = outputFilePath;
        }

        // declare a host image object for an 8-bit grayscale image
        npp::ImageCPU_8u_C3 oHostSrc;
        // load gray-scale image from disk
        stb::loadImage(sFilename, oHostSrc);
        // declare a device image and copy construct from the host image,
        // i.e. upload host to device
        npp::ImageNPP_8u_C3 oDeviceSrc(oHostSrc);

        // create struct with the ROI size
        NppiSize oSrcSize = { (int)oDeviceSrc.width(), (int)oDeviceSrc.height() };
        NppiPoint oSrcOffset = { 0, 0 };
        NppiSize oSizeROI = { (int)oDeviceSrc.width(), (int)oDeviceSrc.height() };


        // allocate device image for the rotated image
        npp::ImageNPP_8u_C3 oDeviceDst(oSizeROI.width, oSizeROI.height);

        NppiBorderType eBorderType = NPP_BORDER_NONE;
        if (sBorderType == "constant")
        {
            eBorderType = NPP_BORDER_CONSTANT;
        }
        else if (sBorderType == "replicate")
        {
            eBorderType = NPP_BORDER_REPLICATE;
        }
        else if (sBorderType == "wrap")
        {
            eBorderType = NPP_BORDER_WRAP;
        }
        else if (sBorderType == "mirror")
        {
            eBorderType = NPP_BORDER_MIRROR;
        }

        // TODO: read value from command line
        NppiMaskSize eMaskSize = NPP_MASK_SIZE_5_X_5;
        NppiSize oMaskSize = { 5, 5 };
        NppiPoint oAnchor = { oMaskSize.width / 2, oMaskSize.height / 2 };
        Npp32f aNoise[] = { 0.5f, 0.47f, 0.53f };

        // run filter box
        if (sFilterType == "box")
        {
            if (eBorderType == NPP_BORDER_NONE) {
                NPP_CHECK_NPP(nppiFilterBox_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, oMaskSize, oAnchor));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterBoxBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, oMaskSize, oAnchor, eBorderType));
            }
        }
        else if (sFilterType == "sobel_h")
        {

            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterSobelHoriz_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterSobelHorizBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eBorderType));
            }
        }
        else if (sFilterType == "sobel_v")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterSobelVert_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterSobelVertBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eBorderType));
            }
        }
        else if (sFilterType == "roberts_up")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterRobertsUp_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterRobertsUpBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eBorderType));
            }
        }
        else if (sFilterType == "roberts_down")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterRobertsDown_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterRobertsDownBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eBorderType));
            }
        }
        else if (sFilterType == "laplace")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterLaplace_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterLaplaceBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize, eBorderType));
            }
        }
        else if (sFilterType == "gauss")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterGauss_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterGaussBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize, eBorderType));
            }
        }
        else if (sFilterType == "highpass")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterHighPass_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterHighPassBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize, eBorderType));
            }
        }
        else if (sFilterType == "lowpass")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterLowPass_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterLowPassBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eMaskSize, eBorderType));
            }
        }
        else if (sFilterType == "sharpen")
        {
            if (eBorderType == NPP_BORDER_NONE)
            {
                NPP_CHECK_NPP(nppiFilterSharpen_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(),
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI));
            }
            else
            {
                NPP_CHECK_NPP(nppiFilterSharpenBorder_8u_C3R(
                    oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                    oDeviceDst.data(), oDeviceDst.pitch(),
                    oSizeROI, eBorderType));
            }
        }
        else if (sFilterType == "wiener")
        {
            NPP_CHECK_NPP(nppiFilterWienerBorder_8u_C3R(
                oDeviceSrc.data(), oDeviceSrc.pitch(), oSrcSize, oSrcOffset,
                oDeviceDst.data(), oDeviceDst.pitch(),
                oSizeROI, oMaskSize, oAnchor, aNoise, eBorderType));
        }


        // declare a host image for the result
        npp::ImageCPU_8u_C3 oHostDst(oDeviceDst.size());
        // and copy the device result data into it
        oDeviceDst.copyTo(oHostDst.data(), oHostDst.pitch());

        stb::saveImage(sResultFilename, oHostDst);
        std::cout << "Saved image: " << sResultFilename << std::endl;

        nppiFree(oDeviceSrc.data());
        nppiFree(oDeviceDst.data());

        exit(EXIT_SUCCESS);
    }
    catch (npp::Exception& rException)
    {
        std::cerr << "Program error! The following exception occurred: \n";
        std::cerr << rException << std::endl;
        std::cerr << "Aborting." << std::endl;

        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "Program error! An unknown type of exception occurred. \n";
        std::cerr << "Aborting." << std::endl;

        exit(EXIT_FAILURE);
        return -1;
    }

    return 0;
}
