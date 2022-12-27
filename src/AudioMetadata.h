#pragma once

#include <cstddef>
#include <cstdint>

#include <span>

namespace parakeet_audio {
/**
 * @brief Get the ID3 header size.
 * @private
 *
 * @param buffer
 * @return std::size_t
 */
std::size_t GetID3HeaderSize(const std::span<const uint8_t> buffer);

/**
 * @brief Get APEv2 header size
 * @private
 *
 * @param buffer
 * @return std::size_t
 */
std::size_t GetAPEv2FullSize(const std::span<const uint8_t> buffer);

/**
 * @brief Detect ID3v2/APEv2 Tag size.
 *        When detected, a positive integer indicating its size from `buf`
 *        will be returned.
 *
 * @param buffer
 * @return std::size_t 0 if header metatype is unsupported.
 */
std::size_t GetAudioHeaderMetadataSize(const std::span<const uint8_t> buffer);

}  // namespace parakeet_audio
