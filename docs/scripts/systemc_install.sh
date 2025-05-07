# Somente Linux
# Tem q rodar com sudo (ou executar como admin se tiver numa GUI)
wget https://github.com/accellera-official/systemc/archive/refs/tags/3.0.1.tar.gz
tar -xvzf 3.0.1.tar.gz
cd systemc-3.0.1/
cmake -B build
cd build
make
make check
sudo make install
cd ../../
rm -rf systemc-3.0.1/ 
rm -rf 3.0.1.tar.gz