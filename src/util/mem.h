#ifndef __MEM_H__
#define __MEM_H__

#include "structs.h"

buf_str allocate_serial_buffers(slc_str slice);
buf_str allocate_parallel_buffers(comm_str comm, slc_str slice);

void dealocate_buffers(buf_str *buffers);

int swap_ptrs(double ***ptr1, double ***ptr2);
#endif	//__MEM_H__