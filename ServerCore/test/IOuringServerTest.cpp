#include "../Network/include/UringCore.h"
#include "../Network/include/SocketUtil.h"
#include <gtest/gtest.h>

class IOuringServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        SocketAddress address(INADDR_ANY, 48000);
        server = new IOuringServer(address);
    }

    void TearDown() override {
        delete server;
    }

    UringCore* server;
};

TEST_F(IOuringServerTest, StartServer) {
    EXPECT_TRUE(server->start());
}