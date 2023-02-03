#include <gtest/gtest.h>

#include "marketPacketGenerator/marketPacketGenerator.h"
#include "marketPacketProcessor/marketPacketProcessor.h"

namespace test
{
    // Ideally, this goes into a config file
    const std::string GENERATE_PATH = "./generate_test.dat";
    const std::string OUTPUT_PATH = "./output_test.dat";
    constexpr const size_t MANY_PACKETS = 1000;

    marketPacket::marketPacketGenerator_t createDefaultGenerator()
    {
        return marketPacket::marketPacketGenerator_t(std::ofstream{GENERATE_PATH});
    }

    marketPacket::marketPacketProcessor_t createDefaultProcessor()
    {
        return marketPacket::marketPacketProcessor_t(std::ifstream{GENERATE_PATH}, std::ofstream{OUTPUT_PATH});
    }

    TEST(marketPacketGeneratorTest, noInit)
    {
        marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
        EXPECT_EQ(mpg.generatePackets(1, 1).value(), marketPacket::UNINITIALIZED);
    }

    TEST(marketPacketGeneratorTest, doubleInit)
    {
        marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
        mpg.initialize();

        EXPECT_DEATH(mpg.initialize(), "");
    }

    TEST(marketPacketGeneratorTest, tooManyUpdates)
    {
        EXPECT_EQ(createDefaultGenerator().generatePackets(1, std::numeric_limits<size_t>::max()).value(), marketPacket::TOO_MANY_UPDATES);
    }

    TEST(marketPacketGeneratorTest, onePacketoneUpdate)
    {
        {
            marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
            mpg.initialize();

            EXPECT_FALSE(mpg.generatePackets(1, 1).has_value());
        }

        // Make sure we wrote exactly the bytes we expected
        size_t expectedSize = sizeof(marketPacket::packetHeader_t) + sizeof(marketPacket::update_t);
        EXPECT_EQ(std::ifstream(GENERATE_PATH, std::ifstream::ate | std::ifstream::binary).tellg(), expectedSize);

        // This inherently checks validity of the the underlying data
        marketPacket::marketPacketProcessor_t mpp = createDefaultProcessor();
        mpp.initialize();

        EXPECT_FALSE(mpp.processNextPacket(1).has_value());
        EXPECT_EQ(mpp.processNextPacket(1).value(), marketPacket::END_OF_FILE);
    }

    TEST(marketPacketGeneratorTest, onePacketManyUpdate)
    {
        {
            marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
            mpg.initialize();

            EXPECT_FALSE(mpg.generatePackets(1, marketPacket::MAX_UPDATES_ALLOWED_IN_PACKET).has_value());
        }

        // Since we don't know exactly the number of packets, we can only check within a bound
        size_t maxSize = sizeof(marketPacket::packetHeader_t) + marketPacket::MAX_UPDATES_ALLOWED_IN_PACKET * sizeof(marketPacket::update_t);
        EXPECT_LE(std::ifstream(GENERATE_PATH, std::ifstream::ate | std::ifstream::binary).tellg(), maxSize);

        marketPacket::marketPacketProcessor_t mpp = createDefaultProcessor();
        mpp.initialize();

        EXPECT_FALSE(mpp.processNextPacket(1).has_value());
        EXPECT_EQ(mpp.processNextPacket(1).value(), marketPacket::END_OF_FILE);
    }

    TEST(marketPacketGeneratorTest, manyPacketOneUpdate)
    {
        {
            marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
            mpg.initialize();

            EXPECT_FALSE(mpg.generatePackets(MANY_PACKETS, 1).has_value());
        }

        size_t expectedSize = MANY_PACKETS * (sizeof(marketPacket::packetHeader_t) + sizeof(marketPacket::update_t));
        EXPECT_EQ(std::ifstream(GENERATE_PATH, std::ifstream::ate | std::ifstream::binary).tellg(), expectedSize);

        marketPacket::marketPacketProcessor_t mpp = createDefaultProcessor();
        mpp.initialize();

        EXPECT_FALSE(mpp.processNextPacket(MANY_PACKETS).has_value());
        EXPECT_EQ(mpp.processNextPacket(1).value(), marketPacket::END_OF_FILE);
    }

    TEST(marketPacketGeneratorTest, manyPacketManyUpdate)
    {
        {
            marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
            mpg.initialize();

            EXPECT_FALSE(mpg.generatePackets(MANY_PACKETS, marketPacket::MAX_UPDATES_ALLOWED_IN_PACKET).has_value());
        }

        // We can only check an upper bound
        size_t expectedSize = MANY_PACKETS * (sizeof(marketPacket::packetHeader_t) + marketPacket::MAX_UPDATES_ALLOWED_IN_PACKET * sizeof(marketPacket::update_t));
        EXPECT_LE(std::ifstream(GENERATE_PATH, std::ifstream::ate | std::ifstream::binary).tellg(), expectedSize);

        marketPacket::marketPacketProcessor_t mpp = createDefaultProcessor();
        mpp.initialize();

        EXPECT_FALSE(mpp.processNextPacket(MANY_PACKETS).has_value());
        EXPECT_EQ(mpp.processNextPacket(1).value(), marketPacket::END_OF_FILE);
    }

    TEST(marketPacketGeneratorTest, multipleCalls)
    {
        constexpr const size_t NUM_CALLS = 5;

        {
            marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
            mpg.initialize();

            for (size_t i = 0; i < NUM_CALLS; i++)
            {
                EXPECT_FALSE(mpg.generatePackets(1, 1).has_value());
            }
        }

        // We can only check an upper bound
        size_t expectedSize = NUM_CALLS * (sizeof(marketPacket::packetHeader_t) + sizeof(marketPacket::update_t));
        EXPECT_LE(std::ifstream(GENERATE_PATH, std::ifstream::ate | std::ifstream::binary).tellg(), expectedSize);

        marketPacket::marketPacketProcessor_t mpp = createDefaultProcessor();
        mpp.initialize();

        EXPECT_FALSE(mpp.processNextPacket(NUM_CALLS).has_value());
        EXPECT_EQ(mpp.processNextPacket(1).value(), marketPacket::END_OF_FILE);
    }

    /**
     * This is a weird case of two classes verifying the other.
     * Past basic tests, we assume basic functionality works at scale for the processor for this test.
     */
    TEST(marketPacketGeneratorTest, genericStressTest_long)
    {
        // Should take about a minute
        constexpr const size_t NUM_PACKETS = 40000;

        {
            marketPacket::marketPacketGenerator_t mpg = createDefaultGenerator();
            mpg.initialize();

            EXPECT_FALSE(mpg.generatePackets(NUM_PACKETS, marketPacket::MAX_UPDATES_ALLOWED_IN_PACKET).has_value());
        }

        marketPacket::marketPacketProcessor_t mpp = createDefaultProcessor();
        mpp.initialize();

        EXPECT_FALSE(mpp.processNextPacket(NUM_PACKETS).has_value());
        EXPECT_EQ(mpp.processNextPacket(1).value(), marketPacket::END_OF_FILE);
    }
}
