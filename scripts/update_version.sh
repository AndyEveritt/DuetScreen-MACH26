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
sed -i "s/\".*\"/\"$VERSION\"/" $VERSION_FILE

if [ $? -eq 0 ]; then
    echo "Successfully updated version to $VERSION"
else
    echo "Failed to update version"
    exit 1
fi
