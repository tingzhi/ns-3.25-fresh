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
#include "gossip-generator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("GossipGeneratorApplication");

TypeId
GossipGenerator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::GossipGenerator")
    .SetParent<Application> ()
    .AddConstructor<GossipGenerator> ()
  ;
  return tid;
}

GossipGenerator::GossipGenerator ()
{
  NS_LOG_FUNCTION (this);
  isNew = false;
  CurrentValue = 0;
//  SentMessages = 0;
  SentAck = 0;
  SentSolicit = 0;
  SentPayload = 0;
  ReceivedData = Seconds(0);
  halt = false;
  gossip_delta_t = Seconds(0.001);
  solicit_delta_t = Seconds(5);
  x = CreateObject<UniformRandomVariable> ();
  
  y = CreateObject<UniformRandomVariable> ();
//  y->SetAttribute("Min", DoubleValue(0.0));
//  y->SetAttribute("Max", DoubleValue((double)neighbours[1].size()));
  // source = src;
}

GossipGenerator::~GossipGenerator ()
{
  NS_LOG_FUNCTION (this);
}

void
GossipGenerator::DoDispose ( void )
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
GossipGenerator::GetEnergySourceContainer (EnergySourceContainer sources)
{
  src = sources;
}

void
GossipGenerator::GetEnergySource (Ptr<EnergySource> testSrc) 
{
  srcPtr = testSrc;
//  std::cout << "Energy Node: " << this->GetNode()->GetId() << std::endl;
//  Ptr<EnergySource> source = src.Get(this->GetNode()->GetId());
////  this->GetNode()->GetObject<EnergySource>();
//  if (source == NULL) 
//    std::cout << "Error!!!!" << std::endl;
//  else 
//     energySource = source;
//  std::cout << "Here!!!!!" << std::endl;
//  std::cout << "Energy fraction of node " << this->GetNode()->GetId() << " is " << srcPtr->GetEnergyFraction() << "!!!!!!!" << std::endl;  
 }

unsigned int 
GossipGenerator::calFanout () {
  unsigned int fanout = 5;
  double remainingEnergyPercentage = srcPtr->GetEnergyFraction();
  
  if (remainingEnergyPercentage >= 0.8)
    fanout = 5;
  else if (remainingEnergyPercentage >= 0.6)
    fanout = 4;
  else if (remainingEnergyPercentage >= 0.4)
    fanout = 3;
  else if (remainingEnergyPercentage >= 0.2)
    fanout = 2;
  else
    fanout = 1;

//  std::cout << "Here!!!!" << std::endl;
//  std::cout << "Initial fanout was " << fanout << std::endl;
  //std::vector<neighbors>::iterator nb = neighborList.begin();
  //std::cout << "node " << nodeIndex << "'s neighborNode size is " << neighborList.at(nodeIndex).neighborNodes.size() << std::endl;

//  std::cout << "neighborlist size is " << nb->neighborNodes.size() << std::endl;
//  std::cout << "FANOUT: fanout for node " << this->GetNode()->GetId() << " is " << std::min(fanout, (unsigned int)neighbours[1].size()) << std::endl;
  unsigned int ret = (unsigned int) std::min(fanout, (unsigned int)neighbours[1].size());
  return ret;
//  return std::min(fanout, (int)neighbours[1].size());
}

/*
void GossipGenerator::printEnergyFraction (Ptr<EnergySource> source) {
  //Ptr<EnergySource> source = this->GetNode()->GetObject();
  std::cout << source->GetEnergyFraction() << std::endl;
}
*/

void
GossipGenerator::AddNeighbor(Ipv4Address own,Ipv4Address neighbor)
{ //TODO duplicates?
  neighbours[0].push_back(own);
  neighbours[1].push_back(neighbor);
  NS_LOG_INFO("Added to neighbors; New size: " << neighbours[0].size());
  /*
  for(uint8_t i=0; i<neighbours[0].size();i++){
    NS_LOG_INFO(neighbours[0].at(i));
    NS_LOG_INFO(" -> " << neighbours[1].at(i));
  }
  */
}

void
GossipGenerator::HandleAck(void)
{
  NS_LOG_INFO("GossipGenerator::HandleAck");
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");

  halt = true;
}

void
GossipGenerator::HandleAck2(void)
{
  NS_LOG_INFO("GossipGenerator::HandleAck");
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");

  isNew = false;
//  halt = true;
}

void
GossipGenerator::HandleSolicit(Ipv4Address src,Ipv4Address dest)
{
  NS_LOG_INFO("GossipGenerator::HandleSolicit " << src << " -> " << dest);
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");
  if (CurrentValue != 0)
  {
    SendPayload(dest,src);
  }
}

void
GossipGenerator::HandleSolicit2(Ipv4Address src,Ipv4Address dest)
{
  NS_LOG_INFO("GossipGenerator::HandleSolicit " << src << " -> " << dest);
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");
    
  SendPayload(dest,src);
}

void
GossipGenerator::HandlePayload(Ipv4Address src,Ipv4Address dest,uint8_t payload_in[])
{
  int payload = (int) payload_in[0];
  int hops = (int) payload_in[1];
  NS_LOG_INFO("GossipGenerator::HandlePayload " << src << " -> " << dest << " Value:" << payload);
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");
  if( payload == CurrentValue)
  {
    SendMessage(dest, src, TYPE_ACK);
  }
  else
  {
    CurrentValue = payload;
    PacketHops = hops;
    ReceivedData = Simulator::Now ();
  }
}

void
GossipGenerator::HandlePayload2(Ipv4Address src,Ipv4Address dest,uint8_t payload_in[])
{
  int payload = (int) payload_in[0];
  int hops = (int) payload_in[1];
  int seq = (int) payload_in[2];
  NS_LOG_INFO("GossipGenerator::HandlePayload " << src << " -> " << dest << " Value:" << payload);
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");
  
  if (rxPktStore.size() == 0 ) {
    if (seq != 0) {
      isNew = true;
      rxPktStore.push_back(seq);
      CurrentValue = payload;
      PacketHops = hops;
      ReceivedData = Simulator::Now ();
      seqNum = seq;
      StoreReceivedDataTime(isNew, ReceivedData);

    }
    else {
      isNew = false;
    }
  }
  else {
    if (seq == rxPktStore.back()) {
      isNew = false;
      SendMessage(dest, src, TYPE_ACK);
    }
    else {
      if (seq > rxPktStore.back()) {
        isNew = true;
        rxPktStore.push_back(seq);
        CurrentValue = payload;
        PacketHops = hops;
        ReceivedData = Simulator::Now ();
        seqNum = seq;
        StoreReceivedDataTime(isNew, ReceivedData);

      }
      else {
        isNew = false;
      }
    }
  }
}

void
GossipGenerator::StoreReceivedDataTime (bool newData, Time receivedDataTime) {
  NS_LOG_FUNCTION(this);
  if (newData) {
    rxDataTime.push_back(receivedDataTime.GetSeconds());
  }
  else {
    // Do nothing...
  }
}

std::vector<int>
GossipGenerator::ChooseNeighbors () {
  int in, im;
  int fanout = (int)calFanout();
  int neighborSize = (int)neighbours[1].size();
  
  im = 0;
  std::vector<int> vec;
 
  for (in = 0; in < neighborSize && im < fanout; ++in) {
    int rn = neighborSize - in;
    int rm = fanout - im;
    if ((unsigned int)(x->GetValue(0.0, (double)rn-1)) < (unsigned int)rm){
      //vec[im++] = in + 1;
      vec.push_back(in);
      im++;
    }
  }
  
//  std::cout << "CHOOSE NEIGHBORS: the random index vector is " << std::endl;
//  for (unsigned int i = 0; i < vec.size(); i++) {
//    std::cout << vec[i] << " ";
//  }
//  std::cout << std::endl;
  
  return vec;
}

void
GossipGenerator::ChooseRandomNeighbor(Ipv4Address ipv4array[2]){
  NS_LOG_INFO("ChooseRandomNeighbor from " << neighbours[0].size());
//  if ((unsigned int)(x->GetValue(0.0, (double)rn-1)) < (unsigned int)rm){
  //double temp = x->GetValue(0.0, (double)neighbours[1].size());
  uint32_t temp_rnd = y->GetInteger(0, neighbours[1].size()-1);
//  std::cout << "From Node " << this->GetNode()->GetId() << " CHOOSE_RANDOM_NEIGHBOR index " << temp_rnd << std::endl;
  //int temp_rnd = rand() % neighbours[0].size(); // Note: Seed, if desired! // TODO may not be random enough.
  ipv4array[0] = neighbours[0].at(0);
  ipv4array[1] = neighbours[1].at(temp_rnd);
}

void
GossipGenerator::SendMessage(Ipv4Address src, Ipv4Address dest, int type)
{
  NS_LOG_FUNCTION (this << src << dest << type);
  NS_LOG_INFO("GossipGenerator::SendMessage " << src << " -> " << dest << " Type:" << type);
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");

  SentMessages++;
  if (type == TYPE_ACK) {
    SentAck++;
  }
  else {
    SentSolicit++;
  }
  
  Ipv4Header header = Ipv4Header ();
  header.SetDestination (dest);
  header.SetPayloadSize (0);
  header.SetSource (src);
  
  Ptr<Icmpv4L4Protocol> icmp = this->GetNode()->GetObject<Icmpv4L4Protocol>();

  switch(type) {
    case TYPE_ACK : 
      icmp->SendAck(header);
      break;
    case TYPE_SOLICIT :
      icmp->SendRequest(header);
      break;
  }
}

void
GossipGenerator::SendPayload(Ipv4Address src, Ipv4Address dest)
{
  NS_LOG_FUNCTION (this << dest );
  NS_LOG_INFO("GossipGenerator::SendPayload " << src << " -> " << dest << " Value:" << CurrentValue );
  NS_LOG_INFO(" Time: " << Simulator::Now ().GetSeconds () << "s");

  SentPayload++;
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
  data[2] = (uint8_t) seqNum;

  Ptr<Icmpv4L4Protocol> icmp = this->GetNode()->GetObject<Icmpv4L4Protocol>(); 
  icmp->SendData(header, data);
}

void
GossipGenerator::SetCurrentValue ( int val )
{
  NS_LOG_FUNCTION (this << val);
  CurrentValue = val;
  PacketHops = 0; // Fresh data -> no hops
  NS_LOG_INFO ("Value of node set to " << CurrentValue);
}

void
GossipGenerator::SetSequenceNumber (int seq)
{
  NS_LOG_FUNCTION (this << seq);
  seqNum = seq;
  sourceNodePktStore.push_back(seq);
//  PrintSentPkt();
}

// Utility function for debug sourceNodePktStore variable
void
GossipGenerator::PrintSentPkt () {
  std::cout << "Print sourceNodePktStore ..." << std::endl;
  for (unsigned int i = 0; i < sourceNodePktStore.size(); i++) {
    std::cout << sourceNodePktStore[i] << " " ;
  }
  std::cout << std::endl;
}

/*
void 
GossipGenerator::SetEnergySource (Ptr<EnergySource> src){
  source = src;
}
*/

void
GossipGenerator::SetSolicitInterval ( Time val )
{
  NS_LOG_FUNCTION (this << val);
  solicit_delta_t = val;
}

void
GossipGenerator::SetGossipInterval ( Time val )
{
  NS_LOG_FUNCTION (this << val);
  gossip_delta_t = val;
}

std::vector<Ipv4Address> 
GossipGenerator::GetNeighbours (void)
{
  //return neighbours[0]; // return the own ipv4 address vector
  return neighbours[1];  // return neighbours ipv4 addresses vector
}

int
GossipGenerator::GetCurrentValue ( void )
{
  NS_LOG_FUNCTION (this);
  return CurrentValue;
}

int
GossipGenerator::GetPacketHops ( void )
{
  NS_LOG_FUNCTION (this);
  return PacketHops;
}

int
GossipGenerator::GetSentMessages ( void )
{
  NS_LOG_FUNCTION (this);
  return SentMessages;
}

int
GossipGenerator::GetSentSolicit ( void )
{
  NS_LOG_FUNCTION (this);
  return SentSolicit;
}

int
GossipGenerator::GetSentAck ( void )
{
  NS_LOG_FUNCTION (this);
  return SentAck;
}

int
GossipGenerator::GetSentPayload ( void )
{
  NS_LOG_FUNCTION (this);
  return SentPayload;
}

Time
GossipGenerator::GetReceivedDataTime ( void )
{
  NS_LOG_FUNCTION (this);
  return ReceivedData;
}

std::vector<double>
GossipGenerator::GetRxDataTime ( void )
{
  NS_LOG_FUNCTION (this);
  return rxDataTime;
}

std::vector<int>
GossipGenerator::GetRxPktStore ( void )
{
  NS_LOG_FUNCTION (this);
  return rxPktStore;
}

void
GossipGenerator::GossipProcess(void)
{
  if(!halt)
  {
    //reschedule
    Simulator::Schedule (gossip_delta_t, &GossipGenerator::GossipProcess,this);
    if (  CurrentValue != 0 )
    {
//      Ipv4Address ipv4array[2];
//      ChooseRandomNeighbor(ipv4array);
//      SendPayload(ipv4array[0],ipv4array[1]);
      std::vector<int> dest_vector;
      dest_vector = ChooseNeighbors();
      
//      std::cout << "dest_vector content" << std::endl;
      for (unsigned int i = 0; i < dest_vector.size(); i++) {
        std::cout << dest_vector[i] << " " ;
      }
      
//      std::cout << "GOSSIP PROCESS: The source node ip is " << neighbours[0].at(0) << std::endl;
      for (unsigned int i = 0; i < dest_vector.size(); i++) {
        SendPayload(neighbours[0].at(0), neighbours[1].at(dest_vector[i]));
//        std::cout << "GOSSIP PROCESS: The dest node ip is " << neighbours[1].at(dest_vector[i]) << std::endl;
      }
    }
  }
}

void
GossipGenerator::GossipProcess2(void)
{
  Simulator::Schedule (gossip_delta_t, &GossipGenerator::GossipProcess2,this);
  if (isNew)
  {
//      Ipv4Address ipv4array[2];
//      ChooseRandomNeighbor(ipv4array);
//      SendPayload(ipv4array[0],ipv4array[1]);
    std::vector<int> dest_vector;
    dest_vector = ChooseNeighbors();

//    std::cout << "dest_vector content" << std::endl;
//    for (unsigned int i = 0; i < dest_vector.size(); i++) {
//      std::cout << dest_vector[i] << " " ;
//    }

//    std::cout << "GOSSIP PROCESS: The source node ip is " << neighbours[0].at(0) << std::endl;
    for (unsigned int i = 0; i < dest_vector.size(); i++) {
      SendPayload(neighbours[0].at(0), neighbours[1].at(dest_vector[i]));
//      std::cout << "GOSSIP PROCESS: The dest node ip is " << neighbours[1].at(dest_vector[i]) << std::endl;
    }
  }
  else {
    // Do nothing...
  }
}

void
GossipGenerator::Solicit(void)
{
  if(CurrentValue == 0)
  {
    //reschedule
    Simulator::Schedule (solicit_delta_t, &GossipGenerator::Solicit,this);
    Ipv4Address ipv4array[2];
    ChooseRandomNeighbor(ipv4array);
    SendMessage(ipv4array[0],ipv4array[1], TYPE_SOLICIT);
  }
}

void
GossipGenerator::Solicit2(void)
{
  //reschedule
  Simulator::Schedule (solicit_delta_t, &GossipGenerator::Solicit2,this);
  Ipv4Address ipv4array[2];
  ChooseRandomNeighbor(ipv4array);
  SendMessage(ipv4array[0],ipv4array[1], TYPE_SOLICIT);
}

void
GossipGenerator::StartApplication ( void )
{
  NS_LOG_FUNCTION (this);

  Simulator::Schedule (gossip_delta_t, &GossipGenerator::GossipProcess2, this);
  Simulator::Schedule (solicit_delta_t, &GossipGenerator::Solicit2, this);
}

void
GossipGenerator::StopApplication ()
{
  NS_LOG_FUNCTION (this);
}

} // Namespace ns3
