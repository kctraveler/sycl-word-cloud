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
- [ ] Decode hashes
- [ ] CLI
- [ ] Parallel map creation?

# Results and Observations
### General Notes
- Parallel seems way faster, however so does creating the map and writing to a file
    - could this observation be the result of using a malloc array as opposed to vector?
    - still have to copy from vector to malloc array. That step is included in the parallel timer. Interesting how it doesn't slow things down


### Timing Results
| Inteval | Time |
| --- | --- |
| Tokenize file and hash | .011 seconds |
| Serial count tokens | 1.67 seconds |
| Write serial data from vector | 5.61 seconds |
| Parallel count tokens | .247 seconds |
| Write parallel results from malloc | 1.650 seconds |
