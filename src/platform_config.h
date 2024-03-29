#pragma once

//set PLAYCHAIN_LIB_FOR_### macro

#if defined(ANDROID) // PLAYCHAIN_LIB_FOR_ANDROID
#if !defined(PLAYCHAIN_LIB_FOR_ANDROID)
#define PLAYCHAIN_LIB_FOR_ANDROID 1
#endif
#if !defined(PLAYCHAIN_LIB_FOR_MOBILE)
#define PLAYCHAIN_LIB_FOR_MOBILE 1
#endif
#elif defined(PLATFORM_IOS) // PLAYCHAIN_LIB_FOR_IOS
#if !defined(PLAYCHAIN_LIB_FOR_IOS)
#define PLAYCHAIN_LIB_FOR_IOS 1
#endif
#if !defined(PLAYCHAIN_LIB_FOR_MOBILE)
#define PLAYCHAIN_LIB_FOR_MOBILE 1
#endif
#elif defined(_WINDOWS) || defined(_WIN32) // PLAYCHAIN_LIB_FOR_WINDOWS
#if !defined(PLAYCHAIN_LIB_FOR_WINDOWS)
#define PLAYCHAIN_LIB_FOR_WINDOWS 1
#endif
#endif
