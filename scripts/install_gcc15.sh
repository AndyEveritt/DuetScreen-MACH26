#!/usr/bin/env bash

set -euo pipefail

GCC_VERSION="${GCC_VERSION:-15.2.0}"
INSTALL_PREFIX="${INSTALL_PREFIX:-/opt/gcc-15}"
BUILD_ROOT="${BUILD_ROOT:-/tmp/gcc15-build}"
DISTRO_ID=""
DISTRO_NAME=""

if ! command -v apt-get >/dev/null 2>&1; then
    echo "This script currently supports apt-based Linux distributions only." >&2
    exit 1
fi

if ! command -v sudo >/dev/null 2>&1; then
    echo "sudo is required to install packages." >&2
    exit 1
fi

package_available() {
    apt-cache show "$1" >/dev/null 2>&1
}

detect_distro() {
    if [[ -r /etc/os-release ]]; then
        # shellcheck disable=SC1091
        source /etc/os-release
        DISTRO_ID="${ID:-}"
        DISTRO_NAME="${PRETTY_NAME:-${NAME:-unknown}}"
    else
        DISTRO_NAME="unknown"
    fi
}

is_ubuntu_like() {
    [[ "${DISTRO_ID}" == "ubuntu" ]]
}

add_toolchain_ppa_and_update() {
    echo "GCC 15 apt package not found. Adding ubuntu-toolchain-r/test PPA and updating package lists..."
    sudo add-apt-repository --yes --update ppa:ubuntu-toolchain-r/test
}

install_from_apt() {
    echo "Installing GCC 15 and G++ 15 from apt (no recommends)..."
    if ! sudo apt-get install -y --no-install-recommends gcc-15 g++-15; then
        echo "apt-based install failed; falling back to source build."
        install_from_source
    fi
}

install_from_source() {
    echo "GCC 15 apt package still unavailable. Building GCC ${GCC_VERSION} from source."
    echo "This can take a while."

    sudo apt-get install -y \
        build-essential \
        flex \
        bison \
        wget \
        xz-utils \
        texinfo \
        libgmp-dev \
        libmpfr-dev \
        libmpc-dev \
        libisl-dev \
        zlib1g-dev

    mkdir -p "${BUILD_ROOT}"
    rm -rf "${BUILD_ROOT}/gcc-${GCC_VERSION}" "${BUILD_ROOT}/gcc-${GCC_VERSION}.tar.xz" "${BUILD_ROOT}/build"
    cd "${BUILD_ROOT}"

    wget "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"
    tar -xf "gcc-${GCC_VERSION}.tar.xz"
    mkdir -p build
    cd build

    "${BUILD_ROOT}/gcc-${GCC_VERSION}/configure" \
        --prefix="${INSTALL_PREFIX}" \
        --enable-languages=c,c++ \
        --disable-multilib

    make -j"$(nproc)"
    sudo make install

    sudo ln -sf "${INSTALL_PREFIX}/bin/gcc" /usr/local/bin/gcc-15
    sudo ln -sf "${INSTALL_PREFIX}/bin/g++" /usr/local/bin/g++-15
}

echo "Installing prerequisites for GCC 15 setup..."
detect_distro

sudo apt-get update

if is_ubuntu_like; then
    sudo apt-get install -y software-properties-common
fi

if ! command -v gcc-15 >/dev/null 2>&1 || ! command -v g++-15 >/dev/null 2>&1; then
    if package_available gcc-15; then
        install_from_apt
    else
        if is_ubuntu_like; then
            add_toolchain_ppa_and_update

            if package_available gcc-15; then
                install_from_apt
            else
                install_from_source
            fi
        else
            echo "gcc-15 apt packages are not available on ${DISTRO_NAME}; building from source instead."
            install_from_source
        fi
    fi
fi

echo "Verifying installed compilers..."
gcc-15 --version | head -n 1
g++-15 --version | head -n 1

cat <<'EOF'

GCC 15 is installed.

To configure DuetScreen with GCC 15:
  cmake --preset Simulation

To build:
  cmake --build --preset Simulation

Optional (for current shell):
  export CC=gcc-15
  export CXX=g++-15

EOF