# - Find Judy Library
# Find hash-Tree dictionnary Judy Library

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

set(Judy_EXTRA_PREFIXES /usr/local /opt/local "$ENV{HOME}")
foreach(prefix ${Judy_EXTRA_PREFIXES})
	list(APPEND Judy_INCLUDE_PATHS "${prefix}/include")
	list(APPEND Judy_LIB_PATHS "${prexix}/lib")
endforeach()

find_path(JUDY_INCLUDE_DIR Judy.h PATHS ${Judy_INCLUDE_PATHS})
find_library(JUDY_LIBRARY Judy PATHS ${Judy_LIB_PATHS})

set(Judy_MSG "Need Judy C Library : sourceforge.net/projects/judy/")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Judy ${Judy_MSG} JUDY_INCLUDE_DIR JUDY_LIBRARY)


