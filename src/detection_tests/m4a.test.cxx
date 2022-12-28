#include "parakeet-audio/DetectAudioType.h"

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

TEST(AudioDetection, M4A) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x20, 'f', 't', 'y', 'p', 'M', '4', 'A', ' ', 0x00, 0x00, 0x00, 0x01,
      'i',  's',  'o',  'm',  'i', 's', 'o', '2', 'M', '4', 'A', ' ', 'm',  'p',  '4',  '2',
  };

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeM4A);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "m4a");
}

TEST(AudioDetection, M4B) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x10, 'f', 't', 'y', 'p', 'M', '4', 'B', ' ', 0x00, 0x00, 0x00, 0x01,
  };

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeM4B);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "m4b");
}

TEST(AudioDetection, M4A_with_NeroAAC) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x10, 'f', 't', 'y', 'p', 'N', 'D', 'A', 'S', 0x00, 0x00, 0x00, 0x01,
  };

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeM4A);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "m4a");
}
