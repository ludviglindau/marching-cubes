# marching-cubes
OpenGL implementation of marching cubes

To run: copy glew32.dll into .exe directory and make sure the working directory contains shader files.

Program does 3D noise texture generation and polygonization on the GPU.
GPU buffers are compacted after polygonization and indirect draw is used for the OpenGL draw call.

Control camera rotation with arrow keys and zoom in with ',' and out with '.' keys.

Example image of running the program on 30x30x7 chunks, each chunk consists of 33x33x33 voxels.
![alt text](https://github.com/ludviglindau/marching-cubes/blob/master/example.png)
Example image contains 6928658 triangles and took about 7 seconds to generate on my 2070 RTX card.
