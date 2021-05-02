#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vc.h"

int main(int argc, char *argv[]){

    IVC* image;
    OVC* blobs;

    int labels;
    int *ptrLabels = &labels;
    int i, cont = 0;

    /* Verificação dos parametros de entrada */
    if(argc < 2 )
    {
        printf("ERROR -> Unspecified file!\n");
        getchar();
        return 0;
    }
    
    /* Inicialização da imagem de entrada */
    image = vc_read_image(argv[1]);


    /* Verificação de erro ao iniciar a imagem */
    if(image == NULL){
        printf("ERROR -> VC_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    IVC* temp  = vc_image_new(image->width, image->height, 1, 255);
    IVC* temp1  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp2  = vc_image_new(image->width, image->height, 1, 1);
    IVC* temp3  = vc_image_new(image->width, image->height, 1, 1);

    /* Converter azul para escala de cinzentos */
    vc_rgb_get_blue_gray(image);

    /* Converter imagem para escala de cinzentos */
    vc_rgb_to_gray(image, temp);
    vc_write_image("gray.pgm", temp);

    /* Converter imagem na escala de cinzentos para binario com threshold a 40 */
    vc_gray_to_binary(temp, temp1, 40);
    vc_write_image("binary.pbm", temp1);

    /* Aplicamos dilatacao e erosao na imagem, kernel de 3 e 5 respetivamente */
    vc_binary_close(temp1, temp2, 3, 5);
    vc_write_image("close.pbm", temp2);

    /* Etiquetagem dos blobs */
    blobs = vc_binary_blob_labelling(temp2, temp3, ptrLabels);

    /* Calculo de area, perimetro e centro de massa de cada blob */
    /* Fazemos tambem as bounding boxes e sinalizamos o centro de massa */
    vc_binary_blob_info(temp3, blobs, labels);
    vc_write_image("final.pbm", temp3);

    /* Mostra numero total de Blobs */
    printf("\nTotal e Blobs: %d", labels);

    /* Mostra informacao sobre os Blobs com nucleo visivel */
    for (i = 0; i < labels; i++)
    {
        if (blobs[i].area > 40)
        {
            cont++;
            printf("\nBlob n: %d", cont);
            printf("\nPerimetro: %d", blobs[i].perimeter);
            printf("\nArea: %d", blobs[i].area);
            printf("\nCentro massa: x: %d y: %d", blobs[i].xc, blobs[i].yc);
            printf("\nBounding box: xmin: %d ymin: %d width: %d height: %d\n", blobs[i].x, blobs[i].y, blobs[i].width, blobs[i].height);
        }
    }

    /* Limpeza de memoria */
    vc_image_free(image);
    vc_image_free(temp);
    vc_image_free(temp1);
    vc_image_free(temp2);
    vc_image_free(temp3);
    
    return 1;
}