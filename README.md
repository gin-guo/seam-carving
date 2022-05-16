# seam-carving

Seam Carving is a content-aware image resizing technique that uses both geometric constraints and image content for the reduction and 
expansion of images. A _seam_ is defined as an optimal path of pixels on a single image from top to bottom, or left to right, 
where optimally is defined by an image energy function. Unlike standard content-agnostic resizing techniques such as cropping and scaling,
seam carving preserves the most significant features of the images (aspect ratio, set of objects present, etc.), and enhances the quality 
of the results. An example of a resized 505-by-287 pixel image is shown below, with a comparison between the original images and the result 
after removing 150 vertical seams, resulting in a 30% narrower image.

![resized-image](https://user-images.githubusercontent.com/72530527/168497669-c6b0c279-cfd6-49d4-b1b2-ee2b7161a455.gif)


This project uses the seam carving technique to create a data type that resizes a H-by-W image. This will be done by 1) computing the 
dual-gradient energy function, then 2) finding the vertical "seams"such that the sum of the dual-gradient energy values in the pixels along 
the path is as small as possible. The file `seamcarving.c` was created that implements all the functions in `seamcarvng.h`.


## Part 1: Dual-Gradient Energy Function

This part describes the function `void calc_energy(struct rgb_img *im, struct rgb_img **grad);`, which computes the dual-gradient energy 
function, and places it in the `struct rgb_img` `*grad`.

The energy of a pixel <img src="https://latex.codecogs.com/svg.image?(x,&space;y)" title="https://latex.codecogs.com/svg.image?(x, y)" />
is <img src="https://latex.codecogs.com/svg.image?\sqrt{\Delta_{x}^{2}(y,&space;x)&space;&plus;&space;\Delta_{y}^{2}(y,&space;x)}" title="https://latex.codecogs.com/svg.image?\sqrt{\Delta_{x}^{2}(y, x) + \Delta_{y}^{2}(y, x)}" />
. Where,

<img src="https://latex.codecogs.com/svg.image?\Delta_{x}^{2}&space;=&space;R_{x}(y,&space;x)^{2}&space;&plus;&space;G_{x}(y,&space;x)^{2}&space;&plus;&space;B_{x}(y,&space;x)^{2}" title="https://latex.codecogs.com/svg.image?\Delta_{x}^{2} = R_{x}(y, x)^{2} + G_{x}(y, x)^{2} + B_{x}(y, x)^{2}" />
<img src="https://latex.codecogs.com/svg.image?\Delta_{y}^{2}&space;=&space;R_{y}(y,&space;x)^{2}&space;&plus;&space;G_{y}(y,&space;x)^{2}&space;&plus;&space;B_{y}(y,&space;x)^{2}" title="https://latex.codecogs.com/svg.image?\Delta_{y}^{2} = R_{y}(y, x)^{2} + G_{y}(y, x)^{2} + B_{y}(y, x)^{2}" />

*_Note that <img src="https://latex.codecogs.com/svg.image?R_{x},&space;G_{x},&space;...,&space;B_{y}" title="https://latex.codecogs.com/svg.image?R_{x}, G_{x}, ..., B_{y}" />
are the differences in the red, green, and blue components of pixels surrounding 
the central pixel, along the x and y-axis._

For example, in the 3-by-4 image below showing RGB values (and in the Github `seam-carving` folder as `3x4.bin`), for the pixel (2, 1):

<img src="https://latex.codecogs.com/svg.image?R_{x}(2,1)&space;=&space;255&space;-&space;255&space;=&space;0" title="https://latex.codecogs.com/svg.image?R_{x} = 225 - 255 = 0" />
<img src="https://latex.codecogs.com/svg.image?G_{x}(2,1)&space;=&space;205&space;-&space;203&space;=&space;2" title="https://latex.codecogs.com/svg.image?R_{x} = 205 - 203 = 2" />
<img src="https://latex.codecogs.com/svg.image?B_{x}(2,1)&space;=&space;255&space;-&space;51&space;=&space;204" title="https://latex.codecogs.com/svg.image?R_{x} = 255 - 51 = 204" />
<img src="https://latex.codecogs.com/svg.image?R_{y}(2,1)&space;=&space;255&space;-&space;255&space;=&space;0" title="https://latex.codecogs.com/svg.image?R_{x} = 25 - 255 = 0" />
<img src="https://latex.codecogs.com/svg.image?G_{y}(2,1)&space;=&space;255&space;-&space;153&space;=&space;102" title="https://latex.codecogs.com/svg.image?R_{x} = 255 - 153 = 102" />
<img src="https://latex.codecogs.com/svg.image?B_{y}(2,1)&space;=&space;153&space;-&space;153&space;=&space;0" title="https://latex.codecogs.com/svg.image?R_{x} = 153 - 153 = 0" />

<img src="https://latex.codecogs.com/svg.image?\sqrt{\Delta_{x}^{2}(1,&space;2)&space;&plus;&space;\Delta_{y}^{2}(1,&space;2)}&space;=&space;\sqrt{52024}" title="https://latex.codecogs.com/svg.image?\sqrt{\Delta_{x}^{2}(x, y) + \Delta_{x}^{2}(x, y)} = \sqrt{52024}" />

For pixels at the edge of the image, you should "wrap around" the image. For example, for the pixel (0, 1) of the image below:

<img src="https://latex.codecogs.com/svg.image?R_{x}(0,1)&space;=&space;255&space;-&space;255&space;=&space;0" title="https://latex.codecogs.com/svg.image?R_{x} = 225 - 255 = 0" />
<img src="https://latex.codecogs.com/svg.image?G_{x}(0,1)&space;=&space;101&space;-&space;101&space;=&space;0" title="https://latex.codecogs.com/svg.image?R_{x} = 205 - 203 = 2" />
<img src="https://latex.codecogs.com/svg.image?B_{x}(0,1)&space;=&space;255&space;-&space;51&space;=&space;204" title="https://latex.codecogs.com/svg.image?R_{x} = 255 - 51 = 204" />
<img src="https://latex.codecogs.com/svg.image?R_{y}(0,1)&space;=&space;255&space;-&space;255&space;=&space;0" title="https://latex.codecogs.com/svg.image?R_{x} = 25 - 255 = 0" />
<img src="https://latex.codecogs.com/svg.image?G_{y}(0,1)&space;=&space;153&space;-&space;255&space;=&space;-102" title="https://latex.codecogs.com/svg.image?R_{x} = 255 - 153 = 102" />
<img src="https://latex.codecogs.com/svg.image?B_{y}(0,1)&space;=&space;153&space;-&space;153&space;=&space;0" title="https://latex.codecogs.com/svg.image?R_{x} = 153 - 153 = 0" />

<img src="https://latex.codecogs.com/svg.image?\sqrt{\Delta_{x}^{2}(1,&space;0)&space;&plus;&space;\Delta_{y}^{2}(1,&space;0)}&space;=&space;\sqrt{52020}" title="https://latex.codecogs.com/svg.image?\sqrt{\Delta_{x}^{2}(x, y) + \Delta_{x}^{2}(x, y)} = \sqrt{52024}" />

<img width="878" alt="image" src="https://user-images.githubusercontent.com/72530527/168500342-97cd9318-09a7-4bb9-bb3c-7e05aa026f20.png">

The computed dual-gradient energy in an image is then stored. This is done by dividing the original energy by 10, and casting it to 
(`uint8_t`). For each pixel, set the r, g, and b channels to the same value (the energy divided by 10 and cast to `uint8_t`).

As such, the resultant dual-gradient energy of the image `3x4.png` is:
```
        14      22      14
        14      22      14
        14      22      14
        14      22      14
```

## Part 2: Cost Array

This part describes the function `void calc_energy(struct rgb_img *im, struct rgb_img **grad);`, which allocates and computes the 
dynamic array `*best_arr`.

`(*best_arr){i*width+j]` contains the minimum cost of a seam from the top of the gradient to the point <img src="https://latex.codecogs.com/svg.image?(i,&space;j)" title="https://latex.codecogs.com/svg.image?(i, j)" />.

For example, for the `6x5.png`, the dual-gradient image is:
```
24      22      30      15      18      19
12      23      15      23      10      15
11      13      22      13      21      14
13      15      17      28      19      21
17      17      7       27      20      19
```

And the best array is: 
```
24.000000       22.000000       30.000000       15.000000       18.000000       19.000000
34.000000       45.000000       30.000000       38.000000       25.000000       33.000000
45.000000       43.000000       52.000000       38.000000       46.000000       39.000000
56.000000       58.000000       55.000000       66.000000       57.000000       60.000000
73.000000       72.000000       62.000000       82.000000       77.000000       76.000000
```

## Part 3: Recover the Seam

This part describes the function `void recover_path(double *best, int height, int width, int **path);`, which allocates a path 
through the minimum seam as defined by the array `best`.

For the best array above, the path is `[3, 4, 3, 2, 2]`.

## Part 4: Remove the Seam

This part describes the function `void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path);`, which creates the
destination image, and writes to it the source image with the seam removed.

## Part 5: Visualize the result

Upon completing the functions in `seamcarving.c`, run the program repeatedly to remove seams from an image.

```
    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "HJoceanSmall.bin");
    
    for(int i = 0; i < 5; i++){
        printf("i = %d\n", i);
        calc_energy(im,  &grad);
        dynamic_seam(grad, &best);
        recover_path(best, grad->height, grad->width, &path);
        remove_seam(im, &cur_im, path);

        char filename[200];
        sprintf(filename, "img%d.bin", i);
        write_img(cur_im, filename);


        destroy_image(im);
        destroy_image(grad);
        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im);
```
_Credit_: the assignment was designed by Michael Guerzhoy.
