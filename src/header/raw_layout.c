#include "raw_layout.h"
#include "header.h"


u_int64_t to_raw_layout(raw_layout_t size){
  raw_layout_t raw = size;
  raw = raw << 2;
  return raw;
}

u_int64_t get_size(raw_layout_t raw){
  raw = raw >> 2;
  return raw;
}

header_t *create_raw_header(const size_t bytes) {
    // Mask storleken till de första 62 bitarna
    uint64_t size = (uint64_t)bytes;

    // Skifta storleken för att lämna plats för signaturkoden i de sista 2 bitarna
    uint64_t raw = size << 2;

    // Lägg till signaturkoden för RAW_LAYOUT (antag 2 som signaturkod enligt get_header_type-logik)
    raw |= 2;

    return header_new(raw);
}

