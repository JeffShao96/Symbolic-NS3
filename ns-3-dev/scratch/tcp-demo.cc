/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

// Network topology
//
//       n0 ----------- n1
//
// - Flow from n0 to n1 using BulkSendApplication.
// - Tracing of queues and packet receptions to file "tcp-bulk-send.tr"
//   and pcap tracing available when tracing is turned on.

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "/home/s2e/s2e/s2e.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpBulkSendExample");

int
main (int argc, char *argv[])
{
  uint32_t maxBytes = 1000;
  Config::SetDefault("ns3::TcpSocket::InitialCwnd",UintegerValue (1));

//
// Allow the user to override any of the defaults at
// run-time, via command-line arguments
//
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

//
// Explicitly create the nodes required by the topology (shown above).
//

  Time::SetResolution (Time::MS); 

  Ptr<Symbolic> symObj0 = CreateObject<Symbolic>();
  symObj0->SetMinMax(1, 1000);
  Time d0 = symObj0->GetSymbolicTime();
  Ptr<Symbolic> symObj1 = CreateObject<Symbolic>();
  symObj0->SetMinMax(1, 1000);
  Time d1 = symObj1->GetSymbolicTime();

NodeContainer nodes;
  nodes.Create (2); 

  uint32_t links = 2;
  std::vector<NodeContainer> nodeAdjacencyList (links);
  nodeAdjacencyList[0] = NodeContainer (nodes.Get (0), nodes.Get (1));
  nodeAdjacencyList[1] = NodeContainer (nodes.Get (0), nodes.Get (1));
  std::vector<PointToPointHelper> p2p (links);

  std::vector<NetDeviceContainer> devices (links);

  p2p[0].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p[1].SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p[0].SetChannelAttribute ("Delay", TimeValue (d0));
  p2p[1].SetChannelAttribute ("Delay", TimeValue (d1));


  for(uint32_t i = 0; i < links; i++){
    devices[i] = p2p[i].Install (nodeAdjacencyList[i]);
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


  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  std::vector<Ptr<Ipv4StaticRouting>> remoteHostStaticRoutingNode (2);
  for(uint32_t i = 0; i < 2; i++){
    remoteHostStaticRoutingNode[i] = ipv4RoutingHelper.GetStaticRouting(nodes.Get (i)->GetObject<Ipv4>());
  }
  remoteHostStaticRoutingNode[0]->RemoveRoute(2);
  remoteHostStaticRoutingNode[0]->RemoveRoute(1);
  remoteHostStaticRoutingNode[1]->RemoveRoute(2);
  remoteHostStaticRoutingNode[1]->RemoveRoute(1);
  remoteHostStaticRoutingNode[0]->AddNetworkRouteTo(Ipv4Address("0.0.0.0"),Ipv4Mask("0.0.0.0"),Ipv4Address("10.1.0.2"),1);
  remoteHostStaticRoutingNode[1]->AddNetworkRouteTo(Ipv4Address("0.0.0.0"),Ipv4Mask("0.0.0.0"),Ipv4Address("10.2.0.1"),2);
//
// Create a BulkSendApplication and install it on node 0
//
  uint16_t port = 9;  // well-known echo port number


  BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (interfaces[0].GetAddress (1), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApps = source.Install (nodes.Get (0));
  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (Seconds (10.0));

//
// Create a PacketSinkApplication and install it on node 1
//
  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (nodes.Get (1));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (10.0));

//
// Now, do the actual simulation.
//
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (2));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
  Time timeAdd0 = 2 * d0 + d1;
  Time timeAdd1 = 3 * d0 + 2 * d1;
  Symbolic::PrintRangeTime("2 d0 + 1 d1", timeAdd0);
  Symbolic::PrintRangeTime("3 d0 + 2 d1", timeAdd1);
  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  Symbolic::Print("Total Bytes Received:");
  Symbolic::Print(sink1->GetTotalRx ());
  Symbolic::Stop("Program terminated");
}
