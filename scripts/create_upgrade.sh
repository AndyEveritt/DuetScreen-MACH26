#!/usr/bin/env bash

# Create DuetScreen upgrade archives.
#
# Usage:
#   create_upgrade.sh [--skip-binary] [-h|--help]
#
# Options:
#   --skip-binary, -B   Do not include the DuetScreen binary in the archive
#   -h, --help          Show this help and exit

set -euo pipefail

SKIP_BINARY=0

usage() {
    echo "Usage: $0 [--skip-binary] [-h|--help]"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --skip-binary|-B)
            SKIP_BINARY=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage
            exit 1
            ;;
    esac
done

echo "Cleaning previous output..."
rm -rf out/update
if [ -f out/update.tar.gz ]; then
    rm "out/update.tar.gz"
fi

if [ -f out/DuetScreen.tar.gz ]; then
    rm "out/DuetScreen.tar.gz"
fi

# Copy binary (unless skipped)
if [[ "$SKIP_BINARY" -eq 0 ]]; then
    echo "Including DuetScreen binary..."
    mkdir -p out/update/usr/bin
    cp ../buildroot-duetscreen/output/target/usr/bin/DuetScreen out/update/usr/bin
    cp ../buildroot-duetscreen/output/target/etc/buildroot_version out/update/
fi

# Copy assets
echo "Including assets..."
mkdir -p out/update/etc/assets
cp -r assets out/update/etc

# Copy libraries
# mkdir -p out/update/usr/lib
# cp ../buildroot-duetscreen/output/target/usr/lib/liblvgl* out/update/usr/lib/

cp scripts/pre-update out/update/pre-update
cp scripts/post-update out/update/post-update

echo "Creating upgrade archive..."
cd out/update
tar -czf ../update.tar.gz *
cp ../update.tar.gz ../DuetScreen.tar.gz
cd -

echo "Upgrade archive created at out/update.tar.gz and out/DuetScreen.tar.gz"