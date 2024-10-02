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
#include "parameter_helpers.h"
#include "filters.h"


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
        Parameters parameters;

        findCudaDevice(argc, (const char**)argv);

        if (printfNPPinfo(argc, argv) == false)
        {
            exit(EXIT_SUCCESS);
        }

        // Parse and validate command line parameters
        int status = parameters.parseCmdLine(argc, argv);
        if (status == -1)
        {
            exit(EXIT_SUCCESS);
        }
        else if (status == -2)
        {
            exit(EXIT_FAILURE);
        }

        // declare a host image object for an 8-bit RGB image
        npp::ImageCPU_8u_C3 oHostSrc;
        // load image from disk
        stb::loadImage(parameters.getInputFilename(), oHostSrc);

        // declare a device image and copy construct from the host image,
        // i.e. upload host to device
        npp::ImageNPP_8u_C3 oDeviceSrc(oHostSrc);

        // set input size and ROI size
        parameters.setSrcSize({ (int)oDeviceSrc.width(), (int)oDeviceSrc.height() });
        parameters.setSizeROI({ (int)oDeviceSrc.width(), (int)oDeviceSrc.height() });

        // allocate device image for the filtered image
        npp::ImageNPP_8u_C3 oDeviceDst(oDeviceSrc.width(), oDeviceSrc.height());

        // run filter box
        filters::execute(parameters, oDeviceSrc, oDeviceDst);

        // declare a host image for the result
        npp::ImageCPU_8u_C3 oHostDst(oDeviceDst.size());

        // and copy the device result data into it
        oDeviceDst.copyTo(oHostDst.data(), oHostDst.pitch());

        // save image to disk
        stb::saveImage(parameters.getOutputFilename(), oHostDst);
        std::cout << "Saved image: " << parameters.getOutputFilename() << std::endl;

        // free device image
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
