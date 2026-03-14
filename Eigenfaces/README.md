# Eigenfaces — Linear Algebra Assignment

This project implements the **Eigenfaces** technique using **PCA computed with SVD**. Face images are loaded, resized, and processed to compute eigenfaces and reconstruct a face using a reduced number of components.

## Overview
- Loads `.pgm` face images from a directory
- Resizes images to **92 × 112**
- Computes the **mean face**
- Performs **PCA via SVD** to obtain eigenfaces
- Reconstructs a selected face using a specified number of components

## Requirements
- Python 3
- NumPy
- Matplotlib
- Pillow

```bash

