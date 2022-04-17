#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/symbolic-module.h"
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ReachabilityTesting");

int
main (int argc, char *argv[])
{

  Time::SetResolution (Time::MS); 


  CommandLine cmd;
  cmd.Parse (argc, argv);

  NodeContainer nodes;
  nodes.Create (8); 

  uint32_t links = 7;
  std::vector<NodeContainer> nodeAdjacencyList (links);
  nodeAdjacencyList[0] = NodeContainer (nodes.Get (0), nodes.Get (1));
  nodeAdjacencyList[1] = NodeContainer (nodes.Get (1), nodes.Get (2));
  nodeAdjacencyList[2] = NodeContainer (nodes.Get (1), nodes.Get (3));
  nodeAdjacencyList[3] = NodeContainer (nodes.Get (1), nodes.Get (4));
  nodeAdjacencyList[4] = NodeContainer (nodes.Get (2), nodes.Get (5));
  nodeAdjacencyList[5] = NodeContainer (nodes.Get (3), nodes.Get (6));
  nodeAdjacencyList[6] = NodeContainer (nodes.Get (4), nodes.Get (7));
  std::vector<PointToPointHelper> pointToPoint (links);

  std::vector<NetDeviceContainer> devices (links);

  pointToPoint[0].SetChannelAttribute ("Delay", StringValue ("5ms"));
  pointToPoint[1].SetChannelAttribute ("Delay", StringValue ("30ms"));
  pointToPoint[2].SetChannelAttribute ("Delay", StringValue ("50ms"));
  pointToPoint[3].SetChannelAttribute ("Delay", StringValue ("70ms"));
  pointToPoint[4].SetChannelAttribute ("Delay", StringValue ("1ms"));
  pointToPoint[5].SetChannelAttribute ("Delay", StringValue ("3ms"));
  pointToPoint[6].SetChannelAttribute ("Delay", StringValue ("7ms"));

  for(uint32_t i = 0; i < links; i++){
    pointToPoint[i].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    devices[i] = pointToPoint[i].Install (nodeAdjacencyList[i]);
  }  

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  std::vector<Ipv4InterfaceContainer> interfaces (links);
  for (uint32_t i = 0; i < links; i++)
    {
      std::ostringstream subset;
      subset << "10." << i + 1 << ".0.0";
      address.SetBase (subset.str().c_str (), "255.255.0.0"); 
      interfaces[i] =
          address.Assign (devices[i]); 
    }

  // We use the following code, it will auto create the routing table to connect to each other.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //Symbol Address
  Ptr<Symbolic> symObj0 = CreateObject<Symbolic>();
  symObj0->SetMinMax(0xa000000, 0xaffffff);
  Ipv4Address symIP0 = symObj0->GetSymbolicIpv4Add();

  V4PingHelper ping(symIP0);
  ApplicationContainer pingApp = ping.Install (nodes.Get(0));
  pingApp.Start (Seconds (1.0));
  pingApp.Stop (Seconds (1.7));
  
  
  Simulator::Stop(Seconds (1.7));
  
  Simulator::Run ();
  Simulator::Destroy ();
  symObj0->PrintRange("symIP0");
  symObj0->Stop("Program terminated");
  return 0;
}
