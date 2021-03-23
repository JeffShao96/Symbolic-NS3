#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "/home/s2e/s2e/s2e.h"
#include "ns3/symbolic-module.h"

//Network Topology
//
// snda -------rcv------- sndb
//    point-to-point
//
 
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SymNS3ScriptExample");

int
main (int argc, char *argv[])
{

  Time::SetResolution (Time::MS); 
  
  // No Log Infomation Allowed in this mode!

  // LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
  // LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);

  CommandLine cmd;
  cmd.Parse (argc, argv);

  NodeContainer nodes;
  nodes.Create (3); 

  std::vector<NodeContainer> nodeAdjacencyList (2);
  nodeAdjacencyList[0] = NodeContainer (nodes.Get (0), nodes.Get (2));
  nodeAdjacencyList[1] = NodeContainer (nodes.Get (1), nodes.Get (2));

  Ptr<Symbolic> symA = CreateObject<Symbolic>();
  symA->SetAttribute("Min",UintegerValue(1));
  // We have Muitlple way to set a Max or Min.
  // symA->SetAttribute("Min",UintegerValue(Time(1ms).GetTimeStep()));
  // symA->SetMin(1);
  // symA->SetMin(Second(1));
  // symA->SetMin(Time("1ms"));
  symA->SetAttribute("Max",UintegerValue(1000));

  Ptr<Symbolic> symB = CreateObject<Symbolic>();
  symB->SetAttribute("Min",UintegerValue(1));
  symB->SetAttribute("Max",UintegerValue(1000));

  std::vector<PointToPointHelper> pointToPoint (2);
  pointToPoint[0].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint[0].SetChannelAttribute ("Symbolic", PointerValue (symA));

  pointToPoint[1].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint[1].SetChannelAttribute ("Symbolic", PointerValue (symB));


  std::vector<NetDeviceContainer> devices (2);
  devices[0] = pointToPoint[0].Install (nodeAdjacencyList[0]);
  devices[1] = pointToPoint[1].Install (nodeAdjacencyList[1]);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  std::vector<Ipv4InterfaceContainer> interfaces (2);
  for (uint32_t i = 0; i < 2; i++)
    {
      std::ostringstream subset;
      subset << "10.1." << i + 1 << ".0";
      address.SetBase (subset.str().c_str (), "255.255.255.0"); 
      interfaces[i] =
          address.Assign (devices[i]); 
    }

  UdpServerHelper server1 (2333);

  ApplicationContainer rcv = server1.Install (nodes.Get (2));
  rcv.Start (Seconds (1.0));
  rcv.Stop (Seconds (10.0));

  UdpClientHelper client1 (interfaces[0].GetAddress (1), 2333);
  client1.SetAttribute ("MaxPackets", UintegerValue (1));
  client1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  client1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer snda = client1.Install (nodes.Get (0));
  snda.Start (Seconds (1.0));
  snda.Stop (Seconds (10.0));

  UdpClientHelper client2 (interfaces[1].GetAddress (1), 2333);
  client2.SetAttribute ("MaxPackets", UintegerValue (1));
  client2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  client2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer sndb = client2.Install (nodes.Get (1));
  sndb.Start (Seconds (1.0));
  sndb.Stop (Seconds (10.0));

  Simulator::Run ();
  Symbolic symC =(Seconds(1.0)+*symA)  -(Seconds(1.0)+*symB);
  symC.PrintRange("DIFF");
  Simulator::Destroy ();

  s2e_kill_state(0,"Program Terminated");
  return 0;
}
