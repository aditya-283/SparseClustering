/**
 * @file spectra.h
 * @brief Reading spectra from .mgf files and printing spectra to stdout.
 * @author Aditya Bhagwat abhagwa2@cs.cmu.edu
 */

#ifndef SPECTRA_H
#define SPECTRA_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>

typedef float peak_t;

typedef struct {
    float pepmass;
    float rtin_seconds;
    std::string title;
    size_t num_peaks;
    std::vector<peak_t> peaks;
    std::vector<size_t> intensities;
} spectrum_t;

std::vector<spectrum_t> parseMgfFile(std::string path);
void printSpectrum(spectrum_t spectrum, bool verbose);

#endif
