/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * X11 Keyboard Mapping in ThinLinc Environment
 *
 * Copyright 2014 Vincent Sourin <sourin-v@bridgestone-bae.com>
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

#ifndef XF_KEYBOARD_THINLINC_H_
#define XF_KEYBOARD_THINLINC_H_

#define THINLINC_EXTENDED 0x80
#define THINLINC_RDP_SCANCODE_EXTENDED(_rdp_scancode) (((_rdp_scancode) & THINLINC_EXTENDED) ? TRUE : FALSE)
#define RDP_SCANCODE_ALTGR (THINLINC_EXTENDED | 0x38)

void xf_keyboard_key_press_thinlinc(xfContext* xfc, BYTE keycode, KeySym keysym);
void xf_keyboard_key_release_thinlinc(xfContext* xfc, BYTE keycode, KeySym keysym);

#endif /* XF_KEYBOARD_THINLINC_H_ */
