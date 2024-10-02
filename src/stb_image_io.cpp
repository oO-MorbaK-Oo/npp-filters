#include "stb_image_io.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

namespace stb
{
    void copy_8u_C1_to_8u_C3(const uint8_t* img, npp::ImageCPU_8u_C3& oImage)
    {
        Npp8u* pDstLine = oImage.data();
        const unsigned int nDstPitch = oImage.pitch();
        const uint8_t* pSrcLine = img;
        const unsigned int nSrcPitch = oImage.width();

        for (size_t iLine = 0; iLine < oImage.height(); ++iLine)
        {
            npp::Pixel<Npp8u, 4>* pDstPixels = (npp::Pixel<Npp8u, 4>*)pDstLine;
            for (size_t iPixel = 0; iPixel < oImage.width(); ++iPixel)
            {
                pDstPixels[iPixel].x = pSrcLine[iPixel];
                pDstPixels[iPixel].y = pSrcLine[iPixel];
                pDstPixels[iPixel].z = pSrcLine[iPixel];
            }
            pSrcLine += nSrcPitch;
            pDstLine += nDstPitch;
        }
    }

    void copy_8u_C3_to_8u_C3(const uint8_t* img, npp::ImageCPU_8u_C3& oImage)
    {
        Npp8u* pDstLine = oImage.data();
        const unsigned int nDstPitch = oImage.pitch();
        const uint8_t* pSrcLine = img;
        const unsigned int nSrcPitch = oImage.width() * 3;
        if (nSrcPitch == nDstPitch)
        {
            memcpy(pDstLine, pSrcLine, nSrcPitch * oImage.height());
        }
        else
        {
            for (size_t iLine = 0; iLine < oImage.height(); ++iLine)
            {
                memcpy(pDstLine, pSrcLine, nSrcPitch);
                pSrcLine += nSrcPitch;
                pDstLine += nDstPitch;
            }
        }
    }

    void copy_8u_C4_to_8u_C3(const uint8_t* img, npp::ImageCPU_8u_C3& oImage)
    {
        Npp8u* pDstLine = oImage.data();
        const unsigned int nDstPitch = oImage.pitch();
        const uint8_t* pSrcLine = img;
        const unsigned int nSrcPitch = oImage.width() * 4;

        for (size_t iLine = 0; iLine < oImage.height(); ++iLine)
        {
            const  npp::Pixel<Npp8u, 4>* pSrcPixels = (npp::Pixel<Npp8u, 4>*)pSrcLine;
            npp::Pixel<Npp8u, 3>* pDstPixels = (npp::Pixel<Npp8u, 3>*)pDstLine;
            for (size_t iPixel = 0; iPixel < oImage.width(); ++iPixel)
            {
                pDstPixels[iPixel].x = pSrcPixels[iPixel].x;
                pDstPixels[iPixel].y = pSrcPixels[iPixel].y;
                pDstPixels[iPixel].z = pSrcPixels[iPixel].z;
            }
            pSrcLine += nSrcPitch;
            pDstLine += nDstPitch;
        }
    }


    void loadImage(const std::string& rFileName, npp::ImageCPU_8u_C3& rImage)
    {
        int width = 0, height = 0, channels = 0;
        uint8_t* img = stbi_load(rFileName.c_str(), &width, &height, &channels, 0);
        if (img == NULL)
        {
            printf("Error: Can't load %s image\n", rFileName.c_str());
            throw npp::Exception("std::loadImage failed (stbi_load return null)");
        }
        if (channels != 1 && channels != 3 && channels != 4) {
            stbi_image_free(img);
            printf("Error: %s must be an 8, 24 or 32 bits image\n", rFileName.c_str());
            throw npp::Exception("std::loadImage failed (invalid pixel format)");
        }

        npp::ImageCPU_8u_C3 oImage(width, height);

        switch (channels) {
        case 1:
            copy_8u_C1_to_8u_C3(img, oImage);
            break;
        case 3:
            copy_8u_C3_to_8u_C3(img, oImage);
            break;
        case 4:
            copy_8u_C4_to_8u_C3(img, oImage);
            break;
        };

        // swap the user given image with our result image, effecively
        // moving our newly loaded image data into the user provided shell
        oImage.swap(rImage);
        stbi_image_free(img);
    }

    void saveImage(const std::string& rFileName, const npp::ImageCPU_8u_C3& rImage)
    {
        stbi_write_png(rFileName.c_str(), rImage.width(), rImage.height(), 3, rImage.data(), rImage.pitch());
    }
} // namespace stb