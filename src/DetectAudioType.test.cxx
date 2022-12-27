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

TEST(AudioDetection, Ogg) {
  std::array<uint8_t, 0x20> header = {"OggS"};
  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeOGG);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "ogg");
}

TEST(AudioDetection, FLAC) {
  std::array<uint8_t, 0x20> header = {"fLaC"};
  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeFLAC);
  EXPECT_EQ(AudioIsLossless(detected_type), true);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "flac");
}

TEST(AudioDetection, M4A) {
  std::array<uint8_t, 0x20> header = {
      0x00, 0x00, 0x00, 0x20, 'f', 't', 'y', 'p', 'M', '4', 'A', ' ', 0x00, 0x00, 0x00, 0x01,  //
      'i',  's',  'o',  'm',  'i', 's', 'o', '2', 'M', '4', 'A', ' ', 'm',  'p',  '4',  '2'    //
  };

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeM4A);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "m4a");
}

TEST(AudioDetection, AAC) {
  std::array<uint8_t, 0x20> header = {
      0xFF, 0xF1, 0x50, 0x80, 0x06, 0xFF, 0xFC, 0xDE, 0x04, 0x00, 0x4C, 0x61, 0x76, 0x63, 0x35, 0x38,  //
      0x2E, 0x31, 0x33, 0x34, 0x2E, 0x31, 0x30, 0x30, 0x00, 0x42, 0x54, 0x07, 0x20, 0xC6, 0xA0, 0x03,  //
  };

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeAAC);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "aac");
}

TEST(AudioDetection, MP3_with_ID3) {
  std::array<uint8_t, 0xB0> header = {0x49, 0x44, 0x33, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x08};
  parakeet_audio::WriteLittleEndian<uint32_t>(&header[0x92], 0x50FBFF);

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kAudioTypeMP3);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "mp3");
}

TEST(AudioDetectionSadPath, CrouptedID3) {
  std::array<uint8_t, 0xB0> header = {0x49, 0x44, 0x33, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  parakeet_audio::WriteLittleEndian<uint32_t>(&header[0x92], 0x50FBFF);

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
}

TEST(AudioDetectionSadPath, ID3TooLarge) {
  std::array<uint8_t, 0xFF> header = {0x49, 0x44, 0x33, 0x04, 0x00, 0x00, 0x7F, 0x7F, 0x7F, 0x7F};

  auto detected_type = DetectAudioType(header);
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
}
