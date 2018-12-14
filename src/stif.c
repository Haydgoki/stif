#include <stdio.h>
#include <stdlib.h>
#include "stif.h"

/*
* Il faut free les datas, puis le block en lui même
*/
void stif_block_free(stif_block_t *b){
    if(b != NULL){
        free(b -> data); /*TODO Check if data not null*/
        free (b);
    }
}

/*
 * while(next_block )free tous les blocks dans block_head.
 * free  grayscale_pixels
 * free rgb_pixels
 * free s
 */
void stif_free(stif_t *s){
    // TODO vérif que s not NULL
    if( s == NULL ){
        return;
    }
    if(s -> grayscale_pixels != NULL){
        free(s -> grayscale_pixels);
    }
    if(s -> rgb_pixels != NULL){
        free(s -> grayscale_pixels);
    }
    stif_block_t *current = s -> block_head;
    stif_block_t * next;

    /*Tant qu'on a des blocks suivant à clear */
    while(current != NULL){
        next = current -> next;
        stif_block_free(current);
        current = next;
    }
}

/*!
 * \brief Read a stif block
 *
 * \param buffer the buffer
 * \param buffer_size size of the buffer
 * \param bytes_read number of bytes read
 *
 * \return the newly allocated block (to be freed with stif_block_free) or NULL on error
 */
stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read){
    // >> READ DATA'S BLOCK HEADER <<
        /*
            vérif que c'est un type data : byte == 1
            vérif que la block size correspond bien à RGB si image RGB ou GS si image GS
        */



    // >> READ DATA'S BLOCK DATA <<


    return NULL;
}


/*!
 * \brief Parse a STIF
 * This function reads a whole STIF file (magic, header and image data).
 * It will store the block in the block_head list and pixels as well (according to the color type).
 *
 * \param buffer the buffer
 * \param buffer_size size of the buffer
 *
 * \return the parsed image (to be freed with stif_free) or NULL on error
 */

//TODO revoir tous les pointeurs sur buffer de i. J'ai fait nimp, au lieu de mettre des * j'ai mis des &. Et j'ai fait buffer[i] et non buffer + i 
stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size){
    // PARSE MAGIC NUMBER : CA FE
    if(buffer_size < 22){ /* 22 = PLUS PETITE IMAGE POSSIBLE = 2o (magic) + 5o (header_block) + 9o (block head) + 5(header_block) + 1(block_gray_scale)*/
        return NULL;
    }
    if(bufffer[0] != 0xCA || buffer[1] != 0xFE ){ // Si on a pas CA et FE
        return NULL;
    }
    stif_t *res;
    stif_header_t *header;
    int32_t block_size;
    int i = 2; // index du buffer



// >>> READ LE BLOC HEADER <<<

    // >> head du block header <<
    if(buffer[i] != 0){ // Si ce n'est pas un type HEADER
        return NULL;
    }
    i ++; // i=3
    if( &(int32_t *)buffer[i] != STIF_BLOCK_HEADER_SIZE){ // On attend un block HEADER la taille est forcément de 9
        return NULL;
    }
    i += 4; // i=7

    // >> contenu du block header <<
    if( &(int32_t *)buffer[i]) < 0 ){ // check Width positive
        return NULL;
    }
    if( &(int32_t *)buffer[i+4]) < 0 ){ // check Height positive
        return NULL;
    }
    if( &(int8_t *)buffer[i+8]) != 0 && &(int8_t *)buffer[i+8]) != 1 ){ // check type d'image == rgb ou grayscale
        return NULL;
    }
    // L'expression correspond au color_type. Si on est en rgb alors les pixels feront 8o (5o header + 3o data_rgb), autrement ils feront 5 px (5o header + 1o data_gs)
    int8_t pixel_block_size = (&(int32_t *)buffer[i+8]) ? 5 + 3 : 5 + 1; // Si on a color_type (==1) alors on a du rgb, donc chaque futur bloc fera 5o(header)+3o(rgb). sinon 5o(header)+1o(gs)
    /* On passe le HEADER (9o) et i , et on vérifie que le nombre d'octet restant est modulo la taille suposée des blocks de pixels.
       e.g. si buffer_size - i - 9 == 32, alors on peut caser 4 pixels si on est en rgb car 32%8 == 0.
       Mais si on est en mode gs , on peut caser que 5.33 pixels, car 32%6 == 2
    */
    if( ((buffer_size - i - 9) % pixel_block_size) != 0){
        return NULL;
    }
    // header correct, on peut initialiser l'image
    stif_header_t *header = malloc(sizeof(stif_header_t));
    header -> width = &(int32_t *)buffer[i];
    header -> height = &(int32_t *)buffer[i+4]; // On passe la width
    int8_t color_type = &(int32_t *)buffer[i+8]; // on passe la width et la height
    header -> color_type = color_type;

    i += 9; // i=16

    // On a fait la vérif qu'il y avait des datas blocks après, on commence donc à les allouer
    stif_block_t *current_data_block = malloc(sizeof(stif_block_t));
    stif_t * res = malloc(sizeof(stif_t));
    res -> header = header;
    res -> grayscale_pixels = NULL;
    res -> rgb_pixels = NULL;
    res -> block_head = current_data_block; //TODO ne pas oublier de link la chaine

//  >>> READ BLOCKS DATA <<<

    size_t bytes_read;
    while(buffer_size - i > 0){
        stif_block_t * = read_stif_block(buffer + i,  pixel_block_size, &bytes_read){
        i+= pixel_block_size;
    }


    return NULL;
}
