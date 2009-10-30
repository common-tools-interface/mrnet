/****************************************************************************
 * Copyright � 2003-2008 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.          *
 ****************************************************************************/
#ifndef RSH_InternalNode_h
#define RSH_InternalNode_h

#include "InternalNode.h"
#include "RSHParentNode.h"
#include "RSHChildNode.h"

namespace MRN
{

class RSHInternalNode : public InternalNode,
                        public RSHParentNode,
                        public RSHChildNode
{
protected:
    virtual int proc_PacketFromParent( PacketPtr cur_packet );

public:
    RSHInternalNode( Network* inetwork,
                        std::string const& ihostname, Rank irank,
                        std::string const& iphostname, Port ipport, Rank iprank,
                        int listeningSocket,
                        Port listeningPort );
    virtual ~RSHInternalNode(void);
};

} // namespace MRN
#endif /* __internalnode_h */