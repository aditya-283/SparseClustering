# Sparse Vector Clustering

## Running Instructions

```python
git clone https://github.com/razered/SparseClustering
cd SparseClustering/
# Add .mgf data files to the folder
make 
```

```
Usage: ./cluster OPTIONS

OPTIONS:
        -f <input_filename> (required)
        -m <pepmass_bin> (default: 2.0)
        -p <peak_bin>  (default: 0.02)
        -t <similarity_threshold> (default: 0.7)
```

## Project Structure
```
SparseClustering/
    - cluster.cpp   Cluster spectra
    - cluster.h     
    - spectra.cpp   Read from file and print spectra
    - spectra.h     
```

## Background

We implement a cosine-similarity based clustering method and 2 heuristics to reduce invocations of the similarity calculation subroutine.

Similarity Calculation - Iterate through all pairs of peaks from the 2 spectra, they represent the same point if they are within $PEAK_BIN Dalton of each other. If the dot product score is greater than $SIMILARITY_THRESHOLD, we can cluster them.

The 2 heuristics are - 
1. We only need to cluster 2 spectra whose pepmasses are within a $PEPMASS_BIN dalton  of each other.

2. Since the similarity calculation is expensive - we have a heuristic to reduce the number of calculations. We bucket the top 5 peaks of each spectra. We now only consider all the other spectra in these 5 buckets as viable candidates for clustering.

The worst-case time complexity of the algorithm is O(N^2). The algorithm is greedy. 

The similarity calculation is single-linkage i.e. say we are trying to find spectra 1's cluster - if spectra 2 and 3 have been added and we are now comparing with spectra 4, we still use only the peaks of spectra 1 for similarity calculation.