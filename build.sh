#!/bin/bash
# RetroStation Build Script

echo "🎮 Building RetroStation..."

# Compile
g++ -o retrostation \
    src/retrostation.cpp \
    -Ilib \
    -Isrc \
    -lm \
    -O2 \
    -std=c++11 \
    -Wall

if [ $? -eq 0 ]; then
    echo "✓ კომპილაცია წარმატებული!"
    echo "📦 Binary: ./retrostation"
    ls -lh retrostation
    echo ""
    echo "გაშვება: sudo ./retrostation"
else
    echo "❌ კომპილაცია ვერ მოხერხდა"
    exit 1
fi
