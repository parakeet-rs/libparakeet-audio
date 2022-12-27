#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>

#include <array>

#include "parakeet-audio/DetectAudioType.h"

using parakeet_audio::AudioIsLossless;
using parakeet_audio::AudioType;
using parakeet_audio::DetectAudioType;
using parakeet_audio::GetAudioTypeExtension;
using ::testing::ElementsAreArray;

TEST(AudioDetection, Ogg) {
  uint8_t data[] = "OggS\x00\x00\x00\x00";
  auto detected_type = DetectAudioType(data);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeOGG);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "ogg");
}

TEST(AudioDetection, FLAC) {
  uint8_t data[] = "fLaC\x00\x00\x00\x00";
  auto detected_type = DetectAudioType(data);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeFLAC);
  EXPECT_EQ(AudioIsLossless(detected_type), true);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "flac");
}

TEST(AudioDetection, M4A) {
  uint8_t m4aHeader[0x20] = {
      0x00, 0x00, 0x00, 0x20, 'f', 't', 'y', 'p', 'M', '4', 'A', ' ', 0x00, 0x00, 0x00, 0x01,  //
      'i',  's',  'o',  'm',  'i', 's', 'o', '2', 'M', '4', 'A', ' ', 'm',  'p',  '4',  '2'    //
  };

  auto detected_type = DetectAudioType(m4aHeader);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeM4A);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "m4a");
}
