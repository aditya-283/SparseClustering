/**
 * Reading spectra
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

void parseMgfFile(FILE* filePath, std::vector<spectrum_t> spectra);



#endif
