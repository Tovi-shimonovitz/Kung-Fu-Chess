#include <chrono>
#include <iostream>
#include <thread>
#include "../../include/network/client/network_client.h"
#include "../../include/network/protocol/messages.h"
#include "../../include/graphics/board/board_renderer.h"
#include "../../include/graphics/sprites/sprite_repository.h"

void pump(NetworkClient& a, NetworkClient& b, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        a.poll();
        b.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    NetworkClient a;
    NetworkClient b;
    a.connect("ws://localhost:9002");
    b.connect("ws://localhost:9002");
    pump(a, b, 20);

    a.send(RegisterMessage("diagA", "pw"));
    b.send(RegisterMessage("diagB", "pw"));
    pump(a, b, 5);

    a.send(PlayRequestMessage());
    b.send(PlayRequestMessage());

    std::cout << "Waiting for a snapshot..." << std::endl;
    pump(a, b, 40);

    auto snapshot = a.latestSnapshot();
    if (!snapshot) {
        std::cout << "No snapshot received - cannot diagnose rendering." << std::endl;
        return 1;
    }

    std::cout << "Got snapshot with " << snapshot->pieces.size() << " pieces." << std::endl;

    SpriteRepository spriteRepository("third_party/images");
    BoardRenderer boardRenderer("third_party/images/board.png", spriteRepository);

    try {
        Img frame = boardRenderer.render(*snapshot, 16);
        std::cout << "Render SUCCEEDED." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Render FAILED: " << e.what() << std::endl;
        for (size_t idx = 0; idx < snapshot->pieces.size(); ++idx) {
            const auto& p = snapshot->pieces[idx];
            std::cout << "  piece " << idx << " id=" << p.id << " color=" << static_cast<int>(p.color)
                      << " kind=" << static_cast<int>(p.kind) << " state=" << static_cast<int>(p.state)
                      << " pos=(" << p.currentPos.row << "," << p.currentPos.col << ")" << std::endl;
        }
        return 1;
    }

    return 0;
}
