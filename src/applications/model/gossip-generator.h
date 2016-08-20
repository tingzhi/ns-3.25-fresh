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

#ifndef GOSSIP_GENERATOR_H
#define GOSSIP_GENERATOR_H

#include <vector>

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/energy-module.h"
#include "ns3/gossip-udp-client.h"

namespace ns3 {
  const uint8_t TYPE_SOLICIT = 21; //!< constant used to indicate type of msg
  const uint8_t TYPE_ACK     = 23; //!< constant used to indicate type of msg

/**
 * \ingroup 
 * \class GossipGenerator
 *
 * \brief An application implementing the gossip protocol to communicate
 */
class GossipGenerator : public Application
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
  GossipGenerator ();

  /**
   * \brief Constructor.
   */
  virtual ~GossipGenerator ();

  /**
   * \brief add a neighbor
   * \param ip of the neighbor
   */
  void AddNeighbor( Ipv4Address own,Ipv4Address neighbor );

  /**
   * \brief Find a random neigbor
   * \param Store a random source and neighbor address in this array
   */
  void ChooseRandomNeighbor(Ipv4Address neighbor[2]);

  /**
   * \brief Seed the internal value of a node to initiate the gossip.
   * \param The internal value.
   */
  void SetCurrentValue (int val);

  /**
   * \brief Set the time between two sent messages.
   * \param The time.
   */
  void SetGossipInterval ( Time val );

  /**
   * \brief Set the time between two requests.
   * \param The time.
   */
  void SetSolicitInterval ( Time val );

  /**
   * \brief Return the current internal value.
   * \return The value
   */ 
  int GetCurrentValue ( void);

  /**
   * \brief Return the amount of icmp Messages sent.
   * \return The value
   */
  int GetSentMessages ( void);

  /**
   * \brief Return the number of hops used.
   * \return The value
   */
  int GetPacketHops ( void);

  /**
   * \brief Return the time used until the data was received.
   * \return The value
   */
  Time GetReceivedDataTime ( void);

  //void SendMessage_debug(Ipv4Address src, Ipv4Address dest, int type);

  /**
   * \brief Accept and process a received Ack.
   */
  void HandleAck(void);

  /**
   * \brief Accept and process a request message.
   * \param the source address
   * \param the destination address
   */
  void HandleSolicit(Ipv4Address src,Ipv4Address dest);

  /**
   * \brief Handle payload
   * \param the source address
   * \param the destination address
   * \param the payload
   */
  void HandlePayload(Ipv4Address src,Ipv4Address dest,uint8_t payload[]);
  
/************************************************************************/
  
  void GetEnergySource (Ptr<EnergySource> testSrc); 
  unsigned int calFanout (); 
  std::vector<int> ChooseNeighbors ();
  void PrintSentPkt ();
  // void printEnergyFraction (Ptr<EnergySource> source);
  void GetEnergySourceContainer (EnergySourceContainer sources);
  void SetSequenceNumber (int seq);
  //void SetEnergySource (Ptr<EnergySource> src);
  std::vector<Ipv4Address> GetNeighbours (void);
  void GossipProcess2(void);
  void Solicit2(void);
  void HandleAck2(void);
  void HandleSolicit2(Ipv4Address src,Ipv4Address dest);
  void HandlePayload2(Ipv4Address src,Ipv4Address dest,uint8_t payload_in[]);
  void StoreReceivedDataTime (bool newData, Time receivedDataTime);
  int GetSentPayload ( void );
  int GetSentSolicit ( void );
  int GetSentAck ( void );
  std::vector<int> GetRxPktStore (void);
  
  void GetUdpClient (Ptr<GossipUdpClient> udpClientApp);


  std::vector<double> GetRxDataTime ();
  
  EnergySourceContainer src;
  Ptr<EnergySource> energySource;
  Ptr<EnergySource> srcPtr;
  Ptr<GossipUdpClient> udpClient;
  Ptr<UniformRandomVariable> x;
  Ptr<UniformRandomVariable> y; // used for ChooseRandomNeighbor function
  
/************************************************************************/

protected:
    
  int CurrentValue; //!< The current Value
  bool halt; //!< If the gossip is paused
  Time gossip_delta_t; //!< Time between sending data to the other nodes
  Time solicit_delta_t; //!< Time between requests of data from the other nodes
  Time ReceivedData; //!< Time when data was first received
  int SentMessages; //!< Amount of messages sent out
  int PacketHops; //!< How many hops the data packet experienced
  std::vector<Ipv4Address> neighbours[2]; //!< The own addresses and corresponding neighbors of this node
  //Ptr<EnergySource> source;
  int seqNum;
  std::vector<int> sourceNodePktStore;
  std::vector<int> rxPktStore;
  bool isNew;
  std::vector<double> rxDataTime;
  int SentPayload;
  int SentAck;
  int SentSolicit;

  
  
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
  void SendMessage(Ipv4Address src, Ipv4Address dest, int type);

  /**
   * \brief Send the current value to the given destination
   * \param the source
   * \param the destination
   */
  void SendPayload(Ipv4Address src, Ipv4Address dest);
  
private:
  /**
   * \brief Start the GossipProcess
   */
  void GossipProcess();

  /**
   * \brief Start the SolicitProcess
   */
  void Solicit(void);

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

#endif // GOSSIP_GENERATOR_H
