#include <iostream>
#include <vector>
#include "parameter_helpers.h"
#include "helper_string.h"

std::string getFilterType(int argc, char* argv[])
{
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

    std::string sFilterType = filterTypes[0];

    char* arg = nullptr;
    if (checkCmdLineFlag(argc, (const char**)argv, "filter"))
    {
        getCmdLineArgumentString(argc, (const char**)argv, "filter", &arg);
    }

    if (arg)
    {
        sFilterType = arg;
    }

    if (std::find(filterTypes.begin(), filterTypes.end(), sFilterType) == filterTypes.end())
    {
        sFilterType = filterTypes[0];
    }
    return sFilterType;
}

std::string getBorderType(int argc, char* argv[]/*, const std::string& sFilterType*/)
{
    const std::vector<std::string> borderTypes = {
    "none",
    //"constant",
    "replicate",
    //"wrap",
    //"mirror",
    };

    std::string sBorderType = borderTypes[1];

    char* arg = nullptr;
    if (checkCmdLineFlag(argc, (const char**)argv, "border"))
    {
        getCmdLineArgumentString(argc, (const char**)argv, "border", &arg);
    }

    if (arg)
    {
        sBorderType = arg;
    }

    if (std::find(borderTypes.begin(), borderTypes.end(), sBorderType) == borderTypes.end())
    {
        sBorderType = borderTypes[1];
    }

    return sBorderType;

}

NppiBorderType sBorderTypeToEnum(const std::string& sBorderType)
{
    if (sBorderType == "constant")
    {
        return NPP_BORDER_CONSTANT;
    }
    else if (sBorderType == "replicate")
    {
        return NPP_BORDER_REPLICATE;
    }
    else if (sBorderType == "wrap")
    {
        return NPP_BORDER_WRAP;
    }
    else if (sBorderType == "mirror")
    {
        return NPP_BORDER_MIRROR;
    }
    return NPP_BORDER_NONE;
}

std::string getInputFileName(int argc, char* argv[])
{
    char* arg = nullptr;
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
        return arg;
    }
    return "Lena.png";
}


int Parameters::parseCmdLine(int argc, char* argv[])
{
    // Filter type
    _sFilterType = ::getFilterType(argc, argv);

    // Border type
    _sBorderType = ::getBorderType(argc, argv);
    _eBorderType = ::sBorderTypeToEnum(_sBorderType);

    // check border / filter compatibility
    if (!isFilterBorderCompatible())
    {
        return -1;
    }

    // input Filename
    _sInputFile = ::getInputFileName(argc, argv);

    // check if inputname file is valid
    if (!isValidInputFilename())
    {
        return -2;
    }

    // output Filename
    _sOutputFile = buildOutputFilename();
    if (checkCmdLineFlag(argc, (const char**)argv, "output"))
    {
        char* outputFilePath;
        getCmdLineArgumentString(argc, (const char**)argv, "output", &outputFilePath);
        _sOutputFile = outputFilePath;
    }

    return 0;
}


void Parameters::setSrcSize(const NppiSize& oSize)
{
    _oSrcSize = oSize;
}

void Parameters::setSizeROI(const NppiSize& oSize)
{
    _oSizeROI = oSize;
}

bool Parameters::isFilterBorderCompatible() const
{
    bool compatible = true;
    if (_sFilterType != "wiener") {
        if (_sBorderType != "none" && _sBorderType != "replicate") {
            std::cout << _sFilterType << " filter support none or replicate border mode" << std::endl;
            compatible = false;
        }
    }
    else {
        if (_sBorderType != "replicate") {
            std::cout << _sFilterType << " filter support replicate border mode" << std::endl;
            compatible = false;
        }
    }
    return compatible;
}

bool Parameters::isValidInputFilename() const
{
    std::ifstream infile(_sInputFile.data(), std::ifstream::in);
    bool ok = infile.good();
    if (ok)
    {
        std::cout << "npp-filters opened: <" << _sInputFile.data()
            << "> successfully!" << std::endl;
    }
    else
    {
        std::cout << "npp-filters unable to open: <" << _sInputFile.data() << ">"
            << std::endl;

    }
    infile.close();
    return ok;
}

std::string Parameters::buildOutputFilename() const
{
    std::string sResultFilename = _sInputFile;

    std::string::size_type dot = sResultFilename.rfind('.');

    if (dot != std::string::npos)
    {
        sResultFilename = sResultFilename.substr(0, dot);
    }

    sResultFilename += "_filter_" + _sFilterType + "_" + _sBorderType + ".png";
    return sResultFilename;
}