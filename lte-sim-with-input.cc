 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/mobility-module.h"
 #include "ns3/lte-module.h"
 #include "ns3/config-store.h"
 #include <ns3/buildings-helper.h>
 //#include "ns3/gtk-config-store.h"
  
 using namespace ns3;
  
 int main (int argc, char *argv[])
 {   
   CommandLine cmd;
   cmd.Parse (argc, argv);
  
   //注意，先load 再 save！
   // to save a template default attribute file run it like this:
   // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lte-sim-with-input
   //
   // to load a previously created default attribute file
   // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lte-sim-with-input
  
   ConfigStore inputConfig;
   inputConfig.ConfigureDefaults ();
 
   // Parse again so you can override default values from the command line
   cmd.Parse (argc, argv);
  
   Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  
   // Uncomment to enable logging
 //  lteHelper->EnableLogComponents ();
  
   // Create Nodes: eNodeB and UE
   NodeContainer enbNodes;
   NodeContainer ueNodes;
   enbNodes.Create (1);
   ueNodes.Create (1);
  
   // Install Mobility Model
   MobilityHelper mobility;
   mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobility.Install (enbNodes);
   BuildingsHelper::Install (enbNodes);
 //  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
 //  mobility.Install (ueNodes);
 //  BuildingsHelper::Install (ueNodes);
  
   mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                  "X", StringValue ("100.0"),
                                  "Y", StringValue ("100.0"),
                                  "Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
   mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                              "Mode", StringValue ("Time"),
                              "Time", StringValue ("2s"),
                              "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                              "Bounds", StringValue ("0|200|0|200"));
   mobility.Install(ueNodes);
   BuildingsHelper::Install (ueNodes);
  
   // Create Devices and install them in the Nodes (eNB and UE)
   NetDeviceContainer enbDevs;
   NetDeviceContainer ueDevs;
   // Default scheduler is PF, uncomment to use RR
   //lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
  
   enbDevs = lteHelper->InstallEnbDevice (enbNodes);
   ueDevs = lteHelper->InstallUeDevice (ueNodes);
  
   // Attach a UE to a eNB
   lteHelper->Attach (ueDevs, enbDevs.Get (0));
  
   // Activate a data radio bearer
   enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
   EpsBearer bearer (q);
   lteHelper->ActivateDataRadioBearer (ueDevs, bearer);
   //lteHelper->EnableTraces ();
  
   Simulator::Stop (Seconds (1.05));
  
   // configure all the simulation scenario here...
   lteHelper->EnablePhyTraces ();
   lteHelper->EnableMacTraces ();
   lteHelper->EnableRlcTraces ();
   lteHelper->EnablePdcpTraces ();
  
   Simulator::Run ();
  
   // GtkConfigStore config;
   // config.ConfigureAttributes ();
  
   Simulator::Destroy ();
   return 0;
 }
