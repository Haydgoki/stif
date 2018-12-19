#include <stdio.h>
#include <stdlib.h>
#include "stif.h"

#define DEBUG 1
#define VERBOSE 0

/* Buffer to int. With a buffer which contains little endian numbers. return an int from it*/
int32_t _btoi(const unsigned char* b){ return *((int32_t *)b);}

void stif_block_free(stif_block_t *b){
    #if VERBOSE == 1
    printf("V : >> Entering stif_block_free\n");
    #endif
    if(b != NULL){
        free(b -> data); /*TODO Check if data not null*/
        free (b);
    }
}

void stif_free(stif_t *s){
    #if VERBOSE == 1
    printf("V : >> Entering stif_free\n");
    #endif
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
    free(s);
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read){
    #if VERBOSE == 1
    printf("V : >> Entering read_stif_block\n");
    #endif
    if(buffer == NULL){
        perror("ERROR : Buffer pointer is null\n");
        return NULL;
    }
    // >> READ DATA'S BLOCK HEADER <<
    (* bytes_read) = 0;
    if(buffer_size<5){
        perror("ERROR : Moins de 5 octets dans le BUFFER pour un nouveau block de datas\n");
        return NULL;
    }
    (* bytes_read) = 1;
    if(buffer[0] != 1){ // On s'attend forcément à un type data
        fprintf(stderr, "ERROR : BlockTypeERROR : Nouveau block type != DATA %x\n", buffer[0]);
        return NULL;
    }
    (* bytes_read) = 5;
    int32_t i = 1;
    int32_t data_size = _btoi(buffer + i);
    if(data_size < 0){
        perror("ERROR : Data's block size < 0 \n");
        return NULL;
    }
    if((u_int32_t)data_size > buffer_size - 5){ // Cast OK, because we checked that data_size>0
        perror("ERROR : buffer_size - 5 < data_size \n");
        return NULL;
    }
    i += 4; // i = 5

    stif_block_t * res = malloc(sizeof(stif_block_t));
    if(res == NULL){
        perror("ERROR : malloc de res à échoué\n");
        return NULL;
    }
    uint8_t *data = malloc(data_size);
    if(data == NULL){
        perror("ERROR : malloc de Datas à échoué\n");
        free(res);
        return NULL;
    }
    // Pour toutes les datas du buffer, on les ajoute  aux datas
    for(; i < data_size + 5; i ++){
        *(data + i - 5) = buffer[i];
    }
    (* bytes_read) = i; // Voir si il faut pas le mettre dans le for, au cas où ça plante
    res -> block_type = 1;
    res -> block_size = data_size;
    res -> data = data;
    res -> next = NULL;
    return res;
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size){
    #if VERBOSE == 1
    printf("V : >> Entering parse_stif\n");
    #endif
    if(buffer == NULL){
        perror("ERROR : >> Buffer pointer is NULL\n");
        return NULL;
    }


    // PARSE MAGIC NUMBER : CA FE
    if(buffer_size < 22){ /* 22 = PLUS PETITE IMAGE POSSIBLE = 2o (magic) + 5o (header_block) + 9o (block head) + 5(header_block) + 1(block_gray_scale)*/
        perror("ERROR : >> buffer_size < 22\n");
        return NULL;
    }
    if( * ((u_int16_t *)buffer) != STIF_MAGIC ){
        perror("ERROR : >> MAGIC != 0x CAFE\n");
        return NULL;
    }
    u_int32_t i = 2; // index du buffer
// >>> READ LE BLOC HEADER <<<
    // >> head du block header <<
    if(buffer[i] != 0){ // Si ce n'est pas un type HEADER
        perror("ERROR : >> First block != HEADER \n");
        return NULL;
    }
    i ++; // i=3
    if( * ( (int32_t *)(buffer + i) ) != STIF_BLOCK_HEADER_SIZE){ // On attend un block HEADER la taille est forcément de 9
        perror("ERROR : >> First block size != STIF_BLOCK_HEADER_SIZE \n");
        return NULL;
    }
    i += 4; // i=7

    // >> vérif du contenu du block header <<
    if( _btoi(buffer + i) <= 0 ){
        perror("ERROR : >> WIDTH <= 0 \n");
        return NULL;
    }
    if( _btoi(buffer + i + 4) <= 0 ){
        perror("ERROR : >> HEIGHT <= 0 \n");
        return NULL;
    }
    if(buffer[i+8] != 0 && buffer[i+8] != 1 ){ // check type d'image == (rgb || grayscale)
        perror("ERROR : >>  Image color_type diff de rgb et de gs \n");
        return NULL;
    }
    size_t pixel_block_size = buffer[i+8] ? 3 : 1; // Si on a color_type (==1) alors on a du rgb, donc chaque data fait 3o. En gs on a un pixel de 1o
    /* On passe le HEADER (9o) et i , et on vérifie que le nombre d'octet restant est modulo la taille suposée des blocks de pixels.
       e.g. si buffer_size - i - 9 == 32, alors on peut caser 4 pixels si on est en rgb car 32%8 == 0.
       Mais si on est en mode gs , on peut caser que 5.33 pixels, car 32%6 == 2

    if( ((buffer_size - i - 9) % pixel_block_size) != 0){
        return NULL;
    }*/


    // header correct, on peut initialiser l'image
    stif_header_t header;
    header.width = _btoi(buffer + i); // Lecture width
    header.height = _btoi(buffer + i + 4); // ...height
    int8_t color_type = buffer[i+8]; // ...color_type
    header.color_type = color_type;
    i += 9; // i=16, les 9 octets des datas du bloc head
    if(buffer_size < ( 2 + 5 + 9 + 5 + ( header.width * header.height * pixel_block_size )) ){ // taille minimum avec 1 seul block de datas
        perror("ERROR : >> Le Buffer ne peut pas acceuillir autant de pixels qu'il est indiqué par la width, la height et le color type \n");
        return NULL;
    }

    // On a fait la vérif qu'il y avait des datas blocks après, on commence donc à les allouer
    stif_t * res = malloc(sizeof(stif_t));
    if(res == NULL){ // problème pour allouer de la mem
        perror("ERROR : >> Malloc pour la stif_s est NULL \n");
        return NULL;
    }
    u_int8_t * pixels = malloc(header.width*header.height*pixel_block_size);
    res -> header = header;
    res -> grayscale_pixels = pixel_block_size == 1 ? pixels : NULL;
    res -> rgb_pixels = pixel_block_size == 3 ? (pixel_rgb_t *)pixels : NULL;
    res -> block_head = NULL;

    if(pixels == NULL){
        perror("ERROR : >> Malloc pour pixels est NULL \n");
        stif_free(res); // free res and pixels
        return NULL;
    }
    size_t pixel_i = 0;

//  >>> READ BLOCKS DATA <<<
    size_t bytes_read;
    stif_block_t **next_block_pt = &(res -> block_head); // Variable contenant le pointeur où l'on devra renseigner l'adresse du prochain block
    stif_block_t * new_block;
    while(buffer_size - i > 0){ // Tant qu'il reste des choses à lire

        new_block = read_stif_block(buffer + i,  buffer_size - i, &bytes_read);
        if(new_block == NULL){
            perror("ERROR : >> Echec du read du Bloc DATA. Valeur de retour nulle\n");
            stif_free(res);
            return NULL;
        }
        (* next_block_pt) = new_block;        //On vient lier l'addresse de next, sur le block précédent
        next_block_pt = &(new_block -> next); // On sauvegarde l'addresse où l'on devra greffer le prochain block
        i += bytes_read;
        for(size_t j = 0; j < bytes_read - 5 ; j++){ // bytes_read - 5 car on skip les 5 premiers octets qui étaient la head
            pixels[pixel_i] = new_block -> data[j];
            pixel_i ++;
        }
    }
    // On vérifie qu'on a lu tous les pixels
    if(pixel_i != header.width*header.height*pixel_block_size){
        stif_free(res); // free res and pixels
        return NULL;
    }

    #if VERBOSE == 1
    printf("V : >> Exiting parse_stif\n");
    #endif

    // TODO dernière vérif ?
    return res;
}
