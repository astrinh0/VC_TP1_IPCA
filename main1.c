#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vc.h"

int main(int argc, char *argv[]){

    IVC* image;
    OVC* blobs;
    int nblobs;
    int *nlabels;

    int labels;
    int *ptrLabels = &labels;

    // Verificação dos parametros de entrada
    if(argc < 2 ){
        printf("ERROR -> Unspecified file!\n");
        getchar();
        return 0;
    }
    
    // Inicialização da imagem de entrada
    image = vc_read_image(argv[1]);

    IVC* temp  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp1  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp2  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp3  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp4  = vc_image_new(image->width, image->height, 1, 255);

    // Verificação de erro ao iniciar a imagem
    if(image == NULL){
        printf("ERROR -> VC_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    vc_gray_to_binary(image, temp, 65);    
    //vc_write_image("binary.pbm", temp);

    vc_binary_erode(temp, temp1, 10);
    //vc_write_image("vc-0001.pbm", temp1);

    vc_binary_dilate(temp1, temp2, 10);
    //vc_write_image("vc-0001.pbm", temp2);

    vc_binary_close(temp2, temp3, 6, 8);
    vc_write_image("vc-0001.pbm", temp3);

    blobs = vc_binary_blob_labelling(temp, temp4, ptrLabels);
    vc_write_image("blobs.pgm", temp4);

    vc_binary_blob_info(temp4, blobs, labels);


    printf("\nlabels: %d", labels);
    printf("\nperimetro: %d", blobs->perimeter);
    printf("\narea: %d\n", blobs->area);
    printf("\ncentro massa: x: %d y: %d", blobs->xc, blobs->yc);


 /*    vc_gray_to_binary_midpoint(image, temp, 21);
    //vc_write_image("vc-0001.ppm", temp);


    vc_binary_erode(temp, temp1, 10);
    //vc_write_image("vc-0001.ppm", temp1);


    vc_binary_dilate(temp1, temp2, 30);
    //vc_write_image("vc-0001.ppm", temp2);


    vc_binary_close(temp2, temp3, 16, 24);
    vc_write_image("vc-0001.ppm", temp3); */


    /* vc_gray_edge_prewitt(image, temp4, 0.85);
    vc_write_image("pewitt.ppm", temp4); */


    /* vc_binary_blob_labelling(temp3, temp4, label);

    nblobs = *label;

    printf("\nnblobs %d\n", nblobs);

    vc_binary_blob_info(temp3, blobs, nblobs);

    printf("\nNumero blobs: %d\n", label); */


    vc_image_free(image);
    vc_image_free(temp);
    vc_image_free(temp1);
    vc_image_free(temp2);
    vc_image_free(temp3);
    vc_image_free(temp4);


    return 1;
}