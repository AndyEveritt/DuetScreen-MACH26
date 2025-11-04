sudo apt-get update &&
sudo apt-get install -y \
    build-essential \
    libsdl2-dev \
    cmake \
    ninja-build \
    libusb-1.0-0-dev \
    pkg-config \
    clang-format-19 \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libavutil-dev \
    libpng-dev \
    libspdlog-dev \
    gdb-multiarch \
    libgtest-dev \
    imagemagick \
    python3-tk \
    gcovr \
    libfreetype6 \
    libfreetype-dev \
    python3 \
    python3-dev \
    python3-pip \
    python3-venv

python3 -m venv env
source env/bin/activate
pip install -r requirements.txt
