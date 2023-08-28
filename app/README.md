# Installation

Install steps on WSL / ubuntu:

```bash
sudo apt install freeglut3-dev

mkdir build && cd build

# Might need to call this twice due to some weird dependency ordering error
cmake .. && make

# Run program
../bin/CMP143
```
