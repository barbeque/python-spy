set -e
g++ -g -framework Python spy-on-python.cpp -o spy
./spy
