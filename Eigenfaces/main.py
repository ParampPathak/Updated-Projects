import os

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import ImageGrid
from PIL import Image
data_dir = r"C:/User/FilePath"

TARGET_H = 112
TARGET_W = 92

file_paths = []
for root, dirs, files in os.walk(data_dir):
    for f in files:
        if f.endswith(".pgm"):
            file_paths.append(os.path.join(root, f))

images_list = []
for fp in file_paths:
    img = Image.open(fp)
    img = img.resize((TARGET_W, TARGET_H))
    images_list.append(np.asarray(img))

images = np.array(images_list)
print("Loaded images:", images.shape)
fig = plt.figure(figsize=(8, 8))
grid = ImageGrid(fig, 111, nrows_ncols=(8, 8), axes_pad=0)

for ax, im in zip(grid, images[:64]):
    ax.imshow(im, cmap='gray')
    ax.axis('off')
face_mean = np.mean(images, axis=0)

fig = plt.figure()
ax = fig.add_subplot(111)
ax.imshow(face_mean, cmap='gray')
ax.axis('off')
plt.title("Mean Face")

num_images, height, width = images.shape

def compute_pca(data, data_mean):

   
data_centered = data - data_mean

   
X = data_centered.reshape(num_images, height * width)

   
U, S, Vt = np.linalg.svd(X, full_matrices=False)
eigenfaces = Vt      

   
weights = X @ eigenfaces.T    

return eigenfaces, weights

eigenfaces, weights = compute_pca(images, face_mean)

fig = plt.figure(figsize=(8, 8))
grid = ImageGrid(fig, 111, nrows_ncols=(4, 4), axes_pad=0)

for ax, ef in zip(grid, eigenfaces[:16]):
    ax.imshow(ef.reshape(height, width), cmap='gray')
    ax.axis('off')

def reconstruct(weights, eigenfaces, X_mean, img_size, img_idx, n_comps=400):
    h, w = img_size

    W = eigenfaces[:n_comps]
    coeffs = weights[img_idx, :n_comps]

    recon = X_mean + coeffs @ W

    return recon.reshape(h, w)

img_idx = 200
n_comps = 400

recovered_img = reconstruct(
    weights,
    eigenfaces,
    face_mean.reshape(-1),
    [height, width],
    img_idx,
    n_comps
)

fig = plt.figure(figsize=(6, 3))
ax1 = fig.add_subplot(121)
ax2 = fig.add_subplot(122)

ax1.imshow(images[img_idx], cmap='gray')
ax1.axis('off')
ax1.set_title("Original")

ax2.imshow(recovered_img, cmap='gray')
ax2.axis('off')
ax2.set_title(f"Reconstruction ({n_comps} comps)")
