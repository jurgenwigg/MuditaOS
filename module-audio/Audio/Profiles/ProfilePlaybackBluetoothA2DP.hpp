// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Profile.hpp"
#include <module-bsp/board/rt1051/bsp/audio/RT1051BluetoothAudio.hpp>

namespace audio
{

    class ProfilePlaybackBluetoothA2DP : public Profile
    {
      public:
        ProfilePlaybackBluetoothA2DP(std::function<int32_t()> callback, Volume volume)
            : Profile("Playback A2DP",
                      Type::PlaybackBTA2DP,
                      bsp::AudioDevice::Format{.sampleRate_Hz = 44100,
                                               .bitWidth      = 16,
                                               .flags         = 0,
                                               .outputVolume  = static_cast<float>(volume),
                                               .inputGain     = 0,
                                               .inputPath     = bsp::AudioDevice::InputPath::None,
                                               .outputPath    = bsp::AudioDevice::OutputPath::BluetoothA2DP},
                      bsp::AudioDevice::Type::Bluetooth,
                      callback)
        {}
    };

} // namespace audio