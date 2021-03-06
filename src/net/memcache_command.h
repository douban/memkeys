#ifndef _NET_MEMCACHE_CMD_H
#define _NET_MEMCACHE_CMD_H

#include <string>

#include "net/packet.h"
#include "net/pcap.h"

namespace mckeys {

enum memcache_command_t {
  MC_UNKNOWN, MC_REQUEST, MC_RESPONSE
};

/**
 * Usage: auto mc = MemcacheCommand::create(pkt, captureAddress);
 */
class MemcacheCommand
{
 public:
  static MemcacheCommand create(const Packet& pkt,
                                const bpf_u_int32 captureAddress,
                                const memcache_command_t expectedCmdType);

  bool isCommand() const
    { return (isRequest() || isResponse()); }
  bool isRequest() const
    { return (cmdType_ == MC_REQUEST); }
  bool isResponse() const
    { return (cmdType_ == MC_RESPONSE); }

  // only when isRequest is true
  std::string getCommandName() const { return commandName_; }

  ssize_t getObjectNumber() const { return objectKeyList_.size(); }

  // sometimes when isResponse is true, sometimes when isRequest is true
  std::string getObjectKey(uint32_t idx = 0) const { 
    return (idx < objectKeyList_.size() ? objectKeyList_[idx] : ""); 
  }

  // only when isResponse is true
  uint32_t getObjectSize(uint32_t idx = 0) const { 
      return (idx < objectSizeList_.size() ? objectSizeList_[idx] : 0); 
  }

  // source address for request
  std::string getSourceAddress() const { return sourceAddress_; }

  // destination address for response
  std::string getDestinationAddress() const { return destinationAddress_; }

 protected:

  // no assignment operator
  MemcacheCommand& operator=(const MemcacheCommand& mc) = delete;

  // Default constructor is protected
  MemcacheCommand();

  MemcacheCommand(const memcache_command_t cmdType,
                  const std::string sourceAddress,
                  const std::string destinationAddress,
                  const std::string commandName);

  MemcacheCommand(const memcache_command_t cmdType,
                  const std::string sourceAddress,
                  const std::string destinationAddress,
                  const std::string commandName,
                  const std::string objectKey,
                  uint32_t objectSize);

  static MemcacheCommand makeRequestCommand(u_char *data,
                                            int dataLength,
                                            std::string sourceAddress,
                                            std::string destinationAddress);
  static MemcacheCommand makeResponseCommand(u_char *data,
                                             int dataLength,
                                             std::string sourceAddress,
                                             std::string destinationAddress);

  void pushObject(const std::string objectKey, uint32_t objectSize);

  memcache_command_t cmdType_;
  std::string sourceAddress_;
  std::string destinationAddress_;
  std::string commandName_;
  std::vector<std::string> objectKeyList_;
  std::vector<uint32_t> objectSizeList_;
};

} // end namespace

#endif
