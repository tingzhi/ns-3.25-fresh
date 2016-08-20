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
  
  CurrentValue = 2;  
  seqNum = 1;  // start at 1
//  pktNum = 1;
  
//  SentMessages = 0;
  PacketHops = 0;
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

void
Gossip::SendPayload(Ipv4Address src, Ipv4Address dest)
{
  NS_LOG_FUNCTION (this << dest );
  NS_LOG_INFO("Gossip::SendPayload " << src << " -> " << dest << " Value:" << CurrentValue );
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");

//  SentMessages++;
  Ipv4Header header = Ipv4Header ();
  header.SetDestination (dest);
  header.SetPayloadSize (0);
  header.SetSource (src);

//  uint8_t NewPacketHops = (uint8_t) PacketHops + 1; // TODO no cast!
  uint8_t NewPacketHops = PacketHops + 1;

  uint8_t data[8];
  for (uint8_t j = 0; j < 8; j++)
  {
    data[j] = 0;
  }
  data[0] = CurrentValue; // ONLY use first 8 bits to store data. // TODO May be extended...  
  data[1] = NewPacketHops;
  
  uint8_t high, low;
  uint16_t lowTemp;
  high = uint8_t(seqNum >> 8);
  lowTemp = seqNum << 8;
  low = uint8_t(lowTemp >> 8);
  
  data[2] = high;
  data[3] = low;
  
//  data[2] = seqNum;

  Ptr<Icmpv4L4Protocol> icmp = this->GetNode()->GetObject<Icmpv4L4Protocol>(); 
  icmp->SendData(header, data);
  sentPktTime.push_back(Simulator::Now().GetSeconds());
}

void
Gossip::SetCurrentValue ( int val )
{
  NS_LOG_FUNCTION (this << val);
  CurrentValue = val;
  PacketHops = 0; // Fresh data -> no hops
  NS_LOG_INFO ("Value of node set to " << CurrentValue);
}

void
Gossip::SetSequenceNumber (uint16_t val)
{
  NS_LOG_FUNCTION (this << val);
  seqNum = val;
//  sourceNodePktStore.push_back(seq);
//  PrintSentPkt();
}

// Utility function for debug sourceNodePktStore variable
//void
//Gossip::PrintSentPkt () {
//  std::cout << "Print sourceNodePktStore ..." << std::endl;
//  for (unsigned int i = 0; i < sourceNodePktStore.size(); i++) {
//    std::cout << sourceNodePktStore[i] << " " ;
//  }
//  std::cout << std::endl;
//}

std::vector<double>
Gossip::GetSentPktTime (void) 
{
  NS_LOG_FUNCTION (this);
  return sentPktTime;
}

int
Gossip::GetPacketHops ( void )
{
  NS_LOG_FUNCTION (this);
  return PacketHops;
}

//int
//Gossip::GetSentMessages ( void )
//{
//  NS_LOG_FUNCTION (this);
//  return SentMessages;
//}

void 
Gossip::SetSourceNode (NodeContainer c) {
  NS_LOG_FUNCTION (this);
  m_sourceNodes = c;
}

void 
Gossip::SetNumberOfNodes (uint32_t val) {
  NS_LOG_FUNCTION (this);
  numOfNodes = val;
}

void
Gossip::SetUdpServer (Ptr<GossipUdpServer> val) {
  udpServer = val;
}

Ipv4Address
Gossip::GetIpv4 (Ptr<Node> node, uint32_t index) {
  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> (); // Get Ipv4 instance of the node
  Ipv4Address addr = ipv4->GetAddress (index, 0).GetLocal (); // Get Ipv4InterfaceAddress of xth interface.
  
  return addr;
}

void
Gossip::GeneratePackets (void) 
{ 
  NS_LOG_FUNCTION (this);

  Ipv4Address srcAddr, destAddr;
  srcAddr = GetIpv4(m_sourceNodes.Get(numOfNodes), 1);
  destAddr = GetIpv4(m_sourceNodes.Get(0), 2);  //  destAddr = GetIpv4(wifiNodes.Get(0), 2);

  std::cout << "srcAddr " << srcAddr << std::endl;
  std::cout << "destAddr " << destAddr << std::endl;

  // pass # of nodes, sequence number info to udp server
  udpServer->SetNumberOfNodes(numOfNodes);
  udpServer->SetSeqNum((uint32_t)seqNum);

  std::cout << "GeneratePackets Function! seqtNum " <<  seqNum << std::endl;
  SendPayload(srcAddr, destAddr);
  seqNum++;
}

void
Gossip::ControlTraffic (void) {
  Simulator::Schedule (Seconds(1.0), &Gossip::ControlTraffic, this);

  bool flag = udpServer->GetBroadcastStatus();
  if (flag == true) {
    // send out a new packet
    GeneratePackets(); 
  }
  else {
    // Do nothing...
  }

}

void
Gossip::StartApplication ( void )
{
  NS_LOG_FUNCTION (this);

//  Simulator::Schedule (gossip_delta_t, &GossipGenerator::GossipProcess2, this);
//  Simulator::Schedule (solicit_delta_t, &GossipGenerator::Solicit2, this);
  Simulator::Schedule(Seconds(3.0), &Gossip::GeneratePackets, this);
  Simulator::Schedule(Seconds(4.0), &Gossip::ControlTraffic, this);
}

void
Gossip::StopApplication ()
{
  NS_LOG_FUNCTION (this);
}

} // Namespace ns3
