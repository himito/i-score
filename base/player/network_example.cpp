#include "player.hpp"
#include <ossia/network/generic/generic_device.hpp>
#include <QCoreApplication>
int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  // Create a player instance
  iscore::Player p;

  // Create a device
  ossia::net::generic_device dev;

  // The name has to match one of the devices in the loaded file
  dev.set_name("OSCdevice");

  // Add a custom callback on the device
  auto address = ossia::net::create_node(dev, "/foo/bar").create_address(ossia::val_type::FLOAT);
  address->add_callback([] (const ossia::value& val) {
    std::cerr << val << std::endl;
  });

  // The device will replace the implementation that will be loaded with the same name.
  p.registerDevice(dev);

  return app.exec();
}
