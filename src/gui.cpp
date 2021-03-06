// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "game.h"
#include "fplbase/utilities.h"
#include "states/game_menu_state.h"
#include "states/states_common.h"

using mathfu::vec2;
using mathfu::vec3;

namespace fpl {
namespace zooshi {

flatui::Event GameMenuState::PlayButtonSound(flatui::Event event,
                                          pindrop::SoundHandle& sound) {
  if (event & flatui::kEventWentUp) {
    audio_engine_->PlaySound(sound);
  }
  return event;
}

flatui::Event GameMenuState::TextButton(const char* text, float size,
                                     const flatui::Margin& margin) {
  return TextButton(text, size, margin, sound_click_);
}

flatui::Event GameMenuState::TextButton(const char* text, float size,
                                     const flatui::Margin& margin,
                                     pindrop::SoundHandle& sound) {
  return PlayButtonSound(flatui::TextButton(text, size, margin), sound);
}

flatui::Event GameMenuState::TextButton(const fplbase::Texture& texture,
                                     const flatui::Margin& texture_margin,
                                     const char* text, float size,
                                     const flatui::Margin& margin,
                                     const flatui::ButtonProperty property) {
  return TextButton(texture, texture_margin, text, size, margin, property,
                    sound_click_);
}

flatui::Event GameMenuState::TextButton(const fplbase::Texture& texture,
                                     const flatui::Margin& texture_margin,
                                     const char* text, float size,
                                     const flatui::Margin& margin,
                                     const flatui::ButtonProperty property,
                                     pindrop::SoundHandle& sound) {
  return PlayButtonSound(flatui::TextButton(texture, texture_margin, text, size,
                                         margin, property), sound);
}

flatui::Event GameMenuState::ImageButtonWithLabel(const fplbase::Texture& tex,
                                                  float size,
                                                  const flatui::Margin& margin,
                                                  const char* label) {
  return ImageButtonWithLabel(tex, size, margin, label, sound_click_);
}

flatui::Event GameMenuState::ImageButtonWithLabel(const fplbase::Texture& tex,
                                                  float size,
                                                  const flatui::Margin& margin,
                                                  const char* label,
                                                  pindrop::SoundHandle& sound) {
  flatui::StartGroup(flatui::kLayoutVerticalLeft, size, "ImageButtonWithLabel");
  flatui::SetMargin(margin);
  auto event = PlayButtonSound(flatui::CheckEvent(), sound);
  flatui::EventBackground(event);
  flatui::ImageBackground(tex);
  flatui::Label(label, size);
  flatui::EndGroup();
  return event;
}

MenuState GameMenuState::StartMenu(fplbase::AssetManager& assetman,
                                   flatui::FontManager& fontman,
                                   fplbase::InputSystem& input) {
  MenuState next_state = kMenuStateStart;

  // Run() accepts a lambda function that is executed 2 times,
  // one for a layout pass and another one in a render pass.
  // In the lambda callback, the user can call Widget APIs to put widget in a
  // layout.
  flatui::Run(assetman, fontman, input, [&]() {
    flatui::StartGroup(flatui::kLayoutHorizontalTop, 0);

    // Background image.
    flatui::StartGroup(flatui::kLayoutVerticalCenter, 0);
    // Positioning the UI slightly above of the center.
    flatui::PositionGroup(flatui::kAlignCenter, flatui::kAlignCenter,
                       mathfu::vec2(0, -150));
    flatui::Image(*background_title_, 1400);
    flatui::EndGroup();

    flatui::SetTextColor(kColorBrown);

    // Menu items. Note that we are layering 2 layouts here
    // (background + menu items).
    flatui::StartGroup(flatui::kLayoutVerticalCenter, 0);
    flatui::PositionGroup(flatui::kAlignCenter, flatui::kAlignCenter,
                       mathfu::vec2(0, -150));
    flatui::SetMargin(flatui::Margin(200, 700, 200, 100));

    auto event = TextButton("Play Game", kMenuSize, flatui::Margin(0),
                            sound_start_);
    if (event & flatui::kEventWentUp) {
      next_state = kMenuStateFinished;
#ifdef ANDROID_GAMEPAD
      if (!flatui::IsLastEventPointerType()) {
        next_state = kMenuStateGamepad;
      }
#endif
    }
    if (fplbase::SupportsHeadMountedDisplay()) {
      event = TextButton("Cardboard", kMenuSize, flatui::Margin(0),
                         sound_start_);
      if (event & flatui::kEventWentUp) {
        next_state = kMenuStateCardboard;
      }
    }
#ifdef USING_GOOGLE_PLAY_GAMES
    auto logged_in = gpg_manager_->LoggedIn();
    event = TextButton(*image_gpg_, flatui::Margin(0, 50, 10, 0),
                       logged_in ? "Sign out" : "Sign in", kMenuSize,
                       flatui::Margin(0), flatui::kButtonPropertyImageLeft,
                       sound_select_);
    if (event & flatui::kEventWentUp) {
      gpg_manager_->ToggleSignIn();
    }
#endif
    event = TextButton("Options", kMenuSize, flatui::Margin(0));
    if (event & flatui::kEventWentUp) {
      next_state = kMenuStateOptions;
    }
    event = TextButton("Quit", kMenuSize, flatui::Margin(0), sound_exit_);
    if (event & flatui::kEventWentUp) {
      // The exit sound is actually around 1.2s but since we fade out the
      // audio as well as the screen it's reasonable to shorten the duration.
      static const int kFadeOutTimeMilliseconds = 1000;
      fader_->Start(
          kFadeOutTimeMilliseconds, mathfu::kZeros3f, kFadeOut,
          vec3(vec2(flatui::VirtualToPhysical(mathfu::kZeros2f)), 0.0f),
          vec3(vec2(flatui::VirtualToPhysical(flatui::GetVirtualResolution())),
               0.0f));
      next_state = kMenuStateQuit;
    }
    flatui::EndGroup();
    flatui::EndGroup();
  });

  return next_state;
}

MenuState GameMenuState::OptionMenu(fplbase::AssetManager& assetman,
                                    flatui::FontManager& fontman,
                                    fplbase::InputSystem& input) {
  MenuState next_state = kMenuStateOptions;

  // FlatUI UI definitions.
  flatui::Run(assetman, fontman, input, [&]() {
    flatui::StartGroup(flatui::kLayoutOverlay, 0);
    flatui::StartGroup(flatui::kLayoutHorizontalTop, 0);
    // Background image. Note that we are layering 3 layouts here
    // (background + menu items + back button).
    flatui::StartGroup(flatui::kLayoutVerticalCenter, 0);
    // Positioning the UI slightly above of the center.
    flatui::PositionGroup(flatui::kAlignCenter, flatui::kAlignCenter,
                       mathfu::vec2(0, -150));
    flatui::Image(*background_options_, 1400);
    flatui::EndGroup();

    flatui::SetTextColor(kColorBrown);

    // Menu items.
    flatui::StartGroup(flatui::kLayoutVerticalCenter, 0);
    flatui::PositionGroup(flatui::kAlignCenter, flatui::kAlignCenter,
                       mathfu::vec2(0, -100));

    switch (options_menu_state_) {
      case kOptionsMenuStateMain:
        OptionMenuMain();
        break;
      case kOptionsMenuStateAbout:
        OptionMenuAbout();
        break;
      case kOptionsMenuStateLicenses:
        OptionMenuLicenses();
        break;
      case kOptionsMenuStateAudio:
        OptionMenuAudio();
        break;
      default:
        break;
    }

    flatui::EndGroup();

    // Foreground image (back button).
    flatui::StartGroup(flatui::kLayoutVerticalRight, 0);
    // Positioning the UI to up-left corner of the dialog.
    flatui::PositionGroup(flatui::kAlignCenter, flatui::kAlignCenter,
                       mathfu::vec2(-450, -250));
    flatui::SetTextColor(kColorLightBrown);

    auto event = ImageButtonWithLabel(*button_back_, 60,
                                      flatui::Margin(60, 35, 40, 50), "Back",
                                      sound_exit_);
    if (event & flatui::kEventWentUp) {
      // Save data when you leave the audio page.
      if (options_menu_state_ == kOptionsMenuStateAudio) {
        SaveData();
      }
      if (options_menu_state_ == kOptionsMenuStateMain) {
        next_state = kMenuStateStart;
      } else {
        options_menu_state_ = kOptionsMenuStateMain;
      }
    }
    flatui::EndGroup();
    flatui::EndGroup();

    flatui::EndGroup();  // Overlay group.
  });

  return next_state;
}

void GameMenuState::OptionMenuMain() {
  flatui::SetMargin(flatui::Margin(200, 300, 200, 100));

  flatui::StartGroup(flatui::kLayoutVerticalLeft, 50, "menu");
  flatui::SetMargin(flatui::Margin(0, 20, 0, 50));
  flatui::SetTextColor(kColorBrown);
  flatui::EndGroup();

  if (TextButton("About", kButtonSize, flatui::Margin(2),
                 sound_select_) & flatui::kEventWentUp) {
    options_menu_state_ = kOptionsMenuStateAbout;
  }

#ifdef USING_GOOGLE_PLAY_GAMES
  auto logged_in = gpg_manager_->LoggedIn();
  auto property = flatui::kButtonPropertyImageLeft;

  if (!logged_in) {
    flatui::SetTextColor(kColorLightGray);
    property |= flatui::kButtonPropertyDisabled;
  }
  auto event = TextButton(*image_leaderboard_, flatui::Margin(0, 25, 10, 0),
                          "Leaderboard", kButtonSize, flatui::Margin(0),
                          property);
  if (logged_in && (event & flatui::kEventWentUp)) {
    // Fill in Leaderboard list.
    auto leaderboard_config = config_->gpg_config()->leaderboards();
    gpg_manager_->ShowLeaderboards(
        leaderboard_config->LookupByKey(kGPGDefaultLeaderboard)->id()->c_str());
  }

  event = TextButton(*image_achievements_, flatui::Margin(0, 20, 0, 0),
                     "Achievements", kButtonSize, flatui::Margin(0), property);
  if (logged_in && (event & flatui::kEventWentUp)) {
    gpg_manager_->ShowAchievements();
  }
  flatui::SetTextColor(kColorBrown);
#endif

  if (TextButton("Licenses", kButtonSize, flatui::Margin(2)) &
      flatui::kEventWentUp) {
    scroll_offset_ = mathfu::kZeros2f;
    options_menu_state_ = kOptionsMenuStateLicenses;
  }

  if (TextButton("Audio", kButtonSize, flatui::Margin(2)) &
      flatui::kEventWentUp) {
    options_menu_state_ = kOptionsMenuStateAudio;
  }

#ifdef ANDROID_HMD
  // If the device supports a head mounted display allow the user to toggle
  // between gyroscopic and onscreen controls.
  if (fplbase::SupportsHeadMountedDisplay()) {
    const bool hmd_controller_enabled = world_->GetHmdControllerEnabled();
    if (TextButton(hmd_controller_enabled ? "Gyroscopic Controls" :
                   "Onscreen Controls", kButtonSize, flatui::Margin(2)) &
        flatui::kEventWentUp) {
      world_->SetHmdControllerEnabled(!hmd_controller_enabled);
      SaveData();
    }
  }
#endif  // ANDROID_HMD
}

void GameMenuState::OptionMenuAbout() {
  flatui::SetMargin(flatui::Margin(200, 400, 200, 100));

  flatui::StartGroup(flatui::kLayoutVerticalLeft, 50, "menu");
  flatui::SetMargin(flatui::Margin(0, 20, 0, 55));
  flatui::SetTextColor(kColorBrown);
  flatui::Label("About", kButtonSize);
  flatui::EndGroup();

  flatui::SetTextColor(kColorDarkGray);
  flatui::SetTextFont("fonts/NotoSans-Bold.ttf");

  flatui::StartGroup(flatui::kLayoutHorizontalCenter);
  flatui::SetMargin(flatui::Margin(50, 0, 0, 0));
  flatui::StartGroup(flatui::kLayoutVerticalCenter, 0, "scroll");
  flatui::StartScroll(kScrollAreaSize, &scroll_offset_);
  flatui::Label(about_text_.c_str(), 35, vec2(kScrollAreaSize.x(), 0));
  vec2 scroll_size = flatui::GroupSize();
  flatui::EndScroll();
  flatui::EndGroup();

  // Normalize the scroll offset to use for the scroll bar value.
  auto scroll_height = (scroll_size.y() - kScrollAreaSize.y());
  if (scroll_height > 0) {
    auto scrollbar_value = scroll_offset_.y() / scroll_height;
    flatui::ScrollBar(*scrollbar_back_, *scrollbar_foreground_,
                   vec2(35, kScrollAreaSize.y()),
                   kScrollAreaSize.y() / scroll_size.y(), "LicenseScrollBar",
                   &scrollbar_value);

    // Convert back the scroll bar value to the scroll offset.
    scroll_offset_.y() = scrollbar_value * scroll_height;
  }

  flatui::EndGroup();
  flatui::SetTextFont("fonts/RaviPrakash-Regular.ttf");
}

void GameMenuState::OptionMenuLicenses() {
  flatui::SetMargin(flatui::Margin(200, 300, 200, 100));

  flatui::StartGroup(flatui::kLayoutVerticalLeft, 50, "menu");
  flatui::SetMargin(flatui::Margin(0, 20, 0, 55));
  flatui::SetTextColor(kColorBrown);
  flatui::Label("Licenses", kButtonSize);
  flatui::EndGroup();

  flatui::SetTextColor(kColorDarkGray);
  flatui::SetTextFont("fonts/NotoSans-Bold.ttf");

  flatui::StartGroup(flatui::kLayoutHorizontalCenter);
  flatui::SetMargin(flatui::Margin(50, 0, 0, 0));
  flatui::StartGroup(flatui::kLayoutVerticalCenter, 0, "scroll");
  // This check event makes the scroll group controllable with a gamepad.
  flatui::StartScroll(kScrollAreaSize, &scroll_offset_);
  auto event = flatui::CheckEvent(true);
  if (!flatui::IsLastEventPointerType())
    flatui::EventBackground(event);
  flatui::Label(license_text_.c_str(), 25, vec2(kScrollAreaSize.x(), 0));
  vec2 scroll_size = flatui::GroupSize();
  flatui::EndScroll();
  flatui::EndGroup();

  // Normalize the scroll offset to use for the scroll bar value.
  auto scroll_height = (scroll_size.y() - kScrollAreaSize.y());
  if (scroll_height > 0) {
    auto scrollbar_value = scroll_offset_.y() / scroll_height;
    flatui::ScrollBar(*scrollbar_back_, *scrollbar_foreground_,
                   vec2(35, kScrollAreaSize.y()),
                   kScrollAreaSize.y() / scroll_size.y(), "LicenseScrollBar",
                   &scrollbar_value);

    // Convert back the scroll bar value to the scroll offset.
    scroll_offset_.y() = scrollbar_value * scroll_height;
  }

  flatui::EndGroup();
  flatui::SetTextFont("fonts/RaviPrakash-Regular.ttf");
}

void GameMenuState::OptionMenuAudio() {
  auto original_music_volume = slider_value_music_;
  auto original_effect_volume = slider_value_effect_;
  flatui::SetMargin(flatui::Margin(200, 200, 200, 100));

  flatui::StartGroup(flatui::kLayoutVerticalLeft, 50, "menu");
  flatui::SetMargin(flatui::Margin(0, 50, 0, 50));
  flatui::SetTextColor(kColorBrown);
  flatui::Label("Audio", kButtonSize);
  flatui::EndGroup();

  flatui::StartGroup(flatui::kLayoutHorizontalCenter, 20);
  flatui::Label("Music volume", kAudioOptionButtonSize);
  flatui::SetMargin(flatui::Margin(0, 40, 0, 0));
  flatui::Slider(*slider_back_, *slider_knob_, vec2(400, 60), 0.6f,
                 "MusicVolume", &slider_value_music_);

  flatui::EndGroup();
  flatui::StartGroup(flatui::kLayoutHorizontalCenter, 20);
  flatui::Label("Effect volume", kAudioOptionButtonSize);
  flatui::SetMargin(flatui::Margin(0, 40, 0, 0));
  auto event = flatui::Slider(*slider_back_, *slider_knob_, vec2(400, 60), 0.6f,
                           "EffectVolume", &slider_value_effect_);
  if (event & (flatui::kEventWentUp | flatui::kEventEndDrag)) {
    audio_engine_->PlaySound(sound_adjust_);
  }
  flatui::EndGroup();

  if (original_music_volume != slider_value_music_ ||
      original_effect_volume != slider_value_effect_) {
    UpdateVolumes();
  }
}

}  // zooshi
}  // fpl
