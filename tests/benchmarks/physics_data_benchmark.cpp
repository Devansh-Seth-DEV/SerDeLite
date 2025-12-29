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

    uint8_t mem[512];
    ByteBuffer buffer(mem, sizeof(mem));
    ByteStream stream(buffer);

    if(testLoop(1000000, buffer, stream, v3)) return 1;

    Vec3 obj;
    bool success = stream.readObject(obj);
    if (!success) cout << "Failed to deserialize stream!\n";
    else {
        cout << "---- Benchmark complete ----\n";
        cout << "Succesfully deserialize stream\n";
    }

    return 0;
}