# L1_cache
This project involves the modeling and performance analysis of a single-level cache.

It implements interactive input, allowing the user to choose between two replacement policies: LFU (Least Frequently Used) / LRU (Least Recently Used), and two write policies: WBWA (Write-Back with Write-Allocate) / WTNA (Write-Through with No-Write-Allocate), while specifying a trace file as input.

The cache functionality is simulated, and the final cache content is outputted, along with the number of read misses, write misses, etc., to calculate cache performance.

## File structure
src: Contains the initial framework source code and Makefile.

trace: Contains all the trace files required for the experiment, i.e., read/write memory access streams.

validation: Contains all the files for simulation result verification.

debug: Contains all the simulator debugging files.

Proj1-1_Specification: The experiment guide.

## Operating Environment
32bit，x86 OS with gcc
