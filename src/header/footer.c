//
// Created by Viktor Liljenberg on 2025-01-08.
//

#include "footer.h"

uint64_t create_footer(uint8_t size){
    uint64_t footer = size;
    // set size field
    footer = footer << 56;
    // now merge with tag
    footer = footer | FOOTER_IDENTIFIER;
    return footer;
}


//last 7 bytes, mask 2 msb bytes
bool confirm_tag(uint64_t tag){
  tag = mask_from_msb(tag, 8);
  return tag == FOOTER_IDENTIFIER;
}