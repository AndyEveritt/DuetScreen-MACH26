#!/bin/bash

# Check if version argument is provided
if [ $# -eq 0 ]; then
    echo "Error: Version argument is required"
    echo "Usage: $0 <version>"
    exit 1
fi

VERSION=$1
VERSION_FILE="./version.h"

# Update the version in version.h
if [ ! -f $VERSION_FILE ]; then
    echo "#pragma once" > $VERSION_FILE
    echo "// This file is auto-generated. Do not edit manually." >> $VERSION_FILE
    echo "" >> $VERSION_FILE
    echo "#define FIRMWARE_VERSION \"$VERSION\"" >> $VERSION_FILE
else
    sed -i "s/\".*\"/\"$VERSION\"/" $VERSION_FILE
fi


if [ $? -eq 0 ]; then
    echo "Successfully updated version to $VERSION"
else
    echo "Failed to update version"
    exit 1
fi
