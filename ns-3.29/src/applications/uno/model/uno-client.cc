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
              printCard(mycards.list.at(i), i);              
            }
            cout<<endl;

            cout<<"You ready to play?" << endl;
            ret_packet=packet;
            break;

        //Turn
        case GameOp::TURN:
          cout<<"Card List : "<<endl;
          for(int i=0;i<int(mycards.list.size());i++){
              printCard(mycards.list.at(i), i);              
          }
          cout<<endl;

          cout<<"Front Card : "<<"("<<uno_packet->frontcard.color<<","<<uno_packet->frontcard.number<<")"<<endl;
          ret_packet=CreateReactionPacket(*uno_packet);
          break;

        case GameOp::DRAW:
          cout<<"----------Draw New Card----------"<<endl;
          cout<<"Card List : "<<endl;
          mycards.list.push_back(uno_packet->passingcard);
          mycards.number++;
          for(int i=0;i<int(mycards.number);i++){
              printCard(mycards.list.at(i), i);              
          }
          cout<<endl;

          ret_packet=CreateReactionPacket(*uno_packet);

        //PENALTY
        case GameOp::PENALTY:
	  if(uno_packet->passingcard.number==10)
	  {
		cout<<"---------You are blocked!!!---------"<<endl;
		ret_packet=CreateReactionPacket(*uno_packet);
	  }
	  if(uno_packet->passingcard.number==12)
	  {
		cout<<"---------You will get two cards!!!---------"<<endl;
	  	cout<<"Card List : "<<endl;
		mycards.list.push_back(uno_packet->cards[0]);
	  	mycards.list.push_back(uno_packet->cards[1]);
	  	mycards.number+=2;
		for(int i=0;i<int(mycards.number);i++){
			printCard(mycards.list.at(i),i);
		}
		cout<<endl;

		ret_packet=CreateReactionPacket(*uno_packet);
	  }
	  if(uno_packet->passingcard.number==14)
	  {
		cout<<"---------You will get four cards!!!-------"<<endl;
                cout<<"Card List : "<<endl;
		mycards.list.push_back(uno_packet->cards[0]);
                mycards.list.push_back(uno_packet->cards[1]);
		mycards.list.push_back(uno_packet->cards[2]);
                mycards.list.push_back(uno_packet->cards[3]);
                mycards.number+=4;
                for(int i=0;i<int(mycards.number);i++){
                        printCard(mycards.list.at(i),i);
                }
                cout<<endl;

		cout<<"The color is changed to "<<uno_packet->color<<endl;

                ret_packet=CreateReactionPacket(*uno_packet);
	  }
          break;


        //UNO
        case GameOp::UNO:
          cout << this_uid << "th player say uno!" << endl;
          ret_packet = CreateReactionPacket(*uno_packet);
          break;


        //GAMEOVER
        case GameOp::GAMEOVER:
          cout << this_uid << "th player end game";
          ret_packet=CreateReactionPacket(*uno_packet);
          break;

        //WAIT
        case GameOp::WAIT:
            cout<<"It's "<<uno_packet->playing<<"s turn! Wait"<<endl;
            cout<<"Front Card : "<<"("<<uno_packet->frontcard.color<<","<<uno_packet->frontcard.number<<")"<<endl;
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
    case GameOp::PENALTY:   return "PENALTY";
    case GameOp::UNO:       return "UNO";
    case GameOp::GAMEOVER:  return "GAMEOVER";
    case GameOp::WAIT:      return "WAIT";
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
UnoClient::printCard(card card, int index)
{
  switch(card.color)
  {
      case 0:
          cout<<"(Special, ";
          break;
      case 1:
          cout<<"(Red, ";
          break;
      case 2:
          cout<<"(Yellow, ";
          break;
      case 3:
          cout<<"(Blue, ";
          break;
      case 4:
          cout<<"(Green, ";
          break;
      default:
          break;
  }
        

  if(card.number>9)
  {
      switch(card.number)
      {
          case 10:
              cout<<"Skip) ";
              break;
          case 11:
              cout<<"Reverse) ";
              break;
          case 12:
              cout<<"Draw Two) ";
              break;
          case 13:
              cout<<"Wild) ";
              break;
          case 14:
              cout<<"Wild & Draw Four) ";
              break;
          default:
              break;
      }
  }
  else
  {
    cout<<card.number<<") ";
  }

  return;
}

Ptr<Packet>
UnoClient::CreateReactionPacket(UnoPacket recv_packet)
{
    Ptr<Packet> p;
    UnoPacket up;
    card deck_front=recv_packet.frontcard;
    
    if(recv_packet.gameOp==GameOp::TURN){
      bool  pass=false;
      //penalty 여부
      bool penalty=false;
      int   pass_index=0;
      //덱 위에 있는 카드가 스페셜 카드면 0번째 카드를 내려 놓는다.
      //규칙이 정확하지 않아서 이 부분은 special 맡으신 분이 체크 부탁드립니다.
      for(int i=0;i<int(mycards.list.size());i++)
      {
	      if(mycards.list.at(i).color==0)
	      {
		      up.passingcard=mycards.list.at(i);
		      pass_index=i;
		      penalty=true;
		      break;
	      }
      }

      /*if(deck_front.number>=10){
        up.passingcard=mycards.list.at(pass_index);
        pass_index=0;
        pass=true;
      }*/

      //덱 위에 있는 카드가 스페셜 카드가 아니면, 손에 들고 있는 카드를 체크한다.
      if(!penalty){
        for(int i=0;i<int(mycards.list.size());i++){
          //발견시 해당 카드 내려 놓기
          if(deck_front.color ==mycards.list.at(i).color || deck_front.number==mycards.list.at(i).number){
            pass_index=i;
            pass=true;
            up.passingcard=mycards.list.at(i);
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
      }
      else if(penalty)
      {
	      mycards.list.erase(mycards.list.begin()+pass_index);
	      mycards.number=mycards.list.size();
	      up.userOp=UserOp::PLAY;
	      up.gameOp=GameOp::PENALTY;
      }
      else{
        //UserOp는 Draw
        up.userOp=UserOp::DRAW;
	up.gameOp=GameOp::DRAW;
      }
      up.seq=recv_packet.seq;
      up.uid=recv_packet.uid;
      //up.gameOp=recv_packet.gameOp;
      up.numOfCards=mycards.number;

      if(penalty)
      {
	      cout<<"Action : PLAY, Card : "<<"("<<up.passingcard.color<<","<<up.passingcard.number<<")"<<endl;
	      cout<<"Next player will get penalty"<<endl;
      }
      else if(pass)
      {
	      cout<<"Action : PLAY, Card : "<<"("<<up.passingcard.color<<","<<up.passingcard.number<<")"<<endl;
      }
      else
      {
	      cout<<"Action : Draw"<<endl;
      }
    }

    //turn이 아닐 경우 구현
    else{
      if(recv_packet.gameOp==GameOp::WAIT){
        up.gameOp=GameOp::WAIT;
        up.uid=recv_packet.uid;
        up.seq=recv_packet.seq;
      }
      if(recv_packet.gameOp==GameOp::DRAW){
        up.gameOp=GameOp::DRAW;
        up.uid=recv_packet.uid;
        up.seq=recv_packet.seq;
      }
      if(recv_packet.gameOp==GameOp::GAMEOVER){
        up.gameOp=GameOp::GAMEOVER;
        up.uid=recv_packet.uid;
        up.seq=recv_packet.seq;
      }
      if(recv_packet.gameOp==GameOp::UNO){
        up.gameOp=GameOp::UNO;
        up.userOp=UserOp::UNO;
        up.uid=recv_packet.uid;
        up.seq=recv_packet.seq;
      }
      if(recv_packet.gameOp==GameOp::PENALTY){
	      up.numOfCards=mycards.number;
	      up.gameOp=GameOp::TURN;
	      up.uid=recv_packet.uid;
	      up.seq=recv_packet.seq;
      }

    }






    p = Create<Packet> (reinterpret_cast<uint8_t*>(&up), sizeof(up));
    
    return p;
}

} // Namespace ns3

