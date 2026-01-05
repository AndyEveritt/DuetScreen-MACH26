#!/bin/bash

# Check if version argument is provided
if [ $# -eq 0 ]; then
    VERSION=$(git describe --tags --dirty --always --match="v*")
    if [ -z "$VERSION" ]; then
        echo "Warning: Could not determine version from git tags."
        exit 0
    fi
else
    VERSION=$1
fi

VERSION_FILE="./version.h"

# Update the version in version.h
if [ ! -f $VERSION_FILE ]; then
    echo "#pragma once" > $VERSION_FILE
    echo "// This file is auto-generated. Do not edit manually." >> $VERSION_FILE
    echo "" >> $VERSION_FILE
    echo "#define FIRMWARE_VERSION \"$VERSION\"" >> $VERSION_FILE
else
    ORIGINAL_VERSION=$(grep '#define FIRMWARE_VERSION' $VERSION_FILE | awk '{print $3}' | tr -d '"')
    if [ "$ORIGINAL_VERSION" == "$VERSION" ]; then
        echo "Version is already up to date: $VERSION"
        exit 0
    fi
    sed -i "s/\".*\"/\"$VERSION\"/" $VERSION_FILE
fi


if [ $? -eq 0 ]; then
    echo "Successfully updated version to $VERSION"
else
    echo "Failed to update version"
    exit 1
fi
