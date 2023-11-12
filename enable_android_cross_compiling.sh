#!/bin/sh

if [ -z "$NDKROOT" ]; then
    echo "Cannot find NDKROOT environment variable."
    exit -1
fi

mkdir -p .vscode
cp -f resources/Android/.vscode/settings.json .vscode/settings.json