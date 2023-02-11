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

TEST(AudioDetection, Ogg) {
  std::array<uint8_t, 0x20> header = {"OggS"};
  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeOGG);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "ogg");
}

TEST(AudioDetection, FLAC) {
  std::array<uint8_t, 0x20> header = {"fLaC"};
  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeFLAC);
  EXPECT_EQ(AudioIsLossless(detected_type), true);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "flac");
}

TEST(AudioDetection, WAV) {
  std::array<uint8_t, 0x20> header = {'R', 'I', 'F', 'F'};

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeWAV);
  EXPECT_EQ(AudioIsLossless(detected_type), true);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "wav");
}

TEST(AudioDetection, DFF) {
  std::array<uint8_t, 0x20> header = {'F', 'R', 'M', '8'};

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeDFF);
  EXPECT_EQ(AudioIsLossless(detected_type), true);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "dff");
}

TEST(AudioDetection, AAC) {
  std::array<uint8_t, 0x20> header = {
      0xFF, 0xF1, 0x50, 0x80, 0x06, 0xFF, 0xFC, 0xDE, 0x04, 0x00, 0x4C, 0x61, 0x76, 0x63, 0x35, 0x38,  //
      0x2E, 0x31, 0x33, 0x34, 0x2E, 0x31, 0x30, 0x30, 0x00, 0x42, 0x54, 0x07, 0x20, 0xC6, 0xA0, 0x03,  //
  };

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeAAC);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "aac");
}

TEST(AudioDetection, WMA) {
  std::array<uint8_t, 0x20> header = {0x30, 0x26, 0xB2, 0x75};

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kAudioTypeWMA);
  EXPECT_EQ(AudioIsLossless(detected_type), false);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "wma");
}

TEST(AudioDetection, UnknownType) {
  std::array<uint8_t, 0x20> header = {0};

  auto detected_type = DetectAudioType(header.data(), header.size());
  EXPECT_EQ(detected_type, AudioType::kUnknownType);
  EXPECT_STREQ(GetAudioTypeExtension(detected_type), "bin");
}
