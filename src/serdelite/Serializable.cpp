#include "serdelite/Serializable.h"
#include "serdelite/JsonStream.h"

namespace serdelite {

bool JsonSerializable::toJson(JsonStream& stream) const {
    if (!serializeToJson(stream)) return false;
    return stream.close();
}

}