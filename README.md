# Sparse Vector Clustering

## Running Instructions

```python
git clone https://github.com/razered/SparseClustering
cd SparseClustering/
mkdir data/
# add 100000.mgf to data/
make 
./cluster 
```

## Project Structure

## Background

We implement a cosine-similarity based clustering method and 2 heuristics to reduce invocations of the similarity calculation subroutine.

Similarity Calculation - Iterate through all pairs of peaks from the 2 spectra, they represent the same point if they are within 0.02 Dalton of each other. If the dot product score is > 0.7, we can cluster them.

The 2 heuristics are - 
1. We only need to cluster 2 spectra whose pepmasses are within a 2.0 Dalton  of each other.

2. Since the similarity calculation is expensive - we have a heuristic to reduce the number of calculations. We bucket the top 5 peaks of each spectra. So spectra 1 may lie in buckets 50-50.02, 75-75.02 and 3 other buckets. We now only consider all the other spectra in these 5 buckets as viable candidates for clustering.

The worst-case time complexity of the algorithm is O(N^2). The algorithm is greedy. 

The similarity calculation is single-linkage i.e. say we are trying to find spectra 1's cluster - if spectra 2 and 3 have been added and we are now comparing with spectra 4, we still use only the peaks of spectra 1 for similarity calculation.