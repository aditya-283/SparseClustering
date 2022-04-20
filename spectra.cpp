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
#include <exception>

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

/**
* @brief Parsing states for parsing the .mgf file. 
* Parsing each spectrum involves cycling through the three states.
*/

enum parse_state { NO_PARSE, PROPERTIES, PEAKS };

enum property {TITLE, PEPMASS, RTINSECONDS};

property inline from_string(std::string property_name) {
    if (property_name == "TITLE") return TITLE;
    else if (property_name == "PEPMASS") return PEPMASS;
    else if (property_name == "RTINSECONDS") return RTINSECONDS;
    else throw std::runtime_error("Unhandled property name!"); 
}

bool inline starts_with(const std::string& big, const std::string& small) {
    return small.length() <= big.length() && 
           equal(small.begin(), small.end(), big.begin());
}

parse_state read_property(std::string line, spectrum_t* spectrum) {
    std::string prop_delimiter = "=";
    std::string prop_name = line.substr(0, line.find(prop_delimiter)); 
    std::string prop_val = line.substr(line.find(prop_delimiter) + 1);
    parse_state final_state = PROPERTIES;
    switch (from_string(prop_name)) {
        case TITLE: 
            spectrum->title = prop_val;
            break;
        case PEPMASS: 
            spectrum->pepmass = std::stof(prop_val);
            break;
        case RTINSECONDS: 
            spectrum->rtin_seconds = std::stof(prop_val);
            final_state = PEAKS;
            break;
    }
    return final_state;
}

void print_progress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

/**
* @brief 
* @param
* @return
*/
std::vector<spectrum_t> parse_mgf_file(std::string path) {
    std::vector<spectrum_t> spectra;
    spectrum_t* cur;
    parse_state state = NO_PARSE;
    std::ifstream file_stream(path);
    std::string line;
    int count = 0;
    printf("Parsing file %s ...\n", path.c_str());
    while (std::getline(file_stream, line)) {
        if (state == NO_PARSE && starts_with(line, "BEGIN IONS")) {
            cur = new spectrum_t();
            state = PROPERTIES;
        } else if (state == PROPERTIES) {
            state = read_property(line, cur);
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
        count += line.size();
    }
    return spectra;
}

/**
* @brief 
* @param
* @return
*/
void print_spectrum(spectrum_t spectrum, bool verbose) {
    printf("Title: %s\n", spectrum.title.c_str());
    printf("Pepmass: %f\n", spectrum.pepmass);
    printf("Rtin Seconds: %f\n", spectrum.rtin_seconds);
    printf("Number of peaks: %d\n", spectrum.num_peaks);
    if (verbose) {
        for (int i=0; i<spectrum.num_peaks; i++) {
            printf("%f: %d\n", spectrum.peaks[i], spectrum.intensities[i]);
        }
    }
    printf("\n");
} 

