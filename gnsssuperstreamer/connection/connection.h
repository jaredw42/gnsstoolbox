#pragma once

class DeviceConnection {

  virtual bool read() = 0;

  virtual bool write() = 0;
};