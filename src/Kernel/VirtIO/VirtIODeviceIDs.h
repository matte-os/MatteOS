#pragma once

#include <Utils/Types.h>

// https://docs.oasis-open.org/virtio/virtio/v1.2/csd01/virtio-v1.2-csd01.pdf
// 5 Device Types
enum class VirtIODeviceIDs : size_t {
  Invalid = 0,
  NetworkCard = 1,
  BlockDevice = 2,
  Console = 3,
  EntropySource = 4,
  MemoryBalloon = 5,
  IOMemory = 6,
  RPMSG = 7,
  SCSIHost = 8,
  _9PTransport = 9,
  Mac80211WiFi = 10,
  RPROCSerial = 11,
  VirtioCAIF = 12,
};

static constexpr size_t PCIEVendorID = 0x1af4;
static constexpr size_t PCIEDeviceID = 0x1040;