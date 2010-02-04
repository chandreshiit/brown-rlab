/**
 * Brown University, Computer Science Department
 *
 * Author: Jonas Schwertfeger <js at cs.brown.edu>
 * Date:   9/20/2007
 *
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <signal.h>
#include <argp.h>
#include "./global.h"
#include "./dc1394camera.h"
#include "./udpsocket.h"
#include "./datagram.h"
#include "./objectposepacket.h"
#include "./artkplocalizer.h"
#include "./cmvisionlocalizer.h"

//#include <cppunit/extensions/TestFactoryRegistry.h>
//#include <cppunit/ui/text/TestRunner.h>
//#include "./objectposepackettest.h"


using namespace std;
using namespace cimg_library;
using namespace rlab;


// Argp (argument parser) definitions
//-----------------------------------------------------------------------------------------------------------
const char* argp_program_version = "Top-Down Localization Client 1.0";
const char* argp_program_bug_address = "<js@cs.brown.edu>";

static char programDoc[] =
  "Top-Down Localization Client -- A client analyzing top-down camera images for object localiation purposes.";

static char argsDoc[] = "";

static struct argp_option options[] = {
   {"host",         'h', "HOSTNAME", 0, "Localization server hostname"},
   {"port",         'p', "PORT", 0, "Localization server port number"},
   {"fwport",       'f', "FWPORT", 0, "Firewire port number"},
   {"camera",       'c', "CAMERA", 0, "Camera number"},
   {"xoffset",      'x', "XOFFSET", 0, "X room offset of camera in meters"},
   {"yoffset",      'y', "YOFFSET", 0, "Y room offset of camera in meters"},
   {"hfov",         'w', "HFOV", 0, "Horizontal field of view in meters"},
   {"vfov",         'v', "VFOV", 0, "Vertical field of view in meters"},
   {"rgb",          'r', 0, 0, "Show RGB window"},
   {"debug",        'd', 0, 0, "Show debugging information"},
   {"tests",        't', 0, 0, "Run unit tests"},
   {0}
};

typedef struct arguments {
  bool runTests;
  bool showRGB;
  bool debugMode;
  string serverHostname;
  int serverPort;
  int fwPortNr;
  int cameraNr;
  float xRoomOffset;
  float yRoomOffset;
  float hFoV;
  float vFoV;
  float artkpPatternWidth;
} Arguments;

static error_t parseOptions(int key, char* arg, struct argp_state* state);

static struct argp argp = {
  options,
  parseOptions,
  argsDoc,
  programDoc
};

// Constants
//-----------------------------------------------------------------------------------------------------------
const int IMG_WIDTH = 640;
const int IMG_HEIGHT = 480;


// Globals
//-----------------------------------------------------------------------------------------------------------
Arguments args;
bool running = false;

DC1394Camera* camera = NULL;
UDPSocket* udpSocket = NULL;
ARTKPLocalizer* artkpLocalizer = NULL;
CMVisionLocalizer* cmVisionLocalizer = NULL;


// Function prototypes
//-----------------------------------------------------------------------------------------------------------
bool runLocalization();
bool setUp();
bool tearDown();
bool runTests();
void registerSignalHandler();
void exitSignalHandler(int signal);


// Functions
//-----------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Default argument values
  args.runTests = false;
  args.showRGB = false;
  args.debugMode = false;
  args.serverHostname = "localhost";
  args.serverPort = 8855;
  args.fwPortNr = 0;
  args.cameraNr = 0;
  args.xRoomOffset = 0.0;
  args.yRoomOffset = 0.0;
  args.hFoV = 0.0;
  args.vFoV = 0.0;
  args.artkpPatternWidth = 160.0;

  argp_parse(&argp, argc, argv, 0, 0, &args);

  if (args.runTests) {
    return runTests();
  }

  registerSignalHandler();

  if (!setUp()) {
    cerr << "Could not initialize client. Exiting." << endl;
    return 1;
  }

  runLocalization();

  if (!tearDown()) {
    return 1;
  }
  else {
    return 0;
  }
}


//-----------------------------------------------------------------------------------------------------------
bool runLocalization()
{
  Image img(IMG_WIDTH, IMG_HEIGHT, 1, 3, 0);
  Image rgbImg(IMG_WIDTH, IMG_HEIGHT, 1, 3, 0);
  Image grayImg(IMG_WIDTH, IMG_HEIGHT, 1, 1, 0);
  CImgDisplay rgbDisp;
  if (args.showRGB) {
    rgbDisp.set_title("RGB");
  }
  CImgDisplay grayDisp(grayImg, "Gray");

  bool collectingColorRange = false;
  Pixel minY = 255, minU = 255, minV = 255;
  Pixel maxY = 0, maxU = 0, maxV = 0;
  vector<Blob> blobs;
  const unsigned char colors[][3] = {
    {255, 140, 0} // Orange
  };
  const int numColors = 1;

  ObjectPosePacket posePacket;
  string posePacketData;
  Datagram poseDatagram("", args.serverPort, args.serverHostname);

  camera->startCapturing();

  running = true;
  while (running && ((args.showRGB && !rgbDisp.is_closed) || !grayDisp.is_closed)) {
    if (camera->getFrame(img)) {
      vector<LocalizableObject> objects;

      // Let ARToolkitPlus localizer analyze image
      artkpLocalizer->localizeObjects(img.get_channel(0), objects);

      // Let CMVision localizer analyze image
      blobs.clear();
      cmVisionLocalizer->localizeObjects(img, objects, blobs);

      if (args.debugMode) {
        cout << "======================================================================" << endl;
        cout << "Found " << objects.size() << " objects:" << endl;
        cout << "Type\tId\tX\tY\tYaw" << endl;
        for (vector<LocalizableObject>::iterator obj = objects.begin(); obj != objects.end(); ++obj) {
          cout << obj->getType();
          cout << "\t" << obj->getId();
          cout << "\t" << setprecision(2) << obj->getPose().getX();
          cout << "\t" << setprecision(2) << obj->getPose().getY();
          cout << "\t" << setprecision(1) << (obj->getPose().getYaw() * 180.0 / 3.14159265) << endl;
        }
      }

      if (objects.size()) {
        // Add objects localized to pose packet
        posePacket.reset();
        posePacket.addLocalizableObjects(objects);

        // Generate a binary datastream from the packet
        if (!posePacket.generate(posePacketData)) {
          cerr << "Failed to generate pose packet." << endl;
        }
        else {
          // Send the datastream as a UDP packet to the server
          poseDatagram.setMessage(posePacketData);
          if (!udpSocket->sendDatagram(poseDatagram)) {
            cerr << "Failed to send pose packet." << endl;
          }
        }
      }

      // Display RGB and gray image
      if (args.showRGB) {
        rgbImg = img.get_YCbCrtoRGB();
        for (vector<Blob>::iterator blob = blobs.begin(); blob != blobs.end(); ++blob) {
          rgbImg.draw_rectangle(
              blob->x1,
              blob->y1,
              blob->x2,
              blob->y2,
              colors[blob->color % numColors],
              0.5);
        }
        rgbDisp << rgbImg;
      }
      grayDisp << img.get_channel(0);
    }
    else {
      cerr << "Failed to get image from camera." << endl;
    }

    // Color range measurement stuff
    if (rgbDisp.button == 2 && collectingColorRange) {
      cout << "YUV = ["
        << (unsigned int)minY << ":" << (unsigned int)maxY << ", "
        << (unsigned int)minU << ":" << (unsigned int)maxU << ", "
        << (unsigned int)minV << ":" << (unsigned int)maxV << "]" << endl;

      collectingColorRange = false;
      minY = 255; minU = 255; minV = 255;
      maxY = 0; maxU = 0; maxV = 0;
    }
    else if (rgbDisp.button == 1) {
      Pixel y = img(rgbDisp.mouse_x, rgbDisp.mouse_y, 0);
      Pixel u = img(rgbDisp.mouse_x, rgbDisp.mouse_y, 1);
      Pixel v = img(rgbDisp.mouse_x, rgbDisp.mouse_y, 2);

      if (y < minY) { minY = y; }
      if (y > maxY) { maxY = y; }
      if (u < minU) { minU = u; }
      if (u > maxU) { maxU = u; }
      if (v < minV) { minV = v; }
      if (v > maxV) { maxV = v; }

      collectingColorRange = true;
    }

    /*cout << "Sleeping..." << endl;
    usleep(250000);*/
  }

  camera->stopCapturing();
  sleep(1);

  return true;
}


//-----------------------------------------------------------------------------------------------------------
bool setUp()
{
  camera = new DC1394Camera(args.fwPortNr, args.cameraNr, DC1394Camera::VIDEOMODE_640x480_YUV422);
/*  if (!camera->setExposureValue(300)) {
    cerr << "Failed to set manual exposure value." << endl;
    return false;
  }*/

  udpSocket = new UDPSocket();
  if (!udpSocket->prepareUDPClientSocket()) {
    cerr << "Failed to create UDP client socket." << endl;
    return false;
  }

  artkpLocalizer = new ARTKPLocalizer(IMG_WIDTH, IMG_HEIGHT, "./fire_i_feb2007.spec");
  artkpLocalizer->setXOffset(args.xRoomOffset);
  artkpLocalizer->setYOffset(args.yRoomOffset);
  artkpLocalizer->setPatternWidth(args.artkpPatternWidth);

  cmVisionLocalizer = new CMVisionLocalizer(IMG_WIDTH, IMG_HEIGHT, "./blobcolors.txt");
  cmVisionLocalizer->setXOffset(args.xRoomOffset);
  cmVisionLocalizer->setYOffset(args.yRoomOffset);
  cmVisionLocalizer->setHorizontalFoV(args.hFoV);
  cmVisionLocalizer->setVerticalFoV(args.vFoV);

  return true;
}


//-----------------------------------------------------------------------------------------------------------
bool tearDown()
{
  if (cmVisionLocalizer) {
    delete cmVisionLocalizer;
  }

  if (artkpLocalizer) {
    delete artkpLocalizer;
  }

  if (udpSocket) {
    delete udpSocket;
  }

  if (camera) {
    delete camera;
  }

  return true;
}


//-----------------------------------------------------------------------------------------------------------
static error_t parseOptions(int key, char* arg, struct argp_state* state)
{
  Arguments* args = reinterpret_cast<Arguments*>(state->input);

  switch (key) {
  case 'h':
    args->serverHostname = string(arg);
    break;
  case 'p':
    if (!from_string<int>(args->serverPort, arg, std::dec)) {
      argp_usage(state);
    }
    break;
  case 'f':
    if (!from_string<int>(args->fwPortNr, arg, std::dec)) {
      argp_usage(state);
    }
    break;
  case 'c':
    if (!from_string<int>(args->cameraNr, arg, std::dec)) {
      argp_usage(state);
    }
    break;
  case 'r':
    args->showRGB = true;
    break;
  case 't':
    args->runTests = true;
    break;
  case 'd':
    args->debugMode = true;
    break;
  case 'x':
    if (!from_string<float>(args->xRoomOffset, arg, std::dec)) {
      argp_usage(state);
    }
    break;
  case 'y':
    if (!from_string<float>(args->yRoomOffset, arg, std::dec)) {
      argp_usage(state);
    }
    break;
  case 'v':
    if (!from_string<float>(args->vFoV, arg, std::dec)) {
      argp_usage(state);
    }
    break;
  case 'w':
    if (!from_string<float>(args->hFoV, arg, std::dec)) {
      argp_usage(state);
    }
    break;
  case ARGP_KEY_ARG:
    if (state->arg_num >= 0) {
      /* Too many arguments. */
      argp_usage(state);
    }
    break;
  case ARGP_KEY_END:
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}


//-----------------------------------------------------------------------------------------------------------
bool runTests()
{
  /*CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry& registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  return runner.run("", false);*/
  cerr << "Did not run any tests; not compiled with CppUnit support." << endl;
  return false;
}


//-----------------------------------------------------------------------------------------------------------
void registerSignalHandler()
{
  struct sigaction action;
  action.sa_handler = &exitSignalHandler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;;

  if (sigaction(SIGINT, &action, NULL) != 0
      || sigaction(SIGHUP, &action, NULL) != 0
      || sigaction(SIGTERM, &action, NULL) != 0) {
    cerr << "Could not register signal handler. Abort." << endl;
    exit(1);
  }
}


//-----------------------------------------------------------------------------------------------------------
void exitSignalHandler(int signal)
{
  cout << "Caught exit signal " << signal << ". Shutting down." << endl;
  running = false;
}

