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

void printSpectrum(spectrum_t spectrum) {
    printf("Title: %s\n", spectrum.title.c_str());
    printf("Pepmass: %f\n", spectrum.pepmass);
    printf("Rtin Seconds: %f\n", spectrum.rtin_seconds);
    printf("Number of peaks: %zu\n", spectrum.num_peaks);
    // for (int i=0; i<spectrum.num_peaks; i++) {
    //     printf("%f: %lu\n", spectrum.peaks[i], spectrum.intensities[i]);
    // }
    printf("\n");
} 

#endif
