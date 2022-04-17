/**
 * Reading spectra
 * @author Aditya Bhagwat abhagwa2@cs.cmu.edu
 */

#include "spectra.h"
#include <stdlib.h>
#include <fstream>
#include <string>

typedef enum parse_state { NO_PARSE, PROPERTIES, PEAKS } parse_state;

std::vector<spectrum_t> parseMgfFile(std::string path) {
    namespace fs = std::filesystem;

    // std::ifstream file(path);
    std::vector<spectrum_t> spectra;
    spectrum_t* cur;
    parse_state state = NO_PARSE;

    std::ifstream file(path, std::ios::in | std::ios::binary);
    const auto sz = fs::file_size(path);
    std::string result(sz, '\0');
    file.read(result.data(), sz);

    std::istringstream stream(result);
    std::string line;
    while (std::getline(stream, line)) {
        if (state == NO_PARSE && line.starts_with("BEGIN IONS")) {
            cur = new spectrum_t();
            state = PROPERTIES;
        } else if(state == PROPERTIES && line.starts_with("TITLE=")) {
            std::string delimiter = "TITLE=";
            std::string title_token = line.substr(delimiter.size()); 
            cur->title = title_token;
        } else if (state == PROPERTIES && line.starts_with("PEPMASS=")) {
            std::string delimiter = "PEPMASS=";
            std::string token = line.substr(delimiter.size()); 
            cur->pepmass = std::stof(token);
        } else if (state == PROPERTIES && line.starts_with("RTINSECONDS=")) {
            std::string delimiter = "RTINSECONDS=";
            std::string token = line.substr(delimiter.size()); 
            state = PEAKS;
            cur->rtin_seconds = std::stof(token);
        } else if (line.starts_with("END IONS")) {
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

void printSpectrum(spectrum_t spectrum, bool verbose) {
    printf("Title: %s\n", spectrum.title.c_str());
    printf("Pepmass: %f\n", spectrum.pepmass);
    printf("Rtin Seconds: %f\n", spectrum.rtin_seconds);
    printf("Number of peaks: %zu\n", spectrum.num_peaks);
    // for (int i=0; i<spectrum.num_peaks; i++) {
    //     printf("%f: %lu\n", spectrum.peaks[i], spectrum.intensities[i]);
    // }
    printf("\n");
} 

