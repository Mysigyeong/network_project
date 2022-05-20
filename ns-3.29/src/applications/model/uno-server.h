/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
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

#ifndef UNO_SERVER_H
#define UNO_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"

#include "uno-packet.h"

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup udpecho
 * \brief A Uno client
 *
 * Every packet sent should be returned by the server and received here.
 */
class UnoServer : public Application 
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  UnoServer ();

  virtual ~UnoServer ();


protected:
  virtual void DoDispose (void);

private:
  virtual void PrepareSocket (uint32_t idx);

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Send a packet to clientIdx
   */
  void Send (uint32_t clientIdx);

  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);

  uint32_t m_numOfSocket; //!< Number of socket ; max 10
  Ptr<Socket> m_socket[10]; //!< Socket
  Address m_clientAddress[10]; //!< Remote peer address
  uint16_t m_clientPort[10]; //!< Remote peer port
  EventId m_sendEvent; //!< Event to send the next packet

  /// Callbacks for tracing the packet Tx events
  TracedCallback<Ptr<const Packet> > m_txTrace;

  /// Callbacks for tracing the packet Rx events
  TracedCallback<Ptr<const Packet> > m_rxTrace;
  
  /// Callbacks for tracing the packet Tx events, includes source and destination addresses
  TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_txTraceWithAddresses;
  
  /// Callbacks for tracing the packet Rx events, includes source and destination addresses
  TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_rxTraceWithAddresses;

};

} // namespace ns3

#endif /* UNO_SERVER_H */
