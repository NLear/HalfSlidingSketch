# Half Sliding Sketch (HSS)

This repository is dedicated to the codebase supporting our manuscript `HSS: A Memory-Efficient, Accurate, and Fast Network Measurement Framework in Sliding Windows`, currently under review at TNSM. The `master` branch hosts our prototype of the Half Sliding Sketch (HSS), along with the benchmark and dataset utilized in the manuscript. Our HSS solution is designed to network measurements within sliding windows by emphasizing memory efficiency, accuracy, and processing speed. The repository borrows from the implementation of the [Sliding-Sketch](https://github.com/sliding-sketch/Sliding-Sketch), enhancing it with our unique contributions. Please explore this space for a practical understanding of our research and feel free to delve into the intricacies of our proposed HSS framework.

## Introduction

Network measurement is indispensable to network management. This paper focuses on three fundamental network measurement tasks: membership query, frequency query, and heavy hitter query. Existing solutions, such as sketches, sliding window algorithms, and the Sliding Sketch [1] framework, struggle to simultaneously achieve memory efficiency, accuracy, real-time operation, and generic application.

Accordingly, this paper proposes the Half Sliding Sketch (HSS), an improvement over the state-of-the-art Sliding Sketch framework. The HSS framework is applied to five contemporary sketches for the three aforementioned query tasks.
Theoretical analysis reveals that our framework is faster, more memory-efficient and more accurate than the state-of-the-art Sliding Sketch while still being generic.
Extensive experimental results reveal that HSS significantly enhances the accuracy for the three query tasks, achieving improvements of 2x to 28.7x, 1.5x to 9x, and 2.4x to 3.6x, respectively. Moreover, in terms of speed, HSS is 1.2x to 1.5x faster than the Sliding Sketch.

## Repository Overview

This repository serves as a comprehensive resource for all the algorithms deployed in our experimental study. A snapshot of the implemented tasks and their corresponding algorithms is detailed in the table below.

| Task              | Algorithms                                 |
| ----------------- | ------------------------------------------ |
| Membership query  | HSS-BF, SS-BF, FBF, SW-BF                  |
| Frequency query   | (H)SS-CM, (H)SS-CU, (H)SS-Count, ECM, SWCM |
| HeavyHitter query | HSS-HK, SS-HK, lambda-Algorithm, WCSS      |

Additionally, this repository includes a basic demonstration illustrating the application of these algorithms on a compact dataset.

The repository is structured as follows:

  - `data`: This directory houses the trace data for testing, where each 8-byte trace corresponds to an item.
  - `src`
    - `Membership`: This directory contains the Membership query algorithms (Sl-BF,FBF, SW-BF).
    - `Frequency`: This directory encompasses the Frequency query algorithms (Sl-CM,Cl-CU,Sl-Count,ECM,SWCM).
    - `HeavyHitter`: This directory is home to the HeavyHitter query algorithms (Sl-HK,lambda-Algorithm,WCSS).
  - `src/.*/demo/main.cpp`: This directory contains demonstrations that serve as practical guides for the utilization of these algorithms.

## Building and Running the Code

### System Requirements

The algorithms and demonstrations included in this repository have been tested on Ubuntu 22.04.2 LTS (GNU/Linux 5.19.0-42-generic x86_64) environment.

Software prerequisites for the successful operation of the provided code include:

- cmake
- g++

Please ensure that your system is equipped with these applications before proceeding with the use of this repository's content.

### Building the Project

Ensure that you have the required software (`cmake` and `g++`) installed on your system. In your terminal, navigate to the root directory of this project and run the following commands:

```
mkdir build
cd build
cmake ..
make
```

These commands will create a `build` directory, generate the necessary Makefiles using `cmake`, and then compile the project with `make`.

### Running the Demos

After you've successfully built the project, you can execute the provided demos. You will find these in the `build` directory of each algorithm's name (e.g., `build/HSS_HK`). There, you will see the compiled executable file. To run the demos, use the following command:

```
./HSS_HK
```



## Contact and Support

We encourage open collaboration and welcome any questions or suggestions regarding the repository. If you have any issues or need further assistance, please feel free to raise an issue in this repository or contact us directly.

## License

This project is licensed under the terms of the MIT license.

## References

`[1] Xiangyang Gou, Long He, Yinda Zhang, Ke Wang, Xilai Liu, Tong Yang, Yi Wang, and Bin Cui. 2020. Sliding Sketches: A Framework using Time Zones for Data Stream Processing in Sliding Windows. In Proceedings of the 26th ACM SIGKDD International Conference on Knowledge Discovery &amp; Data Mining (KDD '20). Association for Computing Machinery, New York, NY, USA, 1015–1025. https://doi.org/10.1145/3394486.3403144`