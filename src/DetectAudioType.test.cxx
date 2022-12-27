#include "parakeet-audio/DetectAudioType.h"

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

TEST(AudioDetection, Ogg) {
  std::array<uint8_t, 0x20> data = {"OggS"};
  auto detected_type = DetectAudioType(data);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeOGG);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "ogg");
}

TEST(AudioDetection, FLAC) {
  std::array<uint8_t, 0x20> data = {"fLaC"};
  auto detected_type = DetectAudioType(data);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeFLAC);
  EXPECT_EQ(AudioIsLossless(detected_type), true);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "flac");
}

TEST(AudioDetection, M4A) {
  std::array<uint8_t, 0x20> m4aHeader = {
      0x00, 0x00, 0x00, 0x20, 'f', 't', 'y', 'p', 'M', '4', 'A', ' ', 0x00, 0x00, 0x00, 0x01,  //
      'i',  's',  'o',  'm',  'i', 's', 'o', '2', 'M', '4', 'A', ' ', 'm',  'p',  '4',  '2'    //
  };

  auto detected_type = DetectAudioType(m4aHeader);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeM4A);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "m4a");
}
