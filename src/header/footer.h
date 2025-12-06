//
// Created by Viktor Liljenberg on 2025-01-08.
//

#ifndef FOOTER_H
#define FOOTER_H

#include <stdint.h>
#include <stdbool.h>
#include "bit_manip.h"
#include "bit_vector.h"

#define FOOTER_IDENTIFIER 0x00DEADBEEFCAFEBAL


uint64_t create_footer(uint8_t size);

bool confirm_tag(uint64_t tag);

#endif //FOOTER_H
