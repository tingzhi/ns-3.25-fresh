/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2015 Marco Falke
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

#include <string>
#include "ns3/log.h"
#include "gossip.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("GossipApplication");

TypeId
Gossip::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Gossip")
    .SetParent<Application> ()
    .AddConstructor<Gossip> ()
  ;
  return tid;
}

Gossip::Gossip ()
{
  NS_LOG_FUNCTION (this);
  
  CurrentValue = 0;
  SentMessages = 0;
  PacketHops = 0;
  seqNum = 0;
//  packetInterval = Seconds(30.0);
}

Gossip::~Gossip ()
{
  NS_LOG_FUNCTION (this);
}

void
Gossip::DoDispose ( void )
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

/*
void
GossipGenerator::SendMessage_debug(Ipv4Address src, Ipv4Address dest, int type)
{
  SendPayload( src,  dest);
}
*/

void
Gossip::SendPayload(Ipv4Address src, Ipv4Address dest)
{
  NS_LOG_FUNCTION (this << dest );
  NS_LOG_INFO("GossipGenerator::SendPayload " << src << " -> " << dest << " Value:" << CurrentValue );
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");

  SentMessages++;
  Ipv4Header header = Ipv4Header ();
  header.SetDestination (dest);
  header.SetPayloadSize (0);
  header.SetSource (src);

  uint8_t NewPacketHops = (uint8_t) PacketHops + 1; // TODO no cast!

  uint8_t data[8];
  for (uint8_t j = 0; j < 8; j++)
  {
    data[j] = 0;
  }
  data[0] = (uint8_t) CurrentValue; // ONLY use first 8 bits to store data. // TODO May be extended...  
  data[1] = NewPacketHops;
  data[2] = seqNum;

  Ptr<Icmpv4L4Protocol> icmp = this->GetNode()->GetObject<Icmpv4L4Protocol>(); 
  icmp->SendData(header, data);
}

//void
//Gossip::SetPacketInterval (Time val) {
//  NS_LOG_FUNCTION (this);
//  packetInterval = val;
//}

void
Gossip::SetCurrentValue ( int val )
{
  NS_LOG_FUNCTION (this << val);
  CurrentValue = val;
  PacketHops = 0; // Fresh data -> no hops
  NS_LOG_INFO ("Value of node set to " << CurrentValue);
}

void
Gossip::SetSequenceNumber (int seq)
{
  NS_LOG_FUNCTION (this << seq);
  seqNum = seq;
  sourceNodePktStore.push_back(seq);
//  PrintSentPkt();
}

// Utility function for debug sourceNodePktStore variable
void
Gossip::PrintSentPkt () {
  std::cout << "Print sourceNodePktStore ..." << std::endl;
  for (unsigned int i = 0; i < sourceNodePktStore.size(); i++) {
    std::cout << sourceNodePktStore[i] << " " ;
  }
  std::cout << std::endl;
}

int
Gossip::GetPacketHops ( void )
{
  NS_LOG_FUNCTION (this);
  return PacketHops;
}

int
Gossip::GetSentMessages ( void )
{
  NS_LOG_FUNCTION (this);
  return SentMessages;
}

void
Gossip::StartApplication ( void )
{
  NS_LOG_FUNCTION (this);

//  Simulator::Schedule (gossip_delta_t, &GossipGenerator::GossipProcess2, this);
//  Simulator::Schedule (solicit_delta_t, &GossipGenerator::Solicit2, this);
}

void
Gossip::StopApplication ()
{
  NS_LOG_FUNCTION (this);
}

} // Namespace ns3
