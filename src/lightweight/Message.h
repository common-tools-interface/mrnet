/****************************************************************************
 *  Copyright 2003-2011 Dorian C. Arnold, Philip C. Roth, Barton P. Miller  *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#if !defined(__message_h)
#define __message_h 1

#include <stdio.h>
#include <stdlib.h>

#include "mrnet_lightweight/Network.h"
#include "mrnet_lightweight/Packet.h"
#include "pdr.h"
#include "vector.h"

#ifndef os_windows
#include <sys/uio.h>
#include <sys/socket.h>
#endif

typedef struct {
   Packet_t* packet;
} Message_t ;

Message_t* new_Message_t();

int Message_recv(int sock_fd, vector_t* packets_in, Rank iinlet_rank);
int Message_send(Message_t* msg_out, int sock_fd);

int MRN_write(int ifd, void *ibuf, size_t ibuf_len);
int MRN_read(int fd, void *buf, size_t count);

#endif /* __message_h */
