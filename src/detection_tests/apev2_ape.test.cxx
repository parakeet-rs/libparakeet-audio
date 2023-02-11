#include "parakeet-audio/detect_audio_type.h"

#include "parakeet_endian.h"

#include <cstdint>
#include <cstdlib>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <array>

using parakeet_audio::AudioIsLossless;
using parakeet_audio::AudioType;
using parakeet_audio::DetectAudioType;
using parakeet_audio::GetAudioTypeExtension;
using ::testing::ElementsAreArray;

TEST(AudioDetection, APEv2_with_APE) {
  std::array<uint8_t, 0xB0> header = {
      'A',  'P',  'E',  'T',  'A',  'G',  'E',  'X',   //
      0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,  // 0x20 bytes header + 0x30 data
  };
  header[0x50] = 'M';
  header[0x51] = 'A';
  header[0x52] = 'C';
  header[0x53] = ' ';

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeAPE);
  EXPECT_EQ(AudioIsLossless(detected_type), true);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "ape");
}
