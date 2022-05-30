#include <iostream>
#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

using namespace ns3;


int main(int argc, char* argv[])
{
    uint32_t num_client = 3;
    CommandLine cmd;
    cmd.AddValue("Num_Client", "Number of clients", num_client);
    cmd.Parse(argc, argv);
    LogComponentEnable("UnoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UnoClientApplication", LOG_LEVEL_INFO);

    /* Create nodes */
    uint32_t num_terminals = num_client+1;
    NodeContainer terminals;
    terminals.Create(num_terminals);

    NodeContainer csmaSwitch;
    csmaSwitch.Create(1);

    /* Create links */
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", DataRateValue(5000000));
    csma.SetChannelAttribute("Delay", TimeValue(MicroSeconds(10)));

    NetDeviceContainer terminalDevices;
    NetDeviceContainer switchDevices;

    /* Connect switch node and terminal node */
    for (uint32_t i = 0; i < num_terminals; i++) {
        NetDeviceContainer link = csma.Install(NodeContainer(terminals.Get(i), csmaSwitch));
        terminalDevices.Add(link.Get(0));
        switchDevices.Add(link.Get(1));
    }

    Ptr<Node> switchNode = csmaSwitch.Get(0);
    BridgeHelper bridge;
    bridge.Install(switchNode, switchDevices);

    /* Install Internet stack and assign IP addresses */
    InternetStackHelper internet;
    internet.Install(terminals);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    ipv4.Assign(terminalDevices);
    /*
     * 10.1.1.1 n0
     * 10.1.1.2 n1
     * 10.1.1.3 n2
     * 10.1.1.4 n3
     */

    uint16_t port = 9;
    // Please use InetSocketAddress and specify address and port
    /*Address clientAddressList[4] = { Address(InetSocketAddress(Ipv4Address("10.1.1.1"), port)),
                                     Address(InetSocketAddress(Ipv4Address("10.1.1.2"), port)),
                                     Address(InetSocketAddress(Ipv4Address("10.1.1.3"), port)) };*/

    Address ClientAddressList[num_client];
    for(uint32_t i=0; i<num_client; i++)
    {
        int add =  i+1;
        string last = to_string(add);
        string s_address = "10.1.1." + last;
        const char* address = s_address.c_str();
        ClientAddressList[i]=Address(InetSocketAddress(Ipv4Address(address), port));
    }

    // Install clients
    UnoClientHelper unoclienthelper(port);

    ApplicationContainer client[num_client];
    for (uint32_t i = 0; i < num_client; i++) {
        client[i] = unoclienthelper.Install(terminals.Get(i));
        client[i].Start(Seconds(1.0));
        client[i].Stop(Seconds(30.0));
    }
    
    // Install server
    UnoServerHelper unoserverhelper(num_client, ClientAddressList);
    ApplicationContainer server = unoserverhelper.Install(terminals.Get(num_terminals-1));
    server.Start(Seconds(1.0));
    server.Stop(Seconds(30.0));
    

    Simulator::Stop(Seconds(35));
    Simulator::Run();
    Simulator::Destroy();
}
