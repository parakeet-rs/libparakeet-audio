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

TEST(AudioDetection, MP4_Container_isom) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x10, 'f', 't', 'y', 'p', 'i', 's', 'o', 'm', 0x00, 0x00, 0x00, 0x01,
  };

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeMP4);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "mp4");
}

TEST(AudioDetection, MP4_Container_iso2) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x10, 'f', 't', 'y', 'p', 'i', 's', 'o', '2', 0x00, 0x00, 0x00, 0x01,
  };

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeMP4);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "mp4");
}

TEST(AudioDetection, MP4_SonyMP4_Container) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x10, 'f', 't', 'y', 'p', 'M', 'S', 'N', 'V', 0x00, 0x00, 0x00, 0x01,  //
  };

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeMP4);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "mp4");
}

TEST(AudioDetection, MP4_Generic_MP4_Container_with_dby1) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x18, 'f', 't', 'y', 'p', 'm',  'p',  '4',  '2',  0x00, 0x00, 0x00, 0x01,
      'm',  'p',  '4',  '2',  'd', 'b', 'y', '1', 0x00, 0x00, 0x1D, 0xD5, 0x6D, 0x6F, 0x6F, 0x76,
  };

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeMP4);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "mp4");
}

TEST(AudioDetection, MP4_Container_with_unsupported_type) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x10, 'f', 't', 'y', 'p', 'u', 'n', 'k', '?', 0x00, 0x00, 0x00, 0x01,
  };

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "bin");
}
