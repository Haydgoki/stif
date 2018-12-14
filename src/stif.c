#include <stdio.h>
#include <stdlib.h>
#include "stif.h"


/*!
 * \brief Free a stif structure
 *
 * \param s the stif structure
 */
void stif_free(stif_t *s){

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
    return NULL;
}

/*!
 * \brief Free a stif block
 *
 * \param b the block
 */
void stif_block_free(stif_block_t *b){

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
stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size){
    return NULL;
}


int main(int argc, char *argv[]){
    return 0
}
