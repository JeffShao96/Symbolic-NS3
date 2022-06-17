#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
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
  
  Ptr<Symbolic> sym0 = CreateObject<Symbolic>();
  sym0->SetMinMax(1, 1000);
  uint32_t d0 = sym0->GetSymbolicUintValue();
  Ptr<Symbolic> sym1 = CreateObject<Symbolic>();
  sym1->SetMinMax(1, 1000);
  uint32_t d1 = sym1->GetSymbolicUintValue();

  std::vector<PointToPointHelper> p2p (2);
  p2p[0].SetChannelAttribute("Delay",TimeValue(Time(d0)));
  p2p[1].SetChannelAttribute("Delay",TimeValue(Time(d1)));

  NodeContainer nodes;
  nodes.Create (3); 

  std::vector<NodeContainer> nodeAdjacencyList (2);
  nodeAdjacencyList[0] = NodeContainer (nodes.Get (0), nodes.Get (2));
  nodeAdjacencyList[1] = NodeContainer (nodes.Get (1), nodes.Get (2));

  std::vector<NetDeviceContainer> devices (2);
  devices[0] = p2p[0].Install (nodeAdjacencyList[0]);
  devices[1] = p2p[1].Install (nodeAdjacencyList[1]);

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

  UdpServerHelper server (2333);

  ApplicationContainer rcv = server.Install (nodes.Get (2));
  rcv.Start (Seconds (1.0));
  rcv.Stop (Seconds (10.0));

  UdpClientHelper snd0 (interfaces[0].GetAddress (1), 2333);
  snd0.SetAttribute ("MaxPackets", UintegerValue (1));

  UdpClientHelper snd1 (interfaces[1].GetAddress (1), 2333);
  snd1.SetAttribute ("MaxPackets", UintegerValue (1));

  ApplicationContainer snd;
  snd.Add(snd0.Install (nodes.Get (0)));
  snd.Add(snd1.Install (nodes.Get (1)));
  snd.Start (Seconds (1.0));
  snd.Stop (Seconds (10.0));

  Simulator::Run ();
  int diff = d0 - d1;
  Symbolic::PrintRange("Diff",diff);
  Simulator::Destroy ();
  Symbolic::Stop("Program terminated");
  return 0;
}
