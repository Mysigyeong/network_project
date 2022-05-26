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
#include "uno-card.h"
#include "uno-packet.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnoServerApplication");

NS_OBJECT_ENSURE_REGISTERED (UnoServer);

Uno unogame;
uint32_t ready_player;

long long seq_num;


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
                   MakeAddressAccessor (&UnoServer::m_clientAddress1),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress2", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress2),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress3", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress3),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress4", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress4),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress5", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress5),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress6", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress6),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress7", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress7),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress8", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress8),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress9", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress9),
                   MakeAddressChecker ())
    .AddAttribute ("ClientAddress10", 
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UnoServer::m_clientAddress10),
                   MakeAddressChecker ())
    .AddAttribute ("NumberOfClient",
                   "The number of clients",
                   UintegerValue (0),
                   MakeUintegerAccessor (&UnoServer::m_numOfSocket),
                   MakeUintegerChecker<uint32_t> ())
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
        if (InetSocketAddress::IsMatchingType (m_clientAddress[idx]) == true)
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

    m_clientAddress[0] = m_clientAddress1;
    m_clientAddress[1] = m_clientAddress2;
    m_clientAddress[2] = m_clientAddress3;
    m_clientAddress[3] = m_clientAddress4;
    m_clientAddress[4] = m_clientAddress5;
    m_clientAddress[5] = m_clientAddress6;
    m_clientAddress[6] = m_clientAddress7;
    m_clientAddress[7] = m_clientAddress8;
    m_clientAddress[8] = m_clientAddress9;
    m_clientAddress[9] = m_clientAddress10;

    for (uint32_t i = 0; i < m_numOfSocket; i++) {
        PrepareSocket(i);
    }

    m_sendEvent = Simulator::Schedule (Seconds(0.), &UnoServer::InitUno, this, 3);//여기에 맨뒤에 참가 인원 수
    m_sendEvent = Simulator::Schedule (Seconds(20.), &UnoServer::Send, this, 0);
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
    p = Create<Packet> (reinterpret_cast<uint8_t*>(&unoPacket), sizeof(unoPacket));
  
    //-------------------------------------------

    Address localAddress;
    m_socket[clientIdx]->GetSockName (localAddress);
    m_socket[clientIdx]->Send (p);

    if (InetSocketAddress::IsMatchingType (m_clientAddress[clientIdx]))
    {
        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << sizeof(unoPacket) << " bytes to client "
                     << clientIdx + 1 << " " << InetSocketAddress::ConvertFrom (m_clientAddress[clientIdx]).GetIpv4 ()
                     << " port " << InetSocketAddress::ConvertFrom (m_clientAddress[clientIdx]).GetPort ());
    }

    m_sendEvent = Simulator::Schedule (Seconds(1.), &UnoServer::Send, this, (clientIdx + 1) % m_numOfSocket);
}
void UnoServer::PacketRead(Ptr<Packet> packet)
{

  uint32_t gameop,uid;
  uint8_t *buf = new uint8_t[packet->GetSize()];
  cout<<endl;
  NS_LOG_INFO("packet payload, here is server");
  packet->CopyData(buf, packet->GetSize ());
  uint8_t *tempbuf=buf;
  string s = string(tempbuf,tempbuf+4);
  cout<<"sequence number: "<<uint32_t(s[0])<<endl;
  tempbuf=tempbuf+4;
  s = string(tempbuf,tempbuf+4); 
  cout<<"uid: "<<uint32_t(s[0])<<endl;
  tempbuf=tempbuf+4;
  uid=uint32_t(s[0]);
  s = string(tempbuf,tempbuf+4);
  cout<<"Game operation: "<<uint32_t(s[0])<<endl;
  tempbuf=tempbuf+4;

  gameop=uint32_t(s[0]);
   
  s = string(tempbuf,tempbuf+4);
  cout<<"User operation: "<<uint32_t(s[0])<<endl;
    
  //userop=uint32_t(s[0]);

  if(gameop==0){
    ready_player++;
    cout<<uid<<" player ready"<<endl;
    if(ready_player==unogame.player_No)
        cout<<"All player ready to start!!"<<endl;
  }

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
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());
          PacketRead(packet);
        }
      socket->GetSockName (localAddress);
    }


}

Ptr<Packet>
UnoServer::UnoPacketCreate(uint32_t uid)
{

    Ptr<Packet> p;

    UnoPacket up;
    up.seq=seq_num++;
    up.gameOp=GameOp::INIT;
    up.numOfCards=7;

    
    up.uid=uid;
    for(int j=0;j<7;j++)
    {
        up.cards[j]=unogame.Draw();
    }
    up.color=unogame.front.color;

    p = Create<Packet> (reinterpret_cast<uint8_t*>(&up), sizeof(up));
    
    return p;
}


void
UnoServer::InitUno(uint32_t num)
{
    seq_num=123;
    ready_player=0;
    unogame.turn=0;
    unogame.playing=0;
    unogame.player_No=num;
    unogame.front=unogame.Draw();
    for(uint32_t i=0;i<num;i++)
    {
        Address localAddress;
        m_socket[i]->GetSockName (localAddress);
        m_socket[i]->Send (UnoPacketCreate(i));

    }
}

} // Namespace ns3
