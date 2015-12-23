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

#include <EGL/egl.h>
#include <EGL/eglext.h>

extern "C" {

EGLBoolean EGLAPIENTRY eglQuerySurfacePointerANGLE(EGLDisplay dpy,
                                                   EGLSurface surface,
                                                   EGLint attribute,
                                                   void** value) {
  return false;
}

EGLBoolean EGLAPIENTRY eglPostSubBufferNV(EGLDisplay dpy,
                                          EGLSurface surface,
                                          EGLint x,
                                          EGLint y,
                                          EGLint width,
                                          EGLint height) {
  return false;
}

EGLDisplay EGLAPIENTRY eglGetPlatformDisplayEXT(EGLenum platform,
                                                void* native_display,
                                                const EGLint* attrib_list) {
  return EGL_NO_DISPLAY;
}

EGLBoolean EGLAPIENTRY eglQueryDisplayAttribEXT(EGLDisplay dpy,
                                                EGLint attribute,
                                                EGLAttrib* value) {
  return false;
}

EGLBoolean EGLAPIENTRY eglQueryDeviceAttribEXT(EGLDeviceEXT device,
                                               EGLint attribute,
                                               EGLAttrib* value) {
  return false;
}

const char* EGLAPIENTRY eglQueryDeviceStringEXT(EGLDeviceEXT device,
                                                EGLint name) {
  return NULL;
}

EGLImageKHR EGLAPIENTRY eglCreateImageKHR(EGLDisplay dpy,
                                          EGLContext ctx,
                                          EGLenum target,
                                          EGLClientBuffer buffer,
                                          const EGLint* attrib_list) {
  return 0;
}

EGLBoolean EGLAPIENTRY eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image) {
  return false;
}

EGLDeviceEXT EGLAPIENTRY eglCreateDeviceANGLE(EGLint device_type,
                                              void* native_device,
                                              const EGLAttrib* attrib_list) {
  return 0;
}

EGLBoolean EGLAPIENTRY eglReleaseDeviceANGLE(EGLDeviceEXT device) {
  return false;
}

}  // extern "C"
