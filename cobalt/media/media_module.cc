/*
 * Copyright 2015 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cobalt/media/media_module.h"

#include "base/compiler_specific.h"

namespace cobalt {
namespace media {

namespace {

class DummyWebMediaPlayer : public ::media::WebMediaPlayer {
 private:
  typedef ::media::BufferedDataSource BufferedDataSource;

  void LoadMediaSource() OVERRIDE {}
  void LoadProgressive(const GURL& url,
                       const scoped_refptr<BufferedDataSource>& data_source,
                       ::media::WebMediaPlayer::CORSMode cors_mode) OVERRIDE {
    UNREFERENCED_PARAMETER(url);
    UNREFERENCED_PARAMETER(data_source);
    UNREFERENCED_PARAMETER(cors_mode);
  }
  void CancelLoad() OVERRIDE {}

  // Playback controls.
  void Play() OVERRIDE {}
  void Pause() OVERRIDE {}
  bool SupportsFullscreen() const OVERRIDE { return false; }
  bool SupportsSave() const OVERRIDE { return false; }
  void Seek(float seconds) OVERRIDE { UNREFERENCED_PARAMETER(seconds); }
  void SetEndTime(float seconds) OVERRIDE { UNREFERENCED_PARAMETER(seconds); }
  void SetRate(float rate) OVERRIDE { UNREFERENCED_PARAMETER(rate); }
  void SetVolume(float volume) OVERRIDE { UNREFERENCED_PARAMETER(volume); }
  void SetVisible(bool visible) OVERRIDE { UNREFERENCED_PARAMETER(visible); }
  const ::media::Ranges<base::TimeDelta>& Buffered() OVERRIDE {
    return buffer_;
  }
  float MaxTimeSeekable() const OVERRIDE { return 0.f; }

  // True if the loaded media has a playable video/audio track.
  bool HasVideo() const OVERRIDE { return false; }
  bool HasAudio() const OVERRIDE { return false; }

  // Dimension of the video.
  gfx::Size NaturalSize() const OVERRIDE { return gfx::Size(); }

  // Getters of playback state.
  bool Paused() const OVERRIDE { return false; }
  bool Seeking() const OVERRIDE { return false; }
  float Duration() const OVERRIDE { return 0.f; }
  float CurrentTime() const OVERRIDE { return 0.f; }

  // Get rate of loading the resource.
  int DataRate() const OVERRIDE { return 1; }

  // Internal states of loading and network.
  NetworkState GetNetworkState() const OVERRIDE { return kNetworkStateEmpty; }
  ReadyState GetReadyState() const OVERRIDE { return kReadyStateHaveNothing; }

  bool DidLoadingProgress() const OVERRIDE { return false; }
  unsigned long long TotalBytes() const OVERRIDE { return 0; }

  bool HasSingleSecurityOrigin() const OVERRIDE { return false; }
  bool DidPassCORSAccessCheck() const OVERRIDE { return false; }

  float MediaTimeForTimeValue(float time_value) const OVERRIDE {
    UNREFERENCED_PARAMETER(time_value);
    return 0.f;
  }

  unsigned DecodedFrameCount() const OVERRIDE { return 0; }
  unsigned DroppedFrameCount() const OVERRIDE { return 0; }
  unsigned AudioDecodedByteCount() const OVERRIDE { return 0; }
  unsigned VideoDecodedByteCount() const OVERRIDE { return 0; }

  ::media::Ranges<base::TimeDelta> buffer_;
};

}  // namespace

using ::media::WebMediaPlayer;

scoped_ptr<WebMediaPlayer> MediaModule::CreateWebMediaPlayer(
    ::media::WebMediaPlayerClient* client) {
  UNREFERENCED_PARAMETER(client);
  return make_scoped_ptr<WebMediaPlayer>(new DummyWebMediaPlayer);
}

}  // namespace media
}  // namespace cobalt
