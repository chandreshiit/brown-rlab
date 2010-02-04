#include <iostream>
#include "./global.h"
#include "./udpsocket.h"
#include "./datagram.h"
#include "./objectposepacket.h"
#include "./localizableobject.h"

using namespace std;
using namespace rlab;

int main(int argc, char* argv[])
{
  // Create UDP socket that listens on port 8856 and is non-blocking
  UDPSocket udpSocket(8856, false);
  if (!udpSocket.prepareUDPServerSocket()) {
    cerr << "Failed to create UDP socket on port " << udpSocket.getPort() << "." << endl;
    return 1;
  }
  cout << "Listening on port " << udpSocket.getPort() << "..." << endl;

  while (true) {
    // Read packets as long as there are some available
    while (udpSocket.dataAvailable()) {
      Datagram* datagram = udpSocket.receiveDatagram();
      ObjectPosePacket* packet = ObjectPosePacket::parse(datagram->getMessage());
      if (!packet) {
        cerr << "Received invalid UDP packet." << endl;
        delete datagram;
        continue;
      }

      cout << "Content of packet from " << datagram->getPeerInfo() << ":" << endl;
      for (unsigned int i = 0; i < packet->getLocalizableObjectCount(); ++i) {
        const LocalizableObject& object = packet->getLocalizableObject(i);

        cout << "Object type: ";
        if (object.getType() == LocalizableObject::OBJECT_TYPE_SMURV) {
          cout << "SmURV"; 
        }
        else if (object.getType() == LocalizableObject::OBJECT_TYPE_BALL) {
          cout << "Ball";
        }
        else {
          cout << "Obstacle";
        }

        cout << ", ID: " << object.getId();
        cout << ", X: " << object.getPose().getX();
        cout << ", Y: " << object.getPose().getY();
        cout << ", Yaw: " << object.getPose().getYaw();
        cout << ", age of pose in ms: " << object.getPoseAge() << endl;
      }
      cout << "------------------------------------------------------" << endl;

      delete packet;
      delete datagram;
    }

    // Do path planning or something other nifty
    // ...
  }



  return 0;
}

