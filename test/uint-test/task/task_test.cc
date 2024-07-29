#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <algorithm>

#include "task/task_coordinator.h"
#include "test_task_manager.h"
#include "uint-test/dep/framework_engine_for_test.h"

using namespace std;

class CoordinatorSimpleTest : public testing::Test
{

protected:
    TaskManagerForTest *tmft = new TaskManagerForTest();
    shared_ptr<TaskManager> tm{tmft};
    TaskCoordinator *cor;
    shared_ptr<NetFrameworkEngineForTest> neg_;
    shared_ptr<TimerFrameworkEngineForTest> teg_;

    void SetUp() override
    {
        cor = new TaskCoordinator(tm);

        shared_ptr<ResourceManager> rm = make_shared<ResourceManagerForTest>();
        cor->setResourceManager(rm);

        neg_ = make_shared<NetFrameworkEngineForTest>();
        Netframework::init(neg_);

        teg_ = make_shared<TimerFrameworkEngineForTest>();
        TimerFramework::init(teg_);

        printf("SetUp \n");
    }

    void TearDown() override
    {
        printf("TearDown \n");
        delete cor;
    }

public:
    /**
     * @brief simple test
     *
     * @param ctx_num
     * @param uri
     * @param size
     */
    void simple_case(uint16_t ctx_num, string uri, uint64_t size)
    {
        auto na = make_shared<NetworkAdapter>();
        vector<std::shared_ptr<NetworkContext>> ctxs;

        for (size_t i = 0; i < ctx_num; i++)
        {
            cor->add_resource(uri, Range(0, size), {na, NetAddr(to_string(i),TransportType::UDP)});
        }

        ASSERT_EQ(ctx_num, tmft->tasks_ctx_num(uri));

        uint64_t blk_nums = (size / BLOCK_SIZE + 1);
        uint64_t offset = 0;

        cor->tick(1000 * 60);

        ASSERT_GT(tm->downloadNum(), 0);

        uint i = 0;
        if (size / BLOCK_SIZE > DOWNLOAD_LIMIT)
        {
            for (; i < DOWNLOAD_LIMIT; i++)
            {
                uint64_t end = min(size, offset + BLOCK_SIZE);
                tm->success(uri, Block(offset, end));
                offset = end;
                if (offset >= size)
                    break;
            }

            ASSERT_FALSE(tm->isSuccess(uri));
        }

        for (; i < blk_nums; i++)
        {
            uint64_t end = min(size, offset + BLOCK_SIZE);
            tm->success(uri, Block(offset, end));
            offset = end;
            if (offset >= size)
                break;
        }

        ASSERT_TRUE(tm->isSuccess(uri));
    }

    /**
     * @brief timeout test
     *
     * @param ctx_num
     * @param uri
     * @param size
     */
    void timeout_case(uint16_t ctx_num, string uri, uint64_t size)
    {
        auto na = make_shared<NetworkAdapter>();
        vector<std::shared_ptr<NetworkContext>> ctxs;

        for (size_t i = 0; i < ctx_num; i++)
        {
            cor->add_resource(uri, Range(0, size), {na, NetAddr(to_string(i),TransportType::UDP)});
        }

        ASSERT_EQ(ctx_num, tmft->tasks_ctx_num(uri));

        vector<Task> old_tasks = tmft->tasks(uri);
        if (old_tasks.size() <= DOWNLOAD_LIMIT)
        {
            for (uint i = 0; i < 7; i++)
                cor->tick(1000 * 60);

            vector<Task> tasks = tmft->tasks(uri);
            ASSERT_EQ(old_tasks.size(), tasks.size());
            for (size_t i = 0; i < tasks.size(); i++)
                ASSERT_NE(old_tasks[i].getNetCtx().getPeer().str(), tasks[i].getNetCtx().getPeer().str());
        }
        else
        {
            for (size_t i = 0; i < old_tasks.size(); i += DOWNLOAD_LIMIT)
            {
                for (uint i = 0; i < 7; i++)
                {
                    cor->tick(1000 * 60);
                }
                for (size_t j = 0; j < DOWNLOAD_LIMIT; j++)
                {
                    uint64_t start = Block::bitPos(i + j);
                    tm->success(uri, Block(start, start + BLOCK_SIZE));
                }
            }

            vector<Task> tasks = tmft->tasks(uri);
            ASSERT_EQ(old_tasks.size(), tasks.size());
            for (size_t i = 0; i < tasks.size(); i++)
                ASSERT_NE(old_tasks[i].getNetCtx().getPeer().str(), tasks[i].getNetCtx().getPeer().str()) << "i:" << i;
        }
    }

    void ctx_join(uint16_t ctx_num, string uri, uint64_t size)
    {
        size = min(static_cast<uint64_t>(SIZE_1MByte * 1024), size);

        auto na = make_shared<NetworkAdapter>();

        uint64_t pos = 0;
        for (size_t i = 0; i < ctx_num; i++)
        {
            cor->add_resource(uri, Range(0, size), {na, NetAddr(to_string(i), TransportType::UDP)});

            cor->tick(1000 * 60);

            for (; pos < 5;)
            {
                uint64_t start = Block::bitPos(pos++);
                tm->success(uri, Block(start, start + BLOCK_SIZE));
            }
        }

        while (!tm->isSuccess(uri))
        {
            uint64_t start = Block::bitPos(pos++);
            tm->success(uri, Block(start, start + BLOCK_SIZE));
        }

        ASSERT_EQ(ctx_num, tmft->tasks_ctx_num(uri));
    }
};

TEST_F(CoordinatorSimpleTest, single_ctx_block_blk_nums_lt_download_limit)
{
    simple_case(1, "rs1", SIZE_1MByte * 50);
}
TEST_F(CoordinatorSimpleTest, single_ctx_block_blk_nums_gt_download_limit)
{
    simple_case(1, "rs1", SIZE_1MByte * 1024);
}

TEST_F(CoordinatorSimpleTest, many_ctx_block_blk_nums_lt_download_limit)
{
    simple_case(10, "rs1", SIZE_1MByte * 50);
}

TEST_F(CoordinatorSimpleTest, many_ctx_block_blk_nums_gt_download_limit)
{
    simple_case(10, "rs1", SIZE_1MByte * 1024);
}

TEST_F(CoordinatorSimpleTest, timout)
{
    timeout_case(10, "rs1", SIZE_1MByte * 50);
}

TEST_F(CoordinatorSimpleTest, timout_gt_download_limit)
{
    timeout_case(10, "rs1", SIZE_1MByte * 1024);
}

TEST_F(CoordinatorSimpleTest, ctx_join_simple)
{
    ctx_join(10, "rs1", SIZE_1MByte * 1024);
}
