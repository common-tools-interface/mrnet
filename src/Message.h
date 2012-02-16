/****************************************************************************
 *  Copyright 2003-2011 Dorian C. Arnold, Philip C. Roth, Barton P. Miller  *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/

#if !defined(Message_h)
#define Message_h

#include <list>
#include <vector>

#include "mrnet/Error.h"
#include "mrnet/Types.h"
#include "mrnet/Packet.h"
#include "xplat/Monitor.h"
#include "mrnet/Network.h"
#include "xplat/NCIO.h"

#include "PerfDataEvent.h"
#include "PerfDataSysEvent.h"


#define MESSAGE_PREALLOC_LEN 10

namespace MRN
{

uint64_t get_TotalBytesSend(void);
uint64_t get_TotalBytesRecv(void);

class Message: public Error{
 public:
    Message( Network * net );
    ~Message();

    int send( XPlat::XPSOCKET isock_fd );
    int recv( XPlat::XPSOCKET isock_fd, std::list < PacketPtr >&opackets, Rank iinlet_rank );

    void add_Packet( PacketPtr );
    size_t size_Packets( void );
   
    void waitfor_MessagesToSend( void );

 private:

    Network * _net;
    enum {MRN_QUEUE_NONEMPTY};

    std::list< PacketPtr > _packets;
    XPlat::Monitor _packet_sync;

    char *_packet_count_buf, *_packet_sizes_buf;
    size_t _packet_count_buf_len, _packet_sizes_buf_len, _ncbuf_len;

    uint64_t _packet_sizes[MESSAGE_PREALLOC_LEN];
    XPlat::NCBuf _ncbuf[MESSAGE_PREALLOC_LEN];
};

ssize_t MRN_send( XPlat::XPSOCKET fd, const char *buf, size_t count );
ssize_t MRN_recv( XPlat::XPSOCKET fd, char *buf, size_t count );

}                               // namespace MRN

#endif                          /* Message_h */
