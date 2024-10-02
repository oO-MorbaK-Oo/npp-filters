#ifndef PARAMETER_HELPERS_H
#define PARAMETER_HELPERS_H
#pragma once
#include <string>
#include <npp.h>

class Parameters {
    std::string _sInputFile;
    std::string _sOutputFile;
    std::string _sFilterType;
    std::string _sBorderType;
    NppiBorderType _eBorderType;

    NppiSize _oSrcSize;

    // TODO: add command line parameters
    NppiPoint _oSrcOffset = { 0, 0 };
    NppiSize _oSizeROI;
    NppiSize _oMaskSize = { 5, 5 };
    NppiPoint _oAnchor = { 5 / 2, 5 / 2 };
    NppiMaskSize _eNppiMaskSize = NPP_MASK_SIZE_5_X_5;
    Npp32f _aNoise[3] = { 0.5f, 0.47f, 0.53f };
public:
    int parseCmdLine(int argc, char* argv[]);

    const std::string& getInputFilename() const { return _sInputFile; }
    const std::string& getOutputFilename() const { return _sOutputFile; }

    const std::string& getFilterType() const { return _sFilterType; }

    const NppiBorderType getBorderType() const { return _eBorderType; }

    const NppiSize& getSrcSize() const { return _oSrcSize; }

    const NppiPoint& getSrcOffset() const { return _oSrcOffset; }

    const NppiSize& getSizeROI() const { return _oSizeROI; }

    const NppiSize& getMaskSize() const { return _oMaskSize; }

    const NppiPoint& getAnchor() const { return _oAnchor; }

    const NppiMaskSize getNppiMaskSize() const { return _eNppiMaskSize; }

    const Npp32f* getNoise() const { return _aNoise; }

    void setSrcSize(const NppiSize& oSize);
    void setSizeROI(const NppiSize& oSize);

private:
    bool isFilterBorderCompatible() const;
    bool isValidInputFilename() const;
    std::string buildOutputFilename() const;
};

#endif // PARAMETER_HELPERS_H