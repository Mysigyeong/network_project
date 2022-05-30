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


uint32_t this_uid;
Uno clientgame;

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

const char*
UnoClient::PrintColor (uint32_t color)
{
    switch (color) {
        case SPECIAL:
        return "special";
        case RED:
        return "red";
        case YELLOW:
        return "yellow";
        case BLUE:
        return "blue";
        case GREEN:
        return "green";
        default:
        return "invalid";
    }
}

Ptr<Packet>
UnoClient::Answer(Ptr<Packet> packet)
{
      //Variables Settting
      UnoPacket* uno_packet;
      Ptr<Packet> ret_packet;
      uint8_t *buf = new uint8_t[packet->GetSize()];
      cout<<"-----------------------------------------------------------"<<endl;
      NS_LOG_INFO("packet payload - Client ");
      packet->CopyData(buf, packet->GetSize());
      uno_packet=reinterpret_cast<UnoPacket*>(buf);
      this_uid = uno_packet->uid;

      //Pakcet Read
      cout<< "Sequence Number    : "<< uno_packet->seq <<endl;
      cout<< "uid                : "<< uno_packet->uid <<endl;
      cout<< "Game Operation     : "<< UnoClient::goptostring(uno_packet->gameOp)<<endl;
      
      //Cards on your hand
      
      switch(uno_packet->gameOp){
        case GameOp::INIT:
        cout<<"First Draw"<<endl;
        mycards.number=uno_packet->numOfCards;
        for(int i=0;i<int(uno_packet->numOfCards);i++){
          mycards.list.push_back(uno_packet->cards[i]);
          printCard(mycards.list.at(i));              
        }
        cout<<endl;

        cout<<"Are you ready to play?" << endl;
        ret_packet=packet;
        break;

        //Turn
        case GameOp::TURN:
        cout<<"Card List : "<<endl;
        for(int i=0;i<int(mycards.list.size());i++){
            printCard(mycards.list.at(i));              
        }
        cout<<endl;

        cout<<"Front Card : ";
        printCard(uno_packet->frontcard);
        cout<<endl;

        if (uno_packet->frontcard.color == SPECIAL) {
          cout << "Previous player selected " << PrintColor(uno_packet->color) << endl;
        }

        ret_packet=CreateReactionPacket(*uno_packet);
        break;

        case GameOp::UNODRAW:
        case GameOp::DRAW:
        cout<<"----------Draw Card----------"<<endl;
        cout<<"Card List : "<<endl;
        for (uint32_t i = 0; i < uno_packet->numOfCards; i++) {
          mycards.list.push_back(uno_packet->cards[i]);
          mycards.number++;
        }
        for(int i=0;i<int(mycards.number);i++){
            printCard(mycards.list.at(i));              
        }
        cout<<endl;

        ret_packet=CreateReactionPacket(*uno_packet);
        break;

        //UNO
        case GameOp::UNO:
        cout << this_uid << "th player say uno!" << endl;
        ret_packet = CreateReactionPacket(*uno_packet);
        break;

        //GAMEOVER
        case GameOp::GAMEOVER:
        cout << this_uid << "th player end game" << endl;
        ret_packet=CreateReactionPacket(*uno_packet);
        break;

        default:
        break;
      }
      return ret_packet;
}

const char*
UnoClient::goptostring(GameOp gop){
  switch(gop){
    case GameOp::INIT:      return "INIT";
    case GameOp::TURN:      return "TURN";
    case GameOp::DRAW:      return "DRAW";
    case GameOp::UNO:       return "UNO";
    case GameOp::UNODRAW:   return "UNODRAW";
    case GameOp::GAMEOVER:  return "GAMEOVER";
    default:                return "Error";
  }
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
          cout<<"-----------------------------------------------------------"<<endl;
          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s client sent " << packet->GetSize () << " bytes to " <<
                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
                       InetSocketAddress::ConvertFrom (from).GetPort () << endl << endl);
        }
    }
}

void
UnoClient::printCard(card card)
{
  switch(card.color)
  {
    case SPECIAL:
    cout<<"(Special, ";
    break;

    case RED:
    cout<<"(Red, ";
    break;

    case YELLOW:
    cout<<"(Yellow, ";
    break;

    case BLUE:
    cout<<"(Blue, ";
    break;

    case GREEN:
    cout<<"(Green, ";
    break;

    default:
    break;
  }

  switch(card.number)
  {
    case SKIP:
    cout<<"Skip) ";
    break;

    case REVERSE:
    cout<<"Reverse) ";
    break;

    case DRAW_TWO:
    cout<<"Draw Two) ";
    break;

    case WILD:
    cout<<"Wild) ";
    break;

    case WILD_DRAW_FOUR:
    cout<<"Wild & Draw Four) ";
    break;

    default:
    cout<<card.number<<") ";
    break;
  }    
}

Ptr<Packet>
UnoClient::CreateReactionPacket(UnoPacket recv_packet)
{
    Ptr<Packet> p;
    UnoPacket up;
    card deck_front=recv_packet.frontcard;
    
    switch (recv_packet.gameOp) {
      case GameOp::TURN: {
      bool pass = false;
      int pass_index = 0;

      // If the front card is special card, check the selected color.
      if (deck_front.color == SPECIAL) {
        for(int i=0;i<int(mycards.list.size());i++){
          if(recv_packet.color == mycards.list.at(i).color || mycards.list.at(i).color == SPECIAL) {
            pass_index = i;
            pass = true;
            up.passingcard = mycards.list.at(i);
            break;
          }
        }
      }
      else {
        for(int i=0;i<int(mycards.list.size());i++){
          if(deck_front.color == mycards.list.at(i).color || deck_front.number == mycards.list.at(i).number || mycards.list.at(i).color == SPECIAL) {
            pass_index = i;
            pass = true;
            up.passingcard = mycards.list.at(i);
            break;
          }
        }
      }
      
      if(pass){
        //내 손에서 카드 제거
        mycards.list.erase(mycards.list.begin()+pass_index);
        mycards.number=mycards.list.size();
        //UserOp는 Play
        up.userOp=UserOp::PLAY;
	      up.gameOp=GameOp::TURN;

        if (up.passingcard.color == SPECIAL) {
          std::random_device rd;
          std::mt19937 gen(rd());
          std::uniform_int_distribution<int> dis(1, 4);
          up.color = dis(gen);
        }
      }
      else{
        //UserOp는 Draw
        up.userOp=UserOp::DRAW;
	      up.gameOp=GameOp::TURN;
      }
      up.seq=recv_packet.seq;
      up.uid=recv_packet.uid;
      //up.gameOp=recv_packet.gameOp;
      up.numOfCards=mycards.number;

      if(pass) {
	      cout << "Action : PLAY, Card : ";
        printCard(up.passingcard);
        cout << endl;
        if (up.passingcard.color == SPECIAL) {
          cout << "select " << PrintColor(up.color) << endl;
        }
      }
      else {
	      cout << "Action : Draw" << endl;
      }
      break;
      }

      case GameOp::DRAW:
      case GameOp::UNODRAW:
      case GameOp::UNO:
      case GameOp::GAMEOVER:
      up.gameOp=recv_packet.gameOp;
      up.uid=recv_packet.uid;
      up.seq=recv_packet.seq;
      break;

      default:
      break;
    }

    p = Create<Packet> (reinterpret_cast<uint8_t*>(&up), sizeof(up));
    
    return p;
}

} // Namespace ns3

