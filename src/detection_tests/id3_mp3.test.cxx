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

TEST(AudioDetection, MP3_with_ID3v1) {
  std::array<uint8_t, 0xB0> header = {'T', 'A', 'G'};
  parakeet_audio::WriteLittleEndian<uint32_t>(&header[128], 0x50FBFF);

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeMP3);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "mp3");
}

TEST(AudioDetection, MP3_with_ID3v2) {
  std::array<uint8_t, 0xB0> header = {'I', 'D', '3', 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08};
  parakeet_audio::WriteLittleEndian<uint32_t>(&header[0x92], 0x50FBFF);

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeMP3);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "mp3");
}

TEST(AudioDetectionSadPath, CrouptedID3) {
  std::array<uint8_t, 0xB0> header = {0x49, 0x44, 0x33, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  parakeet_audio::WriteLittleEndian<uint32_t>(&header[0x92], 0x50FBFF);

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
}

TEST(AudioDetectionSadPath, ID3WithUnsafeInt) {
  std::array<uint8_t, 0xB0> header = {0x49, 0x44, 0x33, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81};
  parakeet_audio::WriteLittleEndian<uint32_t>(&header[0x92], 0x50FBFF);

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
}

TEST(AudioDetectionSadPath, ID3TooLarge) {
  std::array<uint8_t, 0xFF> header = {0x49, 0x44, 0x33, 0x04, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F};

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
}

TEST(AudioDetectionSadPath, ID3_buffer_too_small) {
  std::array<uint8_t, 0x04> header = {0x49, 0x44, 0x33, 0x04};

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
}
