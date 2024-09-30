#!/usr/bin/env bash
make clean build

filters=(box sobel_h sobel_v roberts_up roberts_down laplace gauss highpass lowpass sharpen wiener)
borders=(none replicate)

for filter in ${filters[@]} ; do
    for border in ${borders[@]} ; do
        ./bin/npp-filters --filter $filter --border $border
    done
done
