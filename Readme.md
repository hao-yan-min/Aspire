# Aspire: Zero-Leakage Private Key-Value Storage Based on PIR



Aspire is a research library and should not be used in production systems.

This repository contains the implement of PIRANA-based Aspire, Spiral-based Aspire and PathORAM-based Aspire proposed in the paper.

## Dependencies

- Microsoft SEAL (version>=4.0.0)

- Microsoft Kuku (version=2.1.0)

- OpenSSL (version >=3.0.2)
- cryptopp (version=8.7.0)
- hexl (version=1.2.6)
- Sux (version=1.0.3)

[Microsoft SEAL](https://github.com/microsoft/SEAL) and [Microsoft Kuku](https://github.com/microsoft/Kuku) can be installed using the instructions outlined in the repository.

## Building the project

The project can be built using the following command.


* Install Microsoft SEAL

  ```bash
  git clone https://github.com/microsoft/SEAL.git
  cd ./SEAL && cmake -S . -B build -DCMAKE_INSTALL_PREFIX=~/local && cmake --build build && cmake --install build
  ```

* Install HEXL library

  ```bash
  git clone https://github.com/IntelLabs/hexl.git
  cd ./hexl
  cmake -S . -B build -DCMAKE_INSTALL_PREFIX=~/local && cmake --build build && cmake --install build
  ```

* Install cryptopp

  ```bash
  git clone --branch CRYPTOPP_8_7_0 --depth 1 https://github.com/weidai11/cryptopp.git
  
  cd cryptopp
  
  make static  -j4 
  
  make install PREFIX=./install_x86_64
  ```

  **to test:**

  ```bash
  ./cryptest.exe v
  ```

* Install Microsoft Kuku

  ```bash
  git clone https://github.com/microsoft/Kuku.git
  cd Kuku
  cmake -S . -B build -DCMAKE_INSTALL_PREFIX=~/local && cmake --build 	build &&cmake --install build
  ```

* run

  We can run the Aspire scheme on a database with $2^{18}$ entries and a payload size of 256 bytes using the following command.

  ```bash
  python3 compile.py
  ```

​	Then run the benchmark. The command usage is as follows:

  ```bash
  # Usage syntax:
  # ./build/main <bucket_num> <query_num> <log2(N)> <key_bits> <val_bits> <dim0> <dim1> <threads> <pir_type>
  # Test with PathORAM-based Aspire
  # log2(N)=18, Val=2048 bits, Threads=32
  ./build/main 512 341 18 32 2048 6 2 32 ORAM

  # Test with PIRANA-based Aspire
  ./build/main 512 341 18 32 2048 6 2 32 PIRANA

  # Test with Spiral-based Aspire
  ./build/main 512 341 18 32 2048 6 2 32 SpiralPIR
  ```

  *Note:* dim0 and dim1 are parameters for the Spiral scheme; please do not modify them arbitrarily.

* fully test

  To run more comprehensive tests, you can execute:

  ```bash
  python3 run.py --type PIRANA --threads 16 --testcase 0
  ```

  ## Parameters

  |  Option  |                         Description                          |   Default    |
  | :------: | :----------------------------------------------------------: | :----------: |
  |   type   | The PIR/ORAM scheme used by Aspire; options include *ORAM*, *SpiralPIR*, and *PIRANA* （Running the SpiralPIR requires at least 1 TB of RAM.） |     ORAM     |
  | threads  |             Number of threads used for testing.              |      16      |
  | testcase | Test case to run: 1 for different element numbers, 2 for different value length, 3 for different bucket numbers, 0 for all tests. | 0 (test all) |
