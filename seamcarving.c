
#include "seamcarving.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Part 1: Dual-Gradient Energy Function 
    The function will compute the dual-gradient energy function, and place it in the struct rgb_img *grad.
*/
void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    int red_x, green_x, blue_x;
    int red_y, green_y, blue_y;

    int x, y;

    float energy;
    uint8_t cast_energy;

    create_img(grad, im->height, im->width);

    for(y = 0; y < im->height; y++){
        for(x = 0; x < im->width; x++){
            if(x == 0){
                red_x = get_pixel(im, y, 1, 0) - get_pixel(im, y, im->width-1, 0);
                green_x = get_pixel(im, y, 1, 1) - get_pixel(im, y, im->width-1, 1);
                blue_x = get_pixel(im, y, 1, 2) - get_pixel(im, y, im->width-1, 2);

            }else if(x == im->width-1){
                red_x = get_pixel(im, y, 0, 0) - get_pixel(im, y, im->width-2, 0);
                green_x = get_pixel(im, y, 0, 1) - get_pixel(im, y, im->width-2, 1);
                blue_x = get_pixel(im, y, 0, 2) - get_pixel(im, y, im->width-2, 2);

            }else{
                red_x = get_pixel(im, y, x+1, 0) - get_pixel(im, y, x-1, 0);
                green_x = get_pixel(im, y, x+1, 1) - get_pixel(im, y, x-1, 1);
                blue_x = get_pixel(im, y, x+1, 2) - get_pixel(im, y, x-1, 2);
            }


            if(y == 0){
                red_y = get_pixel(im, im->height-1, x, 0) - get_pixel(im, 1, x, 0);
                green_y = get_pixel(im, im->height-1, x, 1) - get_pixel(im, 1, x, 1);
                blue_y = get_pixel(im, im->height-1, x, 2) - get_pixel(im, 1, x, 2);

            }else if(y == im->height-1){
                red_y = get_pixel(im, im->height-2, x, 0) - get_pixel(im, 0, x, 0);
                green_y = get_pixel(im, im->height-2, x, 1) - get_pixel(im, 0, x, 1);
                blue_y = get_pixel(im, im->height-2, x, 2) - get_pixel(im, 0, x, 2);

            }else{
                red_y = get_pixel(im, y-1, x, 0) - get_pixel(im, y+1, x, 0);
                green_y = get_pixel(im, y-1, x, 1) - get_pixel(im, y+1, x, 1);
                blue_y = get_pixel(im, y-1, x, 2) - get_pixel(im, y+1, x, 2);
            }
        
        energy = sqrt(pow(red_x, 2) + pow(green_x, 2) + pow(blue_x, 2) + pow(red_y, 2) + 
        pow(green_y, 2) + pow(blue_y, 2));

        cast_energy = (uint8_t)(energy/10);
        set_pixel(*grad, y, x, cast_energy, cast_energy, cast_energy);

        }
    }
}

/* Part 2: Cost Array */
void dynamic_seam(struct rgb_img *grad, double **best_arr){
    int i, j; // height, width
    int width = (int)(grad->width);
    int height = (int)(grad->height);

    *best_arr = (double *)malloc(sizeof(double)*width*height);

    double top, top_right, top_left, cur;

    for(j = 0; j < grad->width; j++){
        (*best_arr)[j] = (double)(get_pixel(grad, 0, j, 0));
    }
    
    for(i = 1; i < grad->height; i++){
        for(j = 0; j < grad->width; j++){
            top = (*best_arr)[(i-1)*width+j];
            cur = (double)(get_pixel(grad, i, j, 0));
            if(j == 0){ // left side
                top_right = (*best_arr)[(i-1)*width+(j+1)];
                (*best_arr)[i*width+j] = fmin(top, top_right) + cur;

            }else if(j == grad->width-1){ // right side
                top_left = (*best_arr)[(i-1)*width+(j-1)];
                (*best_arr)[i*width+j] = fmin(top, top_left) + cur;

            }else{
                top_right = (*best_arr)[(i-1)*width+(j+1)];
                top_left = (*best_arr)[(i-1)*width+(j-1)];
                (*best_arr)[i*width+j] = fmin(fmin(top_left, top), top_right) + cur;
            }
        }
    }
}

/* Part 3: Recover the seam */
void recover_path(double *best, int height, int width, int **path){
    int i, j, k;
    int h, w;
    int total_min = 0;
    double min_path; 

    /* Initialize path array */
    *path = (int *)malloc(sizeof(int)*(height));

    /* Last row */
    for(j = 0; j < width; j++){
        if(best[(height-1)*width + j] < best[(height-1)*width + total_min]){
            total_min = j;
        }
    }
    (*path)[height-1] = total_min;
    //printf("%d", total_min); // 2

    h = height-2;

    while(h >= 0){ // working backwards
        w = (*path)[h+1];

        if(w == 0){ // left edges
            if(best[h*width+w] < best[h*width+w+1]){
                (*path)[h] = w;
            }else{
                (*path)[h] = w+1;
            }

        }else if(w == width-1){ // right edges
            if(best[h*width+w] < best[h*width+w-1]){
                (*path)[h] = w;
            }else{
                (*path)[h] = w-1;
            }

        }else{ // and everywhere in between
            min_path = fmin(fmin(best[h*width+w], best[h*width+w-1]), best[h*width+w+1]);
            //printf("1: %f, 2: %f, 3: %f\n", best[h*width+w], best[h*width+w], best[h*width+w]);
            for(k = w-1; k < w+2; k++){
                if(min_path == best[h*width+k]){
                    (*path)[h] = k;
                }
            }
        }
        h--;
    }
}

/* Part 4: Write a function that removes the seam */
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    int h, w, w2;
    int red, green, blue;

    create_img(dest, src->height, src->width-1);

    for(h = 0; h < src->height; h++){
        for(w = 0; w < path[h]; w++){
            red = (int)get_pixel(src, h, w, 0);
            green = (int)get_pixel(src, h, w, 1);
            blue = (int)get_pixel(src, h, w, 2);
            set_pixel(*dest, h, w, red, green, blue);
        }

        for(w = path[h] + 1; w < src->width; w++){
            red = (int)get_pixel(src, h, w, 0);
            green = (int)get_pixel(src, h, w, 1);
            blue = (int)get_pixel(src, h, w, 2);
            set_pixel(*dest, h, w-1, red, green, blue);
        }
    }
}