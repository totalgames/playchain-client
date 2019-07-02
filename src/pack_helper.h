#pragma once

#include "pack_to_raw_helper.h"
#include "pack_to_json_helper.h"

#define PACK_TEMPLATE_DECLARE(object_type)                                               \
    template <typename Stream>                                                           \
    inline void pack(Stream& s, const object_type& obj, uint32_t depth = PACK_MAX_DEPTH) \
    {                                                                                    \
        assert(depth > 0);                                                               \
        obj.pack_object(s);                                                              \
    }
