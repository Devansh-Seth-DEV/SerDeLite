/*
 * SerDeLite
 * Copyright (C) 2025 Devansh Seth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */

#include "serdelite/Serializable.h"
#include "serdelite/JsonStream.h"

namespace serdelite {

bool JsonSerializable::toJson(JsonStream& stream) const {
    if (!serializeToJson(stream)) return false;
    return stream.close();
}

}