#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>
#include <serdelite.h>

using namespace std;
using namespace serdelite;

class Vec3: public ByteSerializable {
public:
    float x, y, z;

    Vec3(): x(0), y(0), z(0) {}

    Vec3(float _x, float _y, float _z)
        : x(_x), y(_y), z(_z) {}

    bool toByteStream(ByteStream& s) const override {
        return s.writeFloat(x) &&
               s.writeFloat(y) &&
               s.writeFloat(z);
    }

    bool fromByteStream(ByteStream& s) override {
        return s.readFloat(x) &&
               s.readFloat(y) &&
               s.readFloat(z);
    }

    size_t byteSize() const override {
        return sizeof(x) + sizeof(y) + sizeof(z);
    }
};


class PlayerStats: public ByteSerializable {
public:
    uint32_t health;
    uint32_t xp;
    uint16_t level;

    PlayerStats(): health(0), xp(0), level(0) {}

    PlayerStats(uint32_t _health, uint32_t _xp, uint16_t _level)
        : health(_health), xp(_xp), level(_level) {}

    bool toByteStream(ByteStream& s) const override {
        return s.writeUint32(health) &&
               s.writeUint32(xp) &&
               s.writeUint16(level);
    }

    bool fromByteStream(ByteStream& s) override {
        return s.readUint32(health) &&
               s.readUint32(xp) &&
               s.readUint16(level);
    }

    size_t byteSize() const override {
        return sizeof(health) + sizeof(xp) + sizeof(level);
    }
};

class Player: public ByteSerializable {
private:
    static const uint8_t MAX_USERNAME_LEN = 16U;
    uint8_t usernameLen;

public:
    uint64_t playerId;
    char username[MAX_USERNAME_LEN];
    Vec3 position;
    PlayerStats stats;
    bool isOnline;

    Player(): playerId(0), isOnline(false), usernameLen(0) {
        username[0] = '\0';
    }

    Player(uint64_t _playerId,
           const char* _username,
           Vec3& _position,
           PlayerStats& _stats,
           bool _isOnline = false)
        : playerId(_playerId),
          position(_position), stats(_stats),
          isOnline(_isOnline) {
        strncpy(username, _username, 16);
        usernameLen = strlen(username);
    }

    bool toByteStream(ByteStream& s) const override {
        return s.writeUint64(playerId) &&
               s.writeString(username) &&
               s.writeObject(position) &&
               s.writeObject(stats) &&
               s.writeBool(isOnline);
    }

    bool fromByteStream(ByteStream& s) override {
        return s.readUint64(playerId) &&
               s.readString(username, MAX_USERNAME_LEN) &&
               s.readObject(position) &&
               s.readObject(stats) &&
               s.readBool(isOnline);
    }

    size_t byteSize() const override {
        return sizeof(playerId) +
               sizeof(uint16_t) + usernameLen +
               position.byteSize() + stats.byteSize() +
               sizeof(isOnline);
    }
};

class InventoryItem : public ByteSerializable {
public:
    uint32_t itemId;
    uint16_t quantity;
    uint8_t quality;

    InventoryItem(): itemId(0), quantity(0), quality(0) {}

    InventoryItem(uint32_t _itemId,
                  uint16_t _quantity,
                  uint8_t _quality)
        : itemId(_itemId), quantity(_quantity), quality(_quality) {}

    bool toByteStream(ByteStream& s) const override {
        return s.writeUint32(itemId) &&
               s.writeUint16(quantity) &&
               s.writeUint8(quality);
    }

    bool fromByteStream(ByteStream& s) override {
        return s.readUint32(itemId) &&
               s.readUint16(quantity) &&
               s.readUint8(quality);
    }

    size_t byteSize() const override {
        return sizeof(itemId) + sizeof(quantity) + sizeof(quality);
    }
};

class ComplexPlayer : public ByteSerializable {
public:
    Player baseProfile;
    InventoryItem inventory[10];

    ComplexPlayer() {}
    
    ComplexPlayer(Player& p, InventoryItem _inventory[])
        : baseProfile(p) {
        for(int i=0; i<10; i++) inventory[i] = _inventory[i];
    }

    bool toByteStream(ByteStream& s) const override {
        bool success = s.writeObject(baseProfile);
        if (!success) return false;

        for(int i = 0; i < 10; ++i) {
            success &= s.writeObject(inventory[i]);
            if (!success) return false;
        }

        return true;
    }

    bool fromByteStream(ByteStream& s) override {
        bool success = s.readObject(baseProfile);
        if (!success) return false;

        for(int i = 0; i < 10; ++i) {
            success &= s.readObject(inventory[i]);
            if (!success) return false;
        }

        return true;
    }

    size_t byteSize() const override {
        InventoryItem item;
        return baseProfile.byteSize() +
               (10 * item.byteSize());
    }
};

int testLoop(const int iterations,
             ByteBuffer& buffer, ByteStream& s, ByteSerializable& obj) {
    // Warm-up ensures the CPU is at max speed and the caches are primed.
    for(int i=0; i < 100000; i++) {
        buffer.clear();
        s.writeObject(obj);
    }

    cout << "Starting Benchmark: " << iterations << " iterations...\n";

    auto start = chrono::high_resolution_clock::now();

    for(int i=0; i<iterations; i++) {
        buffer.clear();
        if (!s.writeObject(obj)) {
            cerr << "Serialization failed at " << i << endl;
            return 1;
        }
    }

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> diff = end - start;
    double operPerSec = iterations / diff.count();

    cout << "<---- Results ---->\n";
    cout << "Total Time: " << diff.count() << "secs\n";
    cout << "Throughput: " << operPerSec / 1e6 << "Million objects/sec\n";
    cout << "Latency: " << (diff.count() * 1e9) / iterations << "ns per object\n";

    return 0;
}


int main() {
    Vec3 v3(10.5f, 20.0f, -5.5f);
    PlayerStats pStat(100, 5000, 42);
    Player p(12345, "Devansh", v3, pStat, true);

    InventoryItem inventory[10];
    for(int i=0; i<10; i++) {
        inventory[i] = InventoryItem(i+1, i+2, 4);
    }

    ComplexPlayer complexP(p, inventory);

    uint8_t mem[512];
    ByteBuffer buffer(mem, sizeof(mem));
    ByteStream stream(buffer);

    if(testLoop(1000000, buffer, stream, complexP)) return 1;

    ComplexPlayer obj;
    bool success = stream.readObject(obj);
    if (!success) cout << "Failed to deserialize stream!\n";
    else {
        cout << "---- Benchmark complete ----\n";
        cout << "Succesfully deserialize stream\n";
    }

    return 0;
}