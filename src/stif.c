#include <stdio.h>
#include <stdlib.h>
#include "stif.h"

#define DEBUG 1
#define VERBOSE 0

/* Buffer to int. With a buffer which contains little endian numbers. return an int from it*/
int32_t _btoi(const unsigned char* b){int32_t res = b[0]<<24 | b[1]<<16 | b[2]<<8 | b[3]; return res;}

void stif_block_free(stif_block_t *b){
    #if DEBUG == 1
    printf("D : >> Entering stif_block_free\n");
    #endif
    if(b != NULL){
        free(b -> data); /*TODO Check if data not null*/
        free (b);
    }
}

void stif_free(stif_t *s){
    #if DEBUG == 1
    printf("D : >> Entering stif_free\n");
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

stif_block_t *read_stif_block_data(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read){
    #if DEBUG == 1
    printf("D : >> Entering read_stif_block_data\n");
    #endif
    // >> READ DATA'S BLOCK HEADER <<
    (* bytes_read) = 1;
    if(buffer[0] != 1){ // On s'attend forcément à un type data
        return NULL;
    }
    int i = 1;
    int32_t data_size = _btoi(buffer + i);
    if( buffer_size - data_size != 5){ // Si la taille du buffer qu'on lui envoie ne correspond pas à la taille des datas indiqué + les 5o de l'entête
        (* bytes_read) = 5;
        return NULL;
    }
    i += 4; // i = 5
    (* bytes_read) = i;
    uint8_t *data;
    if(buffer_size - 5 == 1){ // si on doit lire un pixel GS
        data = malloc(sizeof(pixel_grayscale_t));
        if(data == NULL){
            return NULL;
        }
        * ((pixel_grayscale_t *)data) = buffer[i];
        i += 1; // i = 6
    } else if (buffer_size - 5 == 3){ // si on doit lire un pixel RGB
        data = malloc(sizeof(pixel_rgb_t));
        if(data == NULL){
            return NULL;
        }
        ((pixel_rgb_t *)data) -> red = buffer[i]; //5ème octet
        ((pixel_rgb_t *)data) -> green = buffer[i+1]; //6ème
        ((pixel_rgb_t *)data) -> blue = buffer[i+2]; //7ème
        i += 3; // i = 6
    } else { // une taille de datas qui ne correspond à rien
        return NULL;
    }
    (* bytes_read) = i;
    if (buffer_size != (* bytes_read)){ // On doit avoir tout lu normalement
      return NULL;
    }
    stif_block_t * res = malloc(sizeof(stif_block_t));
    if(res == NULL){
        free(data);
        return NULL;
    }
    res -> block_type = 1;
    res -> block_size = buffer_size - 5;
    res -> data = data;
    return res;
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size){
    #if DEBUG == 1
    printf("D : >> Entering parse_stif\n");
    #endif
    // PARSE MAGIC NUMBER : CA FE
    if(buffer_size < 22){ /* 22 = PLUS PETITE IMAGE POSSIBLE = 2o (magic) + 5o (header_block) + 9o (block head) + 5(header_block) + 1(block_gray_scale)*/
        return NULL;
    }
    #if DEBUG == 1
    printf("D : >> buffer_size > 22\n %X %X %X %X %X %X %X %X\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
    #endif
    if(buffer[0] != 0xCA || buffer[1] != 0xFE ){ // Si on a pas CA ou si on a pas FE
        return NULL;
    }
    #if DEBUG == 1
    printf("D : >> Magic number CAFE parsed\n");
    #endif
    u_int i = 2; // index du buffer
// >>> READ LE BLOC HEADER <<<

    // >> head du block header <<
    if(buffer[i] != 0){ // Si ce n'est pas un type HEADER
        return NULL;
    }
    i ++; // i=3
    if( _btoi(buffer + i) != STIF_BLOCK_HEADER_SIZE){ // On attend un block HEADER la taille est forcément de 9
        return NULL;
    }
    i += 4; // i=7

    // >> vérif du contenu du block header <<
    if( _btoi(buffer + i) < 0 ){ // check Width positive
        return NULL;
    }
    if( _btoi(buffer + i + 4) < 0 ){ // check Height positive
        return NULL;
    }
    if(buffer[i+8] != 0 && buffer[i+8] != 1 ){ // check type d'image == (rgb || grayscale)
        return NULL;
    }
    // L'expression correspond au color_type. Si on est en rgb alors les pixels feront 8o (5o header + 3o data_rgb), autrement ils feront 5 px (5o header + 1o data_gs)
    size_t pixel_block_size = buffer[i+8] ? (5 + 3) : (5 + 1); // Si on a color_type (==1) alors on a du rgb, donc chaque futur bloc fera 5o(header)+3o(rgb). sinon 5o(header)+1o(gs)
    /* On passe le HEADER (9o) et i , et on vérifie que le nombre d'octet restant est modulo la taille suposée des blocks de pixels.
       e.g. si buffer_size - i - 9 == 32, alors on peut caser 4 pixels si on est en rgb car 32%8 == 0.
       Mais si on est en mode gs , on peut caser que 5.33 pixels, car 32%6 == 2
    */
    if( ((buffer_size - i - 9) % pixel_block_size) != 0){
        return NULL;
    }
    // header correct, on peut initialiser l'image
    stif_header_t header;
    header.width = _btoi(buffer + i); // Lecture width
    header.height = _btoi(buffer + i + 4); // ...height
    int8_t color_type = buffer[i+8]; // ...color_type
    header.color_type = color_type;
    i += 9; // i=16, les 9 octets des datas du bloc head

    // On a fait la vérif qu'il y avait des datas blocks après, on commence donc à les allouer
    stif_t * res = malloc(sizeof(stif_t));
    if(res == NULL){ // problème pour allouer de la mem
      return NULL;
    }
    /*
    stif_block_t *current_data_block = malloc(sizeof(stif_block_t));
    if(current_data_block == NULL){
      free(header);
      free(res);
      return NULL;
    }*/
    res -> header = header;
    res -> grayscale_pixels = NULL;
    res -> rgb_pixels = NULL;
    res -> block_head = NULL; //TODO ne pas oublier de link la chaine

//  >>> READ BLOCKS DATA <<<

    size_t bytes_read;
    stif_block_t **next_block_pt = &(res -> block_head); // Variable contenant le pointeur où l'on devra renseigner l'adresse du prochain block
    stif_block_t * new_block;
    while(buffer_size - i > 0){ // Tant qu'il reste des choses à lire
        if(buffer_size - i < pixel_block_size){ // TOTALLY UNEXPECTED. Normalement la taille de la struct sans la head compte n*pixel_block_size, donc on est censé avancer de pixel_block_size à chaque fois.
            stif_free(res);
            return NULL;
        }
        new_block = read_stif_block_data(buffer + i,  pixel_block_size, &bytes_read);
        if(new_block == NULL){
          stif_free(res);
          return NULL;
        }
        (* next_block_pt) = new_block;        //On vient lier l'addresse de next, sur le block précédent
        next_block_pt = &(new_block -> next); // On sauvegarde l'addresse où l'on devra greffer le prochain block
        i += bytes_read;
    }


    return NULL;
}
