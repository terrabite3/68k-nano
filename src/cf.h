#pragma once

#include "type.h"

// Buffers for CompactFlash should usually be the size of one sector, 512 bytes.
// The buffer_len argument is the number of WORDS, not the number of BYTES!

static const uint16_t SECTOR_BUFFER_LEN = 256;


uint16_t cf_identify(uint16_t* buffer, uint16_t buffer_len);