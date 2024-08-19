#include <iostream>
#include <stdio.h>
#include "game_lib/library.h"

__global__ void set_pixel(unsigned char *pixel, int r, int g, int b, int width)
{
    int x  = threadIdx.x + blockDim.x * blockIdx.x;
    int y = threadIdx.y + blockDim.y * blockIdx.y;

    int pxl_index = (x + width * y) * 4;
    pixel[pxl_index] = r;
    pxl_index++;
    pixel[pxl_index] = g;
    pxl_index++;
    pixel[pxl_index] = b;
    pxl_index++;
    pixel[pxl_index] = 255;
}

__global__ void set_pixel_offset(unsigned char *pixel, int r, int g, int b, int width, int offset_x, int offset_y)
{
    int x  = blockIdx.x * blockDim.x + threadIdx.x + offset_x;
    int y = blockIdx.y * blockDim.y + threadIdx.y + offset_y;

    //printf("%i, %i\n", offset_x, offset_y);
    int pxl_index = (x + width * y) * 4;
    pixel[pxl_index] = r;
    pxl_index++;
    pixel[pxl_index] = g;
    pxl_index++;
    pixel[pxl_index] = b;
    pxl_index++;
    pixel[pxl_index] = 255;
}



extern "C" void prepare_pixels(unsigned char *pxl, int width, int height)
{
    unsigned char *pxl2;
    cudaMalloc(&pxl2, ((width * height) * 4) * sizeof(unsigned char));
    cudaMemcpy(pxl2, pxl, ((width * height) * 4), cudaMemcpyHostToDevice);

    set_pixel<<<height, width>>>(pxl2, 0, 0, 0, 2);

    cudaDeviceSynchronize();
    cudaMemcpy(pxl, pxl2, ((width * height) * 4), cudaMemcpyDeviceToHost);
    cudaFree(pxl2);
}

extern "C" void render_shapes(unsigned char *pxl, int width, int height, geometrical_4axis *shapes, int shapes_size)
{
    unsigned char *pxl2;
    cudaMalloc(&pxl2, ((width * height) * 4) * sizeof(unsigned char));
    cudaMemcpy(pxl2, pxl, ((width * height) * 4), cudaMemcpyHostToDevice);

    for (int i = 0; i < shapes_size; i++)
    {
        int width_shape = shapes[i].x_end - shapes[i].x_start;
        int height_shape = shapes[i].y_end - shapes[i].y_start;
        color_alpha col = shapes[i].color;
        const dim3 blockSize(8, 8);
        const dim3 gridSize(width_shape/blockSize.x, height_shape/blockSize.y); 
        set_pixel_offset<<<blockSize, gridSize>>>(pxl2, col.r, col.g, col.b, width, shapes[i].x_start, shapes[i].y_start);
        cudaDeviceSynchronize();
    }
    cudaMemcpy(pxl, pxl2, ((width * height) * 4), cudaMemcpyDeviceToHost);
    cudaFree(pxl2);
}

extern "C" int number_dev()
{
    int ret;
    cudaGetDeviceCount(&ret);

    return ret;
}