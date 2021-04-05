# btmesh_data_log #

## Summary ##

This demo shows how to use Silicon Labs Bluetooth Mesh Vendor Model to send data between nodes in Mesh network.

## Gecko SDK version ##

Bluetooth SDK 3.1.1 Bluetooth Mesh 2.0.1

## Hardware Required ##

EFR32xG21 Development Kits.

## Connections Required ##


## Setup ##

## How It Works ##


## .sls Projects Used ##

soc_btmesh_data_logging_client_xg21.sls
soc_btmesh_data_logging_server_xg21.sls

## How to Port to Another Part ##

You will get a customized light example for Thunderboard Sense 2 as the starting point, which is created from Simplicity Studio based on the light example in the Bluetooth Mesh SDK. The project is called BT-Mesh-Smart-Lightning.sls. The project contains the LC server and the sensor models. Follow below steps to import it to your Simplicity Studio.

1)   Open Simplicity Studio, then click [File] -> [Import], 

2)   Choose BT-Mesh-Smart-Lightning.sls

3)   the project is imported to your IDE

## Special Notes ##

Please see doc\BT Mesh Smart Lightning Demo.docx  for more details.