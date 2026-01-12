### Install SEAL

在主目录下git clone https://github.com/microsoft/SEAL.git

### Install cryptopp

wget https://cryptopp.com/cryptopp870.zip

unzip -a cryptopp890.zip -d cryptopp

cd cryptopp

make static  -j4 

make install PREFIX=./install_x86_64

to test:

./cryptest.exe v

### Install Sux

在主目录下：https://github.com/vigna/sux.git


git lfs install
git lfs pull --include="cryptopp/**"


rm -rf build/ && cmake -S . -B build && cmake --build build -j4 && cp ./build/test_pir ./test_pir



rm -rf build/ && cmake -S . -B build && cmake --build build -j4 && cp ./build/main ./main && ./main