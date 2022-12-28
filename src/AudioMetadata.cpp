#include "AudioMetadata.h"
#include "parakeet_endian.h"

#include <cstddef>

namespace parakeet_audio {

inline auto ParseID3SyncSafeInt(const uint8_t* p) -> int32_t {
  constexpr uint32_t kUnsafeIntMask32 = 0x80808080;
  constexpr uint32_t kU32ByteMask1 = 0xFF000000;
  constexpr uint32_t kU32ByteMask2 = 0x00FF0000;
  constexpr uint32_t kU32ByteMask3 = 0x0000FF00;
  constexpr uint32_t kU32ByteMask4 = 0x000000FF;

  const auto value = ReadBigEndian<uint32_t>(p);

  // Sync safe int should use only lower 7-bits of each byte.
  if ((value & kUnsafeIntMask32) != 0) {
    return 0;
  }

  return static_cast<int32_t>(((value & kU32ByteMask1) >> 3) | ((value & kU32ByteMask2) >> 2) |
                              ((value & kU32ByteMask3) >> 1) | ((value & kU32ByteMask4) >> 0));
}

auto GetID3HeaderSize(const std::span<const uint8_t> buffer) -> std::size_t {
  constexpr std::size_t kID3HeaderMinSize = 10;

  constexpr std::size_t kID3V1Size = 128;
  constexpr uint32_t kID3v1Masks = 0xFF'FF'FF'00u;  // Select first 3 bytes
  constexpr uint32_t kID3v1Magic = 0x54'41'47'00u;  // 'TAG\x00'

  constexpr std::size_t kID3V2HeaderSize = 10;
  constexpr uint32_t kID3v2Masks = 0xFF'FF'FF'00u;  // Select first 3 bytes
  constexpr uint32_t kID3v2Magic = 0x49'44'33'00u;  // 'ID3\x00'

  if (buffer.size() < kID3HeaderMinSize) {
    return 0;
  }

  const auto magic = parakeet_audio::ReadBigEndian<uint32_t>(&buffer[0]);

  // ID3v1 and ID3v1.1: flat 128 bytes
  if ((magic & kID3v1Masks) == kID3v1Magic) {
    return kID3V1Size;
  }

  if ((magic & kID3v2Masks) == kID3v2Magic) {
    // offset    value
    //      0    header('ID3')
    //      3    uint8_t(ver_major) uint8_t(ver_minor)
    //      5    uint8_t(flags)
    //      6    uint32_t(inner_tag_size)
    //     10    byte[inner_tag_size] id3v2 data
    //     ??    byte[*] original_file_content

    if (const auto inner_size = ParseID3SyncSafeInt(&buffer[6]); inner_size > 0) {
      return kID3V2HeaderSize + inner_size;
    }
  }

  return 0;
}

auto GetAPEv2FullSize(const std::span<const uint8_t> buffer) -> std::size_t {
  constexpr std::size_t kMinAPEv2BufferSize = 0x10;
  constexpr std::size_t kOffsetAPEv2HeaderSize = 0x0c;

  constexpr uint64_t kAPEv2Magic = 0x41'50'45'54'41'47'45'58u;  // 'APETAGEX'
  constexpr std::size_t kAPEv2HeaderSize = 32;

  if (buffer.size() >= kMinAPEv2BufferSize && kAPEv2Magic == parakeet_audio::ReadBigEndian<uint64_t>(&buffer[0])) {
    // Tag size in bytes including footer and all tag items excluding the header.
    return parakeet_audio::ReadLittleEndian<uint32_t>(&buffer[kOffsetAPEv2HeaderSize]) + kAPEv2HeaderSize;
  }

  return 0;
}

auto GetAudioHeaderMetadataSize(const std::span<const uint8_t> buffer) -> std::size_t {
  if (std::size_t id3_meta_size = GetID3HeaderSize(buffer)) {
    return id3_meta_size;
  }

  // It's possible to have APEv2 header at the beginning of a file, though rare.
  if (std::size_t ape_meta_size = GetAPEv2FullSize(buffer)) {
    return ape_meta_size;
  }

  return 0;
}

}  // namespace parakeet_audio
