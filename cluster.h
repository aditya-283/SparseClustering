/**
 * Clustering spectra
 * @author Aditya Bhagwat abhagwa2@cs.cmu.edu
 */

#ifndef CLUSTER_H
#define CLUSTER_H

#include "spectra.h"
#include <stdlib.h>
#include <vector>

void cluster_spectra(std::vector<int>& clusters, const std::vector<spectrum_t>& spectra);
void naive_cluster_spectra(std::vector<int>& clusters, const std::vector<spectrum_t>& spectra);

#endif
