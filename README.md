# libparakeet-audio [![Coverage Status](https://coveralls.io/repos/github/parakeet-rs/libparakeet-audio/badge.svg?branch=main)](https://coveralls.io/github/parakeet-rs/libparakeet-audio?branch=main)

A simple audio detection library.

## License

Licensed under the [MIT License](LICENSE.txt).

## Usage

```cmake
# cmake
# include CPM
CPMAddPackage(
  NAME parakeet_audio
  GITHUB_REPOSITORY parakeet-rs/libparakeet-audio
  GIT_TAG v0.1.0
  OPTIONS
      "PARAKEET_AUDIO_BUILD_TESTING OFF"
)

target_link_libraries(YOUR_PROJECT_NAME PRIVATE parakeet::audio)
```

```cpp
#include <parakeet-audio/detect_audio_type.h>

#include <cstdint>
#include <cstdio>

int main() {
  uint8_t test_data[] = {'f', 'L', 'a', 'C'};
  auto ext = parakeet_audio::DetectAudioExtension(test_data);
  printf("detected audio ext: %s\n", ext);
}
```

## References

- Magic numbers:
  https://www.garykessler.net/library/file_sigs.html
- DFF:
  https://www.sonicstudio.com/pdf/dsd/DSDIFF_1.5_Spec.pdf
- MP3:
  http://www.mp3-tech.org/programmer/frame_header.html
- AAC:
  https://wiki.multimedia.cx/index.php/ADTS
- FLAC:
  https://xiph.org/flac/format.html
- fytp:
  https://www.ftyps.com/
