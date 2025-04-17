/**
 * @file Forwards.h
 * @author Matěj Bucek
 */
#pragma once

namespace Kernel {
  class Inode;
  class FileSystem;
  class DevInode;
  class RamFileSystem;
  class RamInode;
  class DeviceManager;
  class Device;
  class ConsoleDevice;
  class VirtIODevice;
  class BlockDevice;
  class Driver;
  class DeviceDriver;
  class BlockDeviceDriver;
  class AsyncRequest;
  class AsyncResult;
  class BlockDeviceAsyncRequest;
  class BlockDeviceAsyncResult;
  class FATInode;
  class FATFileSystem;
}// namespace Kernel
