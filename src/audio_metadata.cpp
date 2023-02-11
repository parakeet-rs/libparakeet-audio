#include "parakeet-audio/audio_metadata.h"
#include "parakeet_endian.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace parakeet_audio {

int32_t ParseID3SyncSafeInt(const uint8_t* ptr) {
  constexpr uint32_t kUnsafeIntMask32 = 0x80808080;
  constexpr uint32_t kU32ByteMask1 = 0xFF000000;
  constexpr uint32_t kU32ByteMask2 = 0x00FF0000;
  constexpr uint32_t kU32ByteMask3 = 0x0000FF00;
  constexpr uint32_t kU32ByteMask4 = 0x000000FF;

  const auto value = ReadBigEndian<uint32_t>(ptr);

  // Sync safe int should use only lower 7-bits of each byte.
  if ((value & kUnsafeIntMask32) != 0) {
    return 0;
  }

  return static_cast<int32_t>(((value & kU32ByteMask1) >> 3) | ((value & kU32ByteMask2) >> 2) |
                              ((value & kU32ByteMask3) >> 1) | ((value & kU32ByteMask4) >> 0));
}

std::size_t GetID3HeaderSize(const uint8_t* buffer, size_t buffer_len) {
  constexpr std::array<uint8_t, 3> kID3v1Magic = {'T', 'A', 'G'};
  constexpr std::array<uint8_t, 3> kID3v2Magic = {'I', 'D', '3'};

  constexpr std::size_t kID3HeaderMinSize = 10;
  if (buffer_len < kID3HeaderMinSize) {
    return 0;
  }

  // ID3v1 and ID3v1.1: flat 128 bytes
  if (std::equal(kID3v1Magic.begin(), kID3v1Magic.end(), buffer)) {
    constexpr std::size_t kID3V1Size = 128;
    return kID3V1Size;
  }

  if (std::equal(kID3v2Magic.begin(), kID3v2Magic.end(), buffer)) {
    // offset    value
    //      0    header('ID3')
    //      3    uint8_t(ver_major) uint8_t(ver_minor)
    //      5    uint8_t(flags)
    //      6    uint32_t(inner_tag_size)
    //     10    byte[inner_tag_size] id3v2 data
    //     ??    byte[*] original_file_content

    if (const auto inner_size = ParseID3SyncSafeInt(buffer + 6); inner_size > 0) {
      constexpr std::size_t kID3V2HeaderSize = 10;
      return kID3V2HeaderSize + inner_size;
    }
  }

  return 0;
}

std::size_t GetAPEv2FullSize(const uint8_t* buffer, size_t buffer_len) {
  constexpr std::size_t kMinAPEv2BufferSize = 0x10;
  constexpr std::size_t kOffsetAPEv2HeaderSize = 0x0c;

  constexpr std::array<uint8_t, 8> kAPEv2Magic = {'A', 'P', 'E', 'T', 'A', 'G', 'E', 'X'};
  constexpr std::size_t kAPEv2HeaderSize = 32;

  if (buffer_len >= kMinAPEv2BufferSize && std::equal(kAPEv2Magic.begin(), kAPEv2Magic.end(), buffer)) {
    // Tag size in bytes including footer and all tag items excluding the header.
    return parakeet_audio::ReadLittleEndian<uint32_t>(&buffer[kOffsetAPEv2HeaderSize]) + kAPEv2HeaderSize;
  }

  return 0;
}

std::size_t GetAudioHeaderMetadataSize(const uint8_t* buffer, size_t buffer_len) {
  if (std::size_t id3_meta_size = GetID3HeaderSize(buffer, buffer_len)) {
    return id3_meta_size;
  }

  // It's possible to have APEv2 header at the beginning of a file, though rare.
  if (std::size_t ape_meta_size = GetAPEv2FullSize(buffer, buffer_len)) {
    return ape_meta_size;
  }

  return 0;
}

}  // namespace parakeet_audio
