/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "uno-helper.h"
#include "ns3/uno-server.h"
#include "ns3/uno-client.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

UnoClientHelper::UnoClientHelper (uint16_t port)
{
  m_factory.SetTypeId (UnoClient::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void 
UnoClientHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
UnoClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UnoClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UnoClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
UnoClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<UnoClient> ();
  node->AddApplication (app);

  return app;
}

UnoServerHelper::UnoServerHelper (uint32_t num, Address* addressList)
{
    const std::string addressAttributes[10] = { "ClientAddress1", "ClientAddress2",
                                                "ClientAddress3", "ClientAddress4",
                                                "ClientAddress5", "ClientAddress6",
                                                "ClientAddress7", "ClientAddress8",
                                                "ClientAddress9", "ClientAddress10" };
    if (num < 2 || num > 10) {
        NS_FATAL_ERROR("Invalid number of client");
    }
    
    m_factory.SetTypeId (UnoServer::GetTypeId ());
    SetAttribute ("NumberOfClient", UintegerValue (num));

    for (uint32_t i = 0; i < num; i++) {
        SetAttribute (addressAttributes[i], AddressValue (addressList[i]));
    }
}

void 
UnoServerHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
UnoServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UnoServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
UnoServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
UnoServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<UnoServer> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
