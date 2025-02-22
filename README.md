# qase
Lightweight command line music playback program

> [!CAUTION]
> Currently qase does not work due to a mysterious error, on wsl. so qase is deprecated.

## dependencies
argp

libid3tag0

ffmpeg(libavformat)

SDL2
SDL2-image
SDL2-mixer

pkg-config

cmake

**install dependencies on ubuntu**:
```bash
sudo apt install libid3tag0 libid3tag0-dev libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev ffmpeg libavformat-dev libavutil-dev libavcodec-dev pkg-config
```

## Usage
`qase <Music file>`

## How to build
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

# Screenshots
![image](https://github.com/user-attachments/assets/85273028-7637-42a6-bbbe-756310a4b054)


# License
This software is licensed under MIT License.

Please read LICENSE file
