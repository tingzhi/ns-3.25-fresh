/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *  Copyright (c) 2007,2008,2009 INRIA, UDCAST
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
 *
 * Author: Amine Ismail <amine.ismail@sophia.inria.fr>
 *                      <amine.ismail@udcast.com>
 */

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "packet-loss-counter.h"

#include "seq-ts-header.h"
#include "gossip-udp-server.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("GossipUdpServer");

NS_OBJECT_ENSURE_REGISTERED (GossipUdpServer);


TypeId
GossipUdpServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GossipUdpServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<GossipUdpServer> ()
    .AddAttribute ("Port",
                   "Port on which we listen for incoming packets.",
                   UintegerValue (100),
                   MakeUintegerAccessor (&GossipUdpServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketWindowSize",
                   "The size of the window used to compute the packet loss. This value should be a multiple of 8.",
                   UintegerValue (32),
                   MakeUintegerAccessor (&GossipUdpServer::GetPacketWindowSize,
                                         &GossipUdpServer::SetPacketWindowSize),
                   MakeUintegerChecker<uint16_t> (8,256))
  ;
  return tid;
}

GossipUdpServer::GossipUdpServer ()
  : m_lossCounter (0)
{
  NS_LOG_FUNCTION (this);
  m_received=0;
  m_SequenceNumber = 0;
  m_NodeNumber = 0;
  m_broadcastStatus = false;
}

GossipUdpServer::~GossipUdpServer ()
{
  NS_LOG_FUNCTION (this);
}

uint16_t
GossipUdpServer::GetPacketWindowSize () const
{
  NS_LOG_FUNCTION (this);
  return m_lossCounter.GetBitMapSize ();
}

void
GossipUdpServer::SetPacketWindowSize (uint16_t size)
{
  NS_LOG_FUNCTION (this << size);
  m_lossCounter.SetBitMapSize (size);
}

uint32_t
GossipUdpServer::GetLost (void) const
{
  NS_LOG_FUNCTION (this);
  return m_lossCounter.GetLost ();
}

uint32_t
GossipUdpServer::GetReceived (void) const
{
  NS_LOG_FUNCTION (this);
  return m_received;
}

void
GossipUdpServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}



void
GossipUdpServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
  {
    if (packet->GetSize () > 0)
    {
      SeqTsHeader seqTs;
      packet->RemoveHeader (seqTs);
      uint32_t currentSequenceNumber = seqTs.GetSeq ();
          
      if (currentSequenceNumber == m_SequenceNumber) {
//        std::cout << "Received Sequence Number is " << currentSequenceNumber << "\n";
//        std::cout << "Internal Sequence Number is " << m_SequenceNumber << "\n";
        if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("TraceDelay: RX " << packet->GetSize () <<
                       " bytes from "<< InetSocketAddress::ConvertFrom (from).GetIpv4 () <<
                       " Sequence Number: " << currentSequenceNumber <<
                       " Uid: " << packet->GetUid () <<
                       " TXtime: " << seqTs.GetTs () <<
                       " RXtime: " << Simulator::Now () <<
                       " Delay: " << Simulator::Now () - seqTs.GetTs ());
          
          m_StoreAck.push_back(InetSocketAddress::ConvertFrom (from).GetIpv4 ());
        }
        else if (Inet6SocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("TraceDelay: RX " << packet->GetSize () <<
                       " bytes from "<< Inet6SocketAddress::ConvertFrom (from).GetIpv6 () <<
                       " Sequence Number: " << currentSequenceNumber <<
                       " Uid: " << packet->GetUid () <<
                       " TXtime: " << seqTs.GetTs () <<
                       " RXtime: " << Simulator::Now () <<
                       " Delay: " << Simulator::Now () - seqTs.GetTs ());
        }
        m_lossCounter.NotifyReceived (currentSequenceNumber);
        m_received++;
      }
      else {
        // Do nothing...
        std::cout << "Gossip-udp-server didn't receive matching packet!!! \n"; 
        std::cout << "Received Sequence Number is " << currentSequenceNumber << "\n";
        std::cout << "Internal Sequence Number is " << m_SequenceNumber << "\n";
      }     
    }
  }
}

void
GossipUdpServer::SetSeqNum (uint32_t val){
  NS_LOG_FUNCTION (this);
  m_SequenceNumber = val;
}

void
GossipUdpServer::SetNumberOfNodes (uint32_t val) {
  NS_LOG_FUNCTION (this);
  m_NodeNumber = val;
}

std::vector<Ipv4Address> 
GossipUdpServer::GetStoreAck (void) {
  NS_LOG_FUNCTION (this);
  return m_StoreAck;
}

bool
GossipUdpServer::GetBroadcastStatus (void) {
  NS_LOG_FUNCTION (this);
  return m_broadcastStatus;
}

void
GossipUdpServer::CheckBroadcastStatus (void) {
  if (m_StoreAck.size() == m_NodeNumber) {
    m_broadcastStatus = true;
//    std::cout << "Ack vector content is: ";
//    for (uint32_t i = 0; i < m_StoreAck.size(); i++) {
//      std::cout << m_StoreAck[i] << " | ";
//    }
//    std::cout << "\n";
    m_StoreAck.clear();
  }
  else {
    m_broadcastStatus = false;
  }
  Simulator::Schedule (Seconds(0.5), &GossipUdpServer::CheckBroadcastStatus, this);
}


// This one is actually being used.
void
GossipUdpServer::CheckBroadcastStatus2 (void) {
  if ((m_StoreAck.size() == m_NodeNumber) && (IsUnique() == true)) {
    m_broadcastStatus = true;
//    std::cout << "Ack vector content is: ";
//    for (uint32_t i = 0; i < m_StoreAck.size(); i++) {
//      std::cout << m_StoreAck[i] << " | ";
//    }
//    std::cout << "\n";
    m_StoreAck.clear();
  } 
  else{
    m_broadcastStatus = false;
  }
  Simulator::Schedule (Seconds(0.5), &GossipUdpServer::CheckBroadcastStatus2, this);
}

bool
GossipUdpServer::IsUnique (void) {
  for (uint32_t i = 0; i < m_StoreAck.size(); i++) {
    for(uint32_t j = i+1; j < m_StoreAck.size(); j++){
      if(m_StoreAck[i] == m_StoreAck[j]){
        return false;
      }
      else {
        // Do nothing...
      }
    }
  }
  return true;
}

void
GossipUdpServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (),
                                                   m_port);
      m_socket->Bind (local);
    }

  m_socket->SetRecvCallback (MakeCallback (&GossipUdpServer::HandleRead, this));

  if (m_socket6 == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket6 = Socket::CreateSocket (GetNode (), tid);
      Inet6SocketAddress local = Inet6SocketAddress (Ipv6Address::GetAny (),
                                                   m_port);
      m_socket6->Bind (local);
    }

  m_socket6->SetRecvCallback (MakeCallback (&GossipUdpServer::HandleRead, this));
  
  Simulator::Schedule(Seconds(4.5), &GossipUdpServer::CheckBroadcastStatus2, this);

}

void
GossipUdpServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

} // Namespace ns3
