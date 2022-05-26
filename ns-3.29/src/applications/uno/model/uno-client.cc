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
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"

#include "uno-client.h"
#include <string>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UnoClientApplication");

NS_OBJECT_ENSURE_REGISTERED (UnoClient);

TypeId
UnoClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UnoClient")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<UnoClient> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&UnoClient::m_port),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

UnoClient::UnoClient ()
{
  NS_LOG_FUNCTION (this);
}

UnoClient::~UnoClient()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
}

void
UnoClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
UnoClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      if (m_socket->Bind (local) == -1)
        {
          NS_FATAL_ERROR ("Failed to bind socket");
        }
      if (addressUtils::IsMulticast (m_local))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, m_local);
            }
          else
            {
              NS_FATAL_ERROR ("Error: Failed to join multicast group");
            }
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&UnoClient::HandleRead, this));
}

void 
UnoClient::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

Ptr<Packet>
UnoClient::Answer(Ptr<Packet> packet)
{
      uint32_t gameop;
      uint8_t *buf = new uint8_t[packet->GetSize()];
      Ptr<Packet> ret_packet;
      cout<<endl;
      NS_LOG_INFO("packet payload ");

      packet->CopyData(buf, packet->GetSize ());
      uint8_t *tempbuf=buf;
      string s = string(tempbuf,tempbuf+4);
      cout<<"sequence number: "<<uint32_t(s[0])<<endl;
      tempbuf=tempbuf+4;
      s = string(tempbuf,tempbuf+4);
      cout<<"uid: "<<uint32_t(s[0])<<endl;
      tempbuf=tempbuf+4;
      
      s = string(tempbuf,tempbuf+4);
      cout<<"Game operation: "<<uint32_t(s[0])<<endl;
      tempbuf=tempbuf+4;
      
      gameop=uint32_t(s[0]);
      
      s = string(tempbuf,tempbuf+4);
      //cout<<"User operation: "<<uint32_t(s[0])<<endl; 이 부분은 server에 추가
      tempbuf=tempbuf+4;
      
      s = string(tempbuf,tempbuf+4);
      cout<<"The number of cards: "<<uint32_t(s[0])<<endl;
      tempbuf=tempbuf+4;
      
      for(int i=0;i<7;i++){
      
        s = string(tempbuf,tempbuf+4);
        switch(uint32_t(s[0]))
        {
            case 0:
                cout<<i+1<<" Special card ";
                break;
            case 1:
                cout<<i+1<<" card color is Red";
                break;
            case 2:
                cout<<i+1<<" card color is Yellow";
                break;
            case 3:
                cout<<i+1<<" card color is Blue";
                break;
            case 4:
                cout<<i+1<<" card color is Green";
                break;
            default:
                break;
        }
        
        tempbuf=tempbuf+4;
        s = string(tempbuf,tempbuf+4);
        if(uint32_t(s[0])>9)
        {
            switch(uint32_t(s[0]))
            {
                case 10:
                    cout<<" Skip!! "<<endl;
                    break;
                case 11:
                    cout<<" Reverse!! "<<endl;
                    break;
                case 12:
                    cout<<" Draw Two!! "<<endl;
                    break;
                case 13:
                    cout<<" Wild!! "<<endl;
                    break;
                case 14:
                    cout<<" Wild & Draw Four!! "<<endl;
                    break;
                default:
                    break;
            }
        }
        else
            cout<<" and number is "<<uint32_t(s[0])<<endl;
        tempbuf=tempbuf+4;
      }

      s = string(tempbuf+4,tempbuf+8);
      cout<<"The color of the front card: "<<uint32_t(s[0])<<endl;
      tempbuf=tempbuf+4;
      s = string(tempbuf,tempbuf+4);
      cout<<"The number of the front card: "<<uint32_t(s[0])<<endl<<endl;
      
      
      switch(gameop){
        case 0:
            cout<<"You ready to play?"<<endl;
            ret_packet=packet;
            break;

        default:
            break;


      }
      cout<<endl;

      return ret_packet;
}

void 
UnoClient::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  Address localAddress;
  while ((packet = socket->RecvFrom (from)))
    {
      socket->GetSockName (localAddress);
      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client received " << packet->GetSize () << " bytes from " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());
        }

      
      packet=Answer(packet);
      
      packet->RemoveAllPacketTags ();
      packet->RemoveAllByteTags ();


      NS_LOG_LOGIC ("Ack packet");

      socket->SendTo (packet, 0, from);

      if (InetSocketAddress::IsMatchingType (from))
        {
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << packet->GetSize () << " bytes to " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort ());
        }
    }
}

} // Namespace ns3
