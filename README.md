# sycl-word-cloud
Implementation of the C++ word-cloud with SYCL and Intel oneAPI

## Working Notes:
- Simple example [oneAPI Simple Add](https://github.com/oneapi-src/oneAPI-samples/tree/master/DirectProgramming/C%2B%2BSYCL/DenseLinearAlgebra/simple-add)
- Previous repository for serial word cloud [kctraveler/caesar_cipher](https://github.com/kctraveler/caesar_cipher)

### qsub Commands for reference
- qsub -I -l nodes=1:gpu:ppn=2 -d .
    - GPU node
- qsub -I
    - general interactive node
    - Serial code ran  faster on GPU node than this one, GPU device slower for parallel than this one.

## ToDo:
#### Functionality
- [ ] Decode hashes
- [ ] CLI (File path, max kernels)
- [ ] Identify further optimizations. Particularly result output
    - Look at different methods in DPL etc to filter max results
#### Data Analysis
- [ ] Investigate Profiling tools (VTune)
- [ ] Parmater sweep max kernels and plot speedup
    - env variable export DPCPP_CPU_NUM_CUS={N} can be used. 
- [ ] Increase data size and plot speedup.
    - Does GPU performance exceed CPU if counting the words in 100 copies of hamlet? 
    - Assume even with USM there is a penalty for data offload to GPU. Will increase in data help or will it just increase transfers. Maybe our task is not a good match for a GPU.


# Results and Observations
### General Notes
- Parallel seems way faster, however so does creating the map and writing to a file
    - could this observation be the result of using a malloc array as opposed to vector?
    - still have to copy from vector to malloc array. That step is included in the parallel timer. Interesting how it doesn't slow things down
    - setting max kernels may allow us to observe how much of the speedup is coming from a potentially more performant data structure/access.


### Timing Results (qSub -I basic node)
| Inteval | Time |
| --- | --- |
| Tokenize file and hash | .011 seconds |
| Serial count tokens | 1.67 seconds |
| Write serial data from vector | 5.61 seconds |
| Parallel count tokens | .247 seconds |
| Write parallel results from malloc | 1.650 seconds |
