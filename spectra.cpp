/**
 * @file spectra.cpp
 * @brief Reading spectra from .mgf files and printing spectra to stdout.
 * @author Aditya Bhagwat abhagwa2@cs.cmu.edu
 */

#include "spectra.h"
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iterator>

/**
* @brief Parsing states for parsing the .mgf file. 
* Parsing each spectrum involves cycling through the three states.
*/

typedef enum parse_state { NO_PARSE, PROPERTIES, PEAKS } parse_state;

bool inline starts_with(const std::string& big, const std::string& small) {
    return small.length() <= big.length() 
        && equal(small.begin(), small.end(), big.begin());
}

/**
* @brief 
* @param
* @return
*/
std::vector<spectrum_t> parseMgfFile(std::string path) {
    std::vector<spectrum_t> spectra;
    spectrum_t* cur;
    parse_state state = NO_PARSE;

    std::ifstream ifs(path);
    std::string result(std::istreambuf_iterator<char>{ifs}, {});
    std::istringstream stream(result);
    std::string line;
    while (std::getline(stream, line)) {
        if (state == NO_PARSE && starts_with(line, "BEGIN IONS")) {
            cur = new spectrum_t();
            state = PROPERTIES;
        } else if(state == PROPERTIES && starts_with(line, "TITLE=")) {
            std::string delimiter = "TITLE=";
            std::string title_token = line.substr(delimiter.size()); 
            cur->title = title_token;
        } else if (state == PROPERTIES && starts_with(line, "PEPMASS=")) {
            std::string delimiter = "PEPMASS=";
            std::string token = line.substr(delimiter.size()); 
            cur->pepmass = std::stof(token);
        } else if (state == PROPERTIES && starts_with(line, "RTINSECONDS=")) {
            std::string delimiter = "RTINSECONDS=";
            std::string token = line.substr(delimiter.size()); 
            state = PEAKS;
            cur->rtin_seconds = std::stof(token);
        } else if (starts_with(line, "END IONS")) {
            state = NO_PARSE;
            spectra.push_back(*cur);
        } else if (state == PEAKS) {
            cur->num_peaks += 1;
            std::string delimiter = " ";
            std::string peak_str = line.substr(0, line.find(delimiter)); 
            std::string intensity_str = line.substr(line.find(delimiter) + 1);
            cur->peaks.push_back(std::stof(peak_str));
            cur->intensities.push_back(std::stof(intensity_str));
        }
    }

    return spectra;
}

/**
* @brief 
* @param
* @return
*/
void printSpectrum(spectrum_t spectrum, bool verbose) {
    printf("Title: %s\n", spectrum.title.c_str());
    printf("Pepmass: %f\n", spectrum.pepmass);
    printf("Rtin Seconds: %f\n", spectrum.rtin_seconds);
    printf("Number of peaks: %zu\n", spectrum.num_peaks);
    if (verbose) {
        for (int i=0; i<spectrum.num_peaks; i++) {
            printf("%f: %lu\n", spectrum.peaks[i], spectrum.intensities[i]);
        }
    }
    printf("\n");
} 

