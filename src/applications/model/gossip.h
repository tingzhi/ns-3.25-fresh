/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2016 Tingzhi Li 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GOSSIP_H
#define GOSSIP_H

#include <vector>

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/gossip-udp-server.h"

namespace ns3 {

/**
 * \ingroup 
 * \class Gossip
 *
 * \brief An application to generate necessary packets to gossip.
 */
 
class Gossip : public Application
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor.
   */
  Gossip ();

  /**
   * \brief Constructor.
   */
  virtual ~Gossip ();
  
  void SetCurrentValue (int val);
  int GetSentMessages ( void);
  int GetPacketHops ( void);

/************************************************************************/
 
  void PrintSentPkt ();
  void SetSequenceNumber (uint32_t seq);
  void SendPayload(Ipv4Address src, Ipv4Address dest);
  Ipv4Address GetIpv4 (Ptr<Node> node, uint32_t index);
  std::vector<double> GetSentPktTime (void); 
  void SetNumberOfNodes (uint32_t val);
  void GeneratePackets (void);
  void SetSourceNode (NodeContainer c);
  void SetUdpServer (Ptr<GossipUdpServer> val);
  void ControlTraffic (void);

  
  NodeContainer m_sourceNodes;
  uint32_t numOfNodes;
//  uint32_t pktNum;
  Ptr<GossipUdpServer> udpServer;

/************************************************************************/

protected:
    
  uint8_t CurrentValue; //!< The current Value
  int SentMessages; //!< Amount of messages sent out
  uint8_t PacketHops; //!< How many hops the data packet experienced
  uint16_t seqNum;
  std::vector<double> sentPktTime;
  
  std::vector<int> sourceNodePktStore;
  
  /**
   * \brief Dispose method.
   */
  virtual void DoDispose (void);

  /**
   * \brief Send a message to the given destination indicating the type
   * \param the source
   * \param the destination
   * \param the message type
   */
//  void SendMessage(Ipv4Address src, Ipv4Address dest, int type);

private:
  /**
   * \brief Start the GossipProcess
   */
//  void GossipProcess();

  /**
   * \brief Start the SolicitProcess
   */
//  void Solicit(void);

  /**
   * \brief Start the application.
   */
  virtual void StartApplication ();

  /**
   * \brief Stop the application.
   */
  virtual void StopApplication ();
};

} // Namespace ns3

#endif // GOSSIP_H
