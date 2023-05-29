#ifndef IO_H
#define IO_H

#include "types.h"

u8 port_byte_in(u16 port, u8 data);
void port_byte_out(u16 port, u8 data);

#endif