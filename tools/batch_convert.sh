#!/bin/bash
# Batch convert all GIFs in the gif folder

echo "=== Mochi GIF Batch Converter ==="
echo ""

# Create output directory
mkdir -p ../include/animations

# Run converter
/home/ivan/miniconda3/bin/python3 gif2bitmap.py ../gif/ \
  --batch \
  --output-dir ../include/animations \
  --threshold 128 \
  --max-frames 30

echo ""
echo "✓ All GIFs converted!"
echo "Output directory: ../include/animations/"
echo ""
echo "To use in your code:"
echo '  #include "animations/mochi_bitmap.h"'
echo ""

