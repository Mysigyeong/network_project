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
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"

#include "uno-server.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnoServerApplication");

NS_OBJECT_ENSURE_REGISTERED (UnoServer);

TypeId
UnoServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnoServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<UnoServer> ()
    .AddAttribute ("ClientAddress1", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[0])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress2", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[1])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress3", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[2])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress4", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[3])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress5", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[4])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress6", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[5])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress7", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[6])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress8", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[7])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress9", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[8])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress10", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&(UnoServer::m_clientAddress[9])),
                   MakeAddressChecker ())
    .AddAttribute ("ClientPort1", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[0])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort2", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[1])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort3", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[2])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort4", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[3])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort5", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[4])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort6", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[5])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort7", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[6])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort8", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[7])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort9", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[8])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("ClientPort10", 
                   "The destination port of the outbound packets",
                   UintegerValue (),
                   MakeUintegerAccessor (&(UnoServer::m_clientPort[9])),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("NumberOfClient",
                   "The number of clients",
                   UintegerValue (0),
                   MakeUintegerAccessor (&UnoServer::m_numOfSocket),
                   MakeUintegerChecker<uint32_t> ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&UnoServer::m_txTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("Rx", "A packet has been received",
                     MakeTraceSourceAccessor (&UnoServer::m_rxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("TxWithAddresses", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&UnoServer::m_txTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
    .AddTraceSource ("RxWithAddresses", "A packet has been received",
                     MakeTraceSourceAccessor (&UnoServer::m_rxTraceWithAddresses),
                     "ns3::Packet::TwoAddressTracedCallback")
  ;
  return tid;
}

UnoServer::UnoServer ()
{
    NS_LOG_FUNCTION (this);
    for (uint32_t i = 0; i < 10; i++) {
        m_socket[i] = 0;
    }
    m_sendEvent = EventId ();
}

UnoServer::~UnoServer()
{
    NS_LOG_FUNCTION (this);
    for (uint32_t i = 0; i < 10; i++) {
        m_socket[i] = 0;
    }
}


void
UnoServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
UnoServer::PrepareSocket (uint32_t idx)
{
    NS_LOG_FUNCTION (this);
    
    if (m_socket[idx] == 0)
    {
        TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
        m_socket[idx] = Socket::CreateSocket (GetNode (), tid);
        if (Ipv4Address::IsMatchingType(m_clientAddress[idx]) == true)
        {
            if (m_socket[idx]->Bind () == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
            m_socket[idx]->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_clientAddress[idx]), m_clientPort[idx]));
        }
        else if (InetSocketAddress::IsMatchingType (m_clientAddress[idx]) == true)
        {
            if (m_socket[idx]->Bind () == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
            m_socket[idx]->Connect (m_clientAddress[idx]);
        }
        else
        {
            NS_ASSERT_MSG (false, "Incompatible address type: " << m_clientAddress[idx]);
        }
    }

    m_socket[idx]->SetRecvCallback (MakeCallback (&UnoServer::HandleRead, this));
    m_socket[idx]->SetAllowBroadcast (true);
}

void 
UnoServer::StartApplication (void)
{
    NS_LOG_FUNCTION (this);

    for (uint32_t i = 0; i < m_numOfSocket; i++) {
        PrepareSocket(i);
    }

    

    m_sendEvent = Simulator::Schedule (Seconds(0.), &UnoServer::Send, this, 0);
}

void 
UnoServer::StopApplication (void)
{
    NS_LOG_FUNCTION (this);

    for (uint32_t i = 0; i < m_numOfSocket; i++) {
        if (m_socket[i] != 0) 
        {
            m_socket[i]->Close ();
            m_socket[i]->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
            m_socket[i] = 0;
        }
    }

    Simulator::Cancel (m_sendEvent);
}


void 
UnoServer::Send (uint32_t clientIdx)
{
    NS_LOG_FUNCTION (this);

    NS_ASSERT (m_sendEvent.IsExpired ());

    Ptr<Packet> p;

    //--------------------------------------------

    UnoPacket unoPacket;

    // Setting unoPacket
    
    p = Create<Packet> (static_cast<uint8_t*>&unoPacket, sizeof(unoPacket));
  
    //-------------------------------------------

    Address localAddress;
    m_socket[clientIdx]->GetSockName (localAddress);
    // call to the trace sinks before the packet is actually sent,
    // so that tags added to the packet can be sent as well
    m_txTrace (p);
    if (Ipv4Address::IsMatchingType (m_clientAddress[clientIdx]))
    {
        m_txTraceWithAddresses (p, localAddress, InetSocketAddress (Ipv4Address::ConvertFrom (m_clientAddress[clientIdx]), m_clientPort[clientIdx]));
    }
    m_socket[clientIdx]->Send (p);

    if (Ipv4Address::IsMatchingType (m_clientAddress[clientIdx]))
    {
        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_size << " bytes to " <<
                   Ipv4Address::ConvertFrom (m_clientAddress[clientIdx]) << " port " << m_clientPort[clientIdx]);
    }
    else if (InetSocketAddress::IsMatchingType (m_clientAddress[clientIdx]))
    {
        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << m_size << " bytes to " <<
                   InetSocketAddress::ConvertFrom (m_clientAddress[clientIdx]).GetIpv4 () << " port " << InetSocketAddress::ConvertFrom (m_clientAddress[clientIdx]).GetPort ());
    }

    m_sendEvent = Simulator::Schedule (Seconds(0.), &UnoServer::Send, this);
}

void
UnoServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
    {
      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client received " << packet->GetSize () << " bytes from " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());
        }
      socket->GetSockName (localAddress);
      m_rxTrace (packet);
      m_rxTraceWithAddresses (packet, from, localAddress);
    }
}

} // Namespace ns3
