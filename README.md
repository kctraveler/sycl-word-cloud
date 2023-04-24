# sycl-word-cloud
Implementation of the C++ word-cloud with SYCL and Intel oneAPI

## Running the code
- Uses oneAPI and runs on Intel Dev Cloud
- Final iteration is on the v2 target
### qsub Commands for different node types
- qsub -I -l nodes=1:gpu:ppn=2 -d .
    - GPU node
- qsub -I
    - general interactive node
    - Serial code ran  faster on GPU node than this one, GPU device slower for parallel than this one.

# Results and Observations
## Version One Notes (Target sycl-word-cloud)
- Started with Unified shared memory, very easy and saw significant speedup in the portion doing the counting
- Moved to buffers and accessors and performance got significantly slower

### Timing Results (CPU Node)
#### Original implementation with USM
| Inteval | Time |
| --- | --- |
| Tokenize file and hash | .011 seconds |
| Serial count tokens | 1.67 seconds |
| Write serial data from vector | 5.61 seconds |
| Parallel count tokens | .247 seconds |
| Write parallel results from malloc | 1.650 seconds |

## Version Two Notes (Target v2)
- Added windowed results for both seq and parallel
- Sub buffers to divide work of input by window range
- 2D buffer for the output where outer dimension is the window and inner is the count vector
- Had troulble using sub buffer of 2D to get a single dimension. 
    - Eliminated sub buffer and did global accessor. 
    - Reading sub buffer allows multiple queues to work on the buffer at the same time
- Probably should make the use of a map on a filtered output on both

### Timing Results (CPU Node)
| Interval | Time (Seconds) |
| --- | --- |
| Tokenize File | .009 |
| Sequential Windowed* Count | 20.07 |
| Parallel Windowed* Count | 0.245 |
\* WINDOW_SIZE = 10,000

### Questions unanswered
- Did it ever need subbuffers or could we have partitioned the range someother way. 
- Could we have tried to do the input the same as the output. Would either option limit the ability for simultaneous kernals to process each window. 
- Is their a chance that we are doing any simultaneous processing in current setup or is the q.submit blocking the outer loop. Probably is I would think. 

## ToDo:
#### Functionality
- [ ] Decode hashes
- [x] CLI (File path, max kernels)
- [x] Identify further optimizations. Particularly result output
    - Working find with large output arrays with 4 windows at least
- [ ] Update verision 2 seq to remove the map creation or add map creation to par
#### Data Analysis
- [ ] Investigate Profiling tools (VTune)