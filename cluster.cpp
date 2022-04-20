/**
 * @file spectra.cpp
 * @brief Algorithms to cluster spectral data efficiently
 * 
 * We implement bottom-up agglomerative clustering of spectra using cosine similarity. 
 * We expose two methods - one that does this naively and the other that employs two approximation heuristics to speed up the process.
 * The first heuristic is that we only attempt clustering two spectra if their pepmasses are close to each other. 
 * The second heuristic is that we only cluster spectra if they share some top peaks.
 * 
 * @author Aditya Bhagwat abhagwa2@cs.cmu.edu
 */

#include "spectra.cpp"
#include "cluster.h"
#include <unordered_set>
#include <unordered_map>
#include <set> 
#include <math.h>
#include <chrono>
#include <iostream>
#include <algorithm>

#define DEFAULT_PEPMASS_BIN 2.f
#define DEFAULT_PEAK_BIN 0.02f
#define DEFAULT_SIMILARITY_THRESHOLD 0.7f

static int _argc;
static const char **_argv;
static float peak_bin;
static float pepmass_bin;
static float similarity_threshold;
/**
* @brief Implements the pepmass heuristic - returns true if thetwo spectra that have close enough pepmasses
*/
bool inline passes_pepmass_test(const spectrum_t& a, const spectrum_t& b) {
    return fabs(a.pepmass - b.pepmass) < pepmass_bin; 
}

/**
* @brief Checks if 2 peaks represent the same thing by checking if they are close enough
*/
bool inline is_identical_peak(const peak_t& a, const peak_t& b) {
    return fabs(a - b) <  peak_bin;
}

/**
* @brief Computes the cosine similarity of two spectra
*/
float cosine_similarity(const spectrum_t& a, const spectrum_t& b) {
    int i=0, j=0;
    float score=0;
    float a_den=0, b_den=0;
    while (i < a.num_peaks && j < b.num_peaks) {
        if (is_identical_peak(a.peaks[i], b.peaks[j])) {
            score += (a.intensities[i] * b.intensities[j]);
            a_den += pow(a.intensities[i], 2);
            b_den += pow(b.intensities[j], 2);
            i += 1;
            j += 1;
        } else if (a.peaks[i] < b.peaks[j]) {
            a_den += pow(a.intensities[i], 2);
            i += 1;
        }
        else if (a.peaks[i] > b.peaks[j]) {
            b_den += pow(b.intensities[j], 2);
            j += 1;
        }
    }
    return score / sqrt(a_den * b_den);
}

/**
* @brief Checks if the cosine similarity of two spectra is large enough to cluster them together
*/
bool inline is_similar(const spectrum_t& a, const spectrum_t& b) {
    return cosine_similarity(a, b) > similarity_threshold;
}

/**
* @brief Initialise a vector which denotes the cluster to which each spectra belongs.
* @param sz Size of the `clusters` (same as size of `spectra`)
* @return initialized `clusters` vector
*/
std::vector<int> initialize_cluster(int sz) {
    std::vector<int> clusters(sz, 0);
    for (int i=0; i<sz; i++) {
        clusters[i] = i;
    }
    return clusters;
}


/**
* @brief Finds the start point of the peak_bucket. e.g. 50.01 lies in (50.00, 50.02) so this should return (50.00)
* @param peak the peak to be bucketed
* @return string representation of peak bucket so that it can be hashed.
*/
std::string get_peak_bucket(peak_t peak) {
	char buffer[10];
	float result =  floorf(peak / peak_bin) * peak_bin;
	snprintf(buffer, 10, "%.*f", 3, result);
	return buffer;
}

/**
* @brief Optimized algorithm that clusters spectra by using both the top 5 peaks and pepmass heuristics.
* @param clusters a vector that denotes the cluster to which each spectra belongs to
* @param spectra a vector of spectra
* @return nothing
*/
std::vector<int> get_common_peak_candidates(const spectrum_t& spectrum, std::unordered_map<std::string, std::vector<int>>& peak_buckets) {
    std::vector<int> candidates;
    int sz = std::min(5, (int)spectrum.num_peaks);
    for (int i=0; i<sz; i++) {
        std::string peak_str = get_peak_bucket(spectrum.peaks[i]);
        if (peak_buckets.find(peak_str) != peak_buckets.end()) {
            candidates.insert(candidates.end(), peak_buckets[peak_str].begin(), peak_buckets[peak_str].end());
        } 
    }

    std::set<int> unq(candidates.begin(), candidates.end());
    candidates.assign(unq.begin(), unq.end());
    return candidates;
}

/**
* @brief Iterate over the top 5 peaks of a newly added cluster and add them to the peak_buckets data structure
* @param peak_buckets a data structure that stores a map of a peak-range with a vector of clusters
                      (their representatives) whose peaks like in that range. The range is a floating point range of size 0.02. 
                      It is represented by the string representation of the left limit. 
                      Range (50.00, 50.02) is represented as "50.00". We choose strings so that we don't have to deal with
                      precision issues and because strings hash easily. `peak_buckets` is modified in-place.
* @param spectra the vector that stores all spectra
* @param idx the index inside the above vector
* 
*/
void bucket_spectrum_peaks(std::unordered_map<std::string, std::vector<int>>& peak_buckets, const spectrum_t& spectrum, int idx) {
    int sz = std::min(5, (int)spectrum.num_peaks);
    for (int i=0; i<sz; i++) {
        std::string peak = get_peak_bucket(spectrum.peaks[i]);
        peak_buckets[peak].push_back(idx);
    }
}

void dbg_print_buckets(std::unordered_map<std::string, std::vector<int>>& peak_buckets) {
    for (auto& it: peak_buckets) {
        printf("key : %s\t", it.first.c_str());
        for (int v: it.second)
            printf("%d ", v);
        printf("\n");
    }
}

/**
* @brief Optimized algorithm that clusters spectra by using both the top 5 peaks and pepmass heuristics.
* @param clusters a vector that denotes the cluster to which each spectra belongs to
* @param spectra a vector of spectra
*/
void cluster_spectra(std::vector<int>& clusters, const std::vector<spectrum_t>& spectra) {
    std::unordered_map<std::string, std::vector<int>> peak_buckets;
    for (int i=0; i<spectra.size(); i++) {
        if (!(i % (spectra.size()/100))) print_progress((float)i/spectra.size());
        bool new_cluster = true;
        std::vector<int> candidates = get_common_peak_candidates(spectra[i], peak_buckets);
        for (const int& candidate: candidates) {
            if (passes_pepmass_test(spectra[i], spectra[candidate]) && is_similar(spectra[i], spectra[candidate])) {
                clusters[i] = candidate;
                new_cluster = false;
                break;
            } 
        }

        if (new_cluster) bucket_spectrum_peaks(peak_buckets, spectra[i], i);
    }
    print_progress(1.0f);
}

/**
* @brief Debug method to print clusters and their sizes
*/
void print_clusters(const std::vector<int>& clusters) {
    std::unordered_map<int, std::vector<int>> map;
    for (int i=0; i<clusters.size(); i++) 
        map[clusters[i]].push_back(i);

    for (auto& it: map) 
        printf("Cluster with %d has size %lu\n", it.first, it.second.size());
    
}


/**
* @brief Cluster spectra using the naive O(N^2) greedy agglomerative clustering algorithm.
* The algorithm includes the pepmass heuristic
* @param clusters a vector that denotes the cluster to which each spectra belongs to
* @param spectra a vector of spectra
*/
void naive_cluster_spectra(std::vector<int>& clusters, const std::vector<spectrum_t>& spectra) {
    for (int i=1; i<spectra.size(); i++) {
        std::unordered_set<int> seen_candidates;
        for (int j=0; j<i; j++) {
            int candidate = clusters[j];
            if (seen_candidates.find(candidate) != seen_candidates.end()) continue;
            if (passes_pepmass_test(spectra[i], spectra[candidate]) && 
                is_similar(spectra[i], spectra[candidate])) {
                clusters[i] = candidate;
                break;
            } else {
                seen_candidates.insert(candidate);
            }
        }
    }
}


const char *get_option_string(const char *option_name, const char *default_value) {
    for (int i = _argc - 2; i >= 0; i -= 2)
        if (strcmp(_argv[i], option_name) == 0)
            return _argv[i + 1];
    return default_value;
}

float get_option_float(const char *option_name, float default_value) {
    for (int i = _argc - 2; i >= 0; i -= 2)
        if (strcmp(_argv[i], option_name) == 0)
            return (float)atof(_argv[i + 1]);
    return default_value;
}

static void show_help(const char *program_path) {
    printf("Usage: %s OPTIONS\n", program_path);
    printf("\n");
    printf("OPTIONS:\n");
    printf("\t-f <input_filename> (required)\n");
    printf("\t-m <pepmass_bin> (default: 2.0)\n");
    printf("\t-p <peak_bin>  (default: 0.02)\n");
    printf("\t-t <similarity_threshold> (default: 0.7)\n");
}

/**
* @brief Parses .mgf file and clusters the spectra using our optimized algorithm
* @return 0 on success
*/
int main(int argc, const char *argv[]) {
    using namespace std::chrono;
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<double> dsec;

    _argc = argc - 1;
    _argv = argv + 1;
    const char *file_path = get_option_string("-f", NULL);
    pepmass_bin = get_option_float("-m", DEFAULT_PEPMASS_BIN);
    peak_bin = get_option_float("-p", DEFAULT_PEAK_BIN);
    similarity_threshold = get_option_float("-t", DEFAULT_SIMILARITY_THRESHOLD);
    
    if (file_path == NULL) {
        show_help(argv[0]);
        return -1;
    }

    auto init_start = Clock::now();
    std::vector<spectrum_t> spectra = parse_mgf_file(file_path);
    int sz = spectra.size();
    auto parsing_complete = Clock::now();
    printf("Reading the file took %lf seconds in total\n", duration_cast<dsec>(parsing_complete - init_start).count());
    printf("Using parameters pepmass_bin=%.2f peak_bin=%.3f and similarity_threshold=%.2f ...\n", pepmass_bin, peak_bin, similarity_threshold);
    printf("Clustering %lu spectra ...\n", spectra.size());
    std::vector<int> clusters = initialize_cluster(sz); // stores the representative for the cluster that the ith spectrum belongs to
    cluster_spectra(clusters, spectra);
    auto clustering_complete = Clock::now();
    printf("\nClustering took %lf seconds\n", duration_cast<dsec>(clustering_complete - parsing_complete).count());

    auto num_clusters = std::set<int>(clusters.begin(), clusters.end()).size();
    printf("The %lu spectra could be clustered into %lu clusters\n", spectra.size(), num_clusters);

    return 0;
}