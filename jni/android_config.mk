# Copyright 2015 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Locations of 3rd party and FPL libraries.
THIRD_PARTY_ROOT:=$(ZOOSHI_DIR)/../../../../external
PREBUILTS_ROOT:=$(ZOOSHI_DIR)/../../../../prebuilts
FPL_ROOT:=$(ZOOSHI_DIR)/../../libs
# If the dependencies directory exists either as a subdirectory or as the
# container of this project directory, assume the dependencies directory is
# the root directory for all libraries required by this project.
DEPENDENCIES_ROOT?=$(wildcard $(ZOOSHI_DIR)/dependencies)
ifneq ($(DEPENDENCIES_ROOT),)
  THIRD_PARTY_ROOT:=$(DEPENDENCIES_ROOT)
  FPL_ROOT:=$(DEPENDENCIES_ROOT)
  PREBUILTS_ROOT:=$(DEPENDENCIES_ROOT)
endif

# Location of the SDL library.
DEPENDENCIES_SDL_DIR?=$(THIRD_PARTY_ROOT)/sdl
# Location of the SDL Mixer library.
DEPENDENCIES_SDL_MIXER_DIR?=$(THIRD_PARTY_ROOT)/sdl_mixer
# Location of the CORGI library.
DEPENDENCIES_CORGI_DIR?=$(FPL_ROOT)/corgi
# Location of the CORGI library's component collection.
DEPENDENCIES_CORGI_COMPONENT_LIBRARY_DIR?=\
$(DEPENDENCIES_CORGI_DIR)/component_library
# Location of the breadboard library.
DEPENDENCIES_BREADBOARD_DIR?=$(FPL_ROOT)/breadboard
# Location of the breadboard module library's module collection.
DEPENDENCIES_BREADBOARD_MODULE_LIBRARY_DIR?=\
    $(DEPENDENCIES_BREADBOARD_DIR)/module_library
# Location of the Flatbuffers library.
DEPENDENCIES_FLATBUFFERS_DIR?=$(FPL_ROOT)/flatbuffers
# Location of the Pindrop library.
DEPENDENCIES_PINDROP_DIR?=$(FPL_ROOT)/pindrop
# Location of the fplbase library.
DEPENDENCIES_FPLBASE_DIR?=$(FPL_ROOT)/fplbase
# Location of the fplutil library.
DEPENDENCIES_FPLUTIL_DIR?=$(FPL_ROOT)/fplutil
# Location of the FlatUI library.
DEPENDENCIES_FLATUI_DIR?=$(FPL_ROOT)/flatui
# Location of the MathFu library.
DEPENDENCIES_MATHFU_DIR?=$(FPL_ROOT)/mathfu
# Location of the Scene Lab library.
DEPENDENCIES_SCENE_LAB_DIR?=$(FPL_ROOT)/scene_lab
# Location of the Motive library.
DEPENDENCIES_MOTIVE_DIR?=$(FPL_ROOT)/motive
# Location of the Google Play Games library.
DEPENDENCIES_GPG_DIR?=$(PREBUILTS_ROOT)/gpg-cpp-sdk/android
# Location of the webp library.
DEPENDENCIES_WEBP_DIR?=$(THIRD_PARTY_ROOT)/webp
# Location of the Bullet Physics library.
DEPENDENCIES_BULLETPHYSICS_DIR?=$(THIRD_PARTY_ROOT)/bulletphysics
# Location of the Cardboard library.
DEPENDENCIES_CARDBOARD_DIR?=$(PREBUILTS_ROOT)/cardboard-java/CardboardSample

ifeq (,$(DETERMINED_DEPENDENCY_DIRS))
DETERMINED_DEPENDENCY_DIRS:=1
$(eval DEPENDENCIES_DIR_VALUE:=$$(DEPENDENCIES_$(DEP_DIR)_DIR))
print_dependency:
	@echo $(abspath $(DEPENDENCIES_DIR_VALUE))
endif
