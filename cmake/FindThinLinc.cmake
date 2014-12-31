# - Find ThinLinc
# Find ThinLinc keymaps directory
#
#  This module defines the following variables:
#     THINLINC_FOUND        - true if THINLINC_KEYMAPS_DIR is found
#     THINLINC_KEYMAPS_DIRS - Set when THINLINC_KEYMAPS_DIR is found
#
#     THINLINC_KEYMAPS_DIR  - where to find keymaps
#

#=============================================================================
# Copyright 2014 Vincent Sourin <sourin-v@bridgestone-bae.com>
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
#=============================================================================

find_path(THINLINC_KEYMAPS_DIR NAMES common
	  PATHS /opt/thinlinc/share/rdesktop/keymaps/
          DOC "Thinlinc keymaps directory"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ThinLinc DEFAULT_MSG THINLINC_KEYMAPS_DIR)

if(THINLINC_FOUND)
  set( THINLINC_KEYMAPS_DIRS ${THINLINC_KEYMAPS_DIR})
endif()
mark_as_advanced(THINLINC_KEYMAPS_DIR)
