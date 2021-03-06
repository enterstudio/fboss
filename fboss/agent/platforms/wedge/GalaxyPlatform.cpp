/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include "fboss/agent/platforms/wedge/GalaxyPlatform.h"

#include "fboss/lib/usb/GalaxyI2CBus.h"
#include "fboss/agent/platforms/wedge/GalaxyPort.h"

using std::make_unique;

namespace facebook { namespace fboss {

GalaxyPlatform::GalaxyPlatform(
    std::unique_ptr<WedgeProductInfo> productInfo,
    WedgePlatformMode mode)
    : WedgePlatform(
          std::move(productInfo),
          mode == WedgePlatformMode::GALAXY_LC ? kNumFrontPanelPorts : 0) {
  CHECK(
      mode == WedgePlatformMode::GALAXY_LC ||
      mode == WedgePlatformMode::GALAXY_FC);
}

GalaxyPlatform::InitPortMap GalaxyPlatform::initPorts() {
  InitPortMap ports;

  auto add_quad = [&](int start,
      TransceiverID frontPanelPort=TransceiverID(-1)) {
    for (int i = 0; i < 4; i++) {
      int num = start + i;
      PortID portID(num);
      opennsl_port_t bcmPortNum = num;

      auto port = make_unique<GalaxyPort>(portID,
          frontPanelPort,
          ChannelID(i),
          frontPanelPort == TransceiverID(-1));

      ports.emplace(bcmPortNum, port.get());
      ports_.emplace(portID, std::move(port));
    }
  };

  for (auto mapping : getFrontPanelMapping()) {
    add_quad(mapping.second, mapping.first);
  }

  for (auto portNum : getBackplanePorts()) {
    // Backplane ports are quad ports too.
    // Even though its unlikely we will ever
    // use them in anything except all 4 lanes
    // being used by single port.
    add_quad(portNum);
  }
  return ports;
}

std::unique_ptr<BaseWedgeI2CBus> GalaxyPlatform::getI2CBus() {
  return make_unique<GalaxyI2CBus>();
}

GalaxyPlatform::FrontPanelMapping GalaxyPlatform::getFrontPanelMapping() {
  if (isFC()) {
    return getFCFrontPanelMapping();
  } else if (isLC()) {
    return getLCFrontPanelMapping();
  }
  LOG(FATAL) <<" Unhandled mode " << static_cast<int>(getMode())
          << " on Galaxy";
  return FrontPanelMapping{};
}

GalaxyPlatform::FrontPanelMapping
GalaxyPlatform::getFCFrontPanelMapping() const {
    // No front panel ports on FCs
    return FrontPanelMapping{};
}

GalaxyPlatform::FrontPanelMapping
GalaxyPlatform::getLCFrontPanelMapping() const {
  return {
    {TransceiverID(0), PortID(84)},
    {TransceiverID(1), PortID(88)},
    {TransceiverID(2), PortID(92)},
    {TransceiverID(3), PortID(96)},
    {TransceiverID(4), PortID(102)},
    {TransceiverID(5), PortID(106)},
    {TransceiverID(6), PortID(110)},
    {TransceiverID(7), PortID(114)},
    {TransceiverID(8), PortID(118)},
    {TransceiverID(9), PortID(122)},
    {TransceiverID(10), PortID(126)},
    {TransceiverID(11), PortID(130)},
    {TransceiverID(12), PortID(1)},
    {TransceiverID(13), PortID(5)},
    {TransceiverID(14), PortID(9)},
    {TransceiverID(15), PortID(13)},
  };
}

GalaxyPlatform::BackplanePorts GalaxyPlatform::getBackplanePorts() const {
  if (isFC()) {
    return getFCBackplanePorts();
  } else if (isLC()) {
    return getLCBackplanePorts();
  }
  LOG(FATAL) <<" Unhandled mode " << static_cast<int>(getMode())
          << " on Galaxy";

  return BackplanePorts{};
}

GalaxyPlatform::BackplanePorts
GalaxyPlatform::getFCBackplanePorts() const {
  return {
    PortID(72),
    PortID(76),
    PortID(68),
    PortID(80),
    PortID(62),
    PortID(58),
    PortID(54),
    PortID(50),
    PortID(110),
    PortID(106),
    PortID(102),
    PortID(114),
    PortID(96),
    PortID(92),
    PortID(84),
    PortID(88),
    PortID(5),
    PortID(9),
    PortID(1),
    PortID(13),
    PortID(130),
    PortID(118),
    PortID(122),
    PortID(126),
    PortID(42),
    PortID(38),
    PortID(29),
    PortID(46),
    PortID(34),
    PortID(21),
    PortID(25),
    PortID(17),
  };
}

GalaxyPlatform::BackplanePorts
GalaxyPlatform::getLCBackplanePorts() const {
    return {
      PortID(68),
      PortID(72),
      PortID(76),
      PortID(80),
      PortID(50),
      PortID(54),
      PortID(58),
      PortID(62),
      PortID(34),
      PortID(38),
      PortID(42),
      PortID(46),
      PortID(17),
      PortID(21),
      PortID(25),
      PortID(29),
    };
}

}}
