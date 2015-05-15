# - Find ThinLinc
# Find ThinLinc Environment
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

find_path(THINLINC_DIR NAMES thinlinc-release
	PATHS $ENV{TL_PREFIX}/etc/
	DOC "Thinlinc Install Directory"
)

set(THINLINC_MSG "Thinlinc Install directory not found")
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ThinLinc THINLINC_MSG THINLINC_DIR)

