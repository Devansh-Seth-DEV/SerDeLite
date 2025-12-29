#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>
#include <serdelite.h>

using namespace std;
using namespace serdelite;

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
    PlayerStats pStat(100, 5000, 42);

    uint8_t mem[512];
    ByteBuffer buffer(mem, sizeof(mem));
    ByteStream stream(buffer);

    if(testLoop(1000000, buffer, stream, pStat)) return 1;

    PlayerStats obj;
    bool success = stream.readObject(obj);
    if (!success) cout << "Failed to deserialize stream!\n";
    else {
        cout << "---- Benchmark complete ----\n";
        cout << "Succesfully deserialize stream\n";
    }

    return 0;
}