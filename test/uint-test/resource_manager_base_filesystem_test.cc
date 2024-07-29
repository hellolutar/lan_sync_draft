#include <gtest/gtest.h>

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>

#include "resource_manager.h"

using namespace std;

class ResourceManagerBaseFilesystemTest : public testing::Test
{
    void SetUp() override
    {
    }

    void TearDown() override {

    };

    void genFile(std::vector<std::string> fs)
    {
        for (auto &&fstr : fs)
        {
            std::filesystem::path fp(fstr);
            auto endIdx = fstr.find_last_of("/");
            std::filesystem::path parentp(fstr.substr(0, endIdx));
            if (!std::filesystem::exists(parentp))
                std::filesystem::create_directories(parentp);

            std::ofstream f(fstr);
            f << "hello world" << std::endl;
        }
    }

    void clearFile(std::vector<std::string> fs)
    {
        for (auto &&fstr : fs)
        {
            std::filesystem::path fp(fstr);
            if (filesystem::exists(fp))
                filesystem::remove_all(fp);
        }
    }

public:
    void idx(std::vector<std::string> expected, std::string relative_home)
    {
        try
        {
            genFile(expected);
            auto rshome = filesystem::current_path().string() + "/" + relative_home;
            ResourceManagerBaseFilesystem rmbf(rshome);
            auto table = rmbf.idx();

            ASSERT_EQ(expected.size(), table.size());

            map<string, string> dic;
            for (auto &&r : table)
                dic[r.getUri()] = r.getPath();

            for (auto &&str : expected)
            {
                auto uri = str.substr(relative_home.size());
                ASSERT_TRUE(dic.find(uri) != dic.end());
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            clearFile(expected);
        }
        clearFile(expected);
    }
};

TEST_F(ResourceManagerBaseFilesystemTest, idx)
{

    vector<string> c1 = {"demo.d/sub.d/1_1_1.txt",
                         "demo.d/sub.d/1_1_2.txt",
                         "demo.d/sub2.d/1_1_2.txt",
                         "demo.d/1.txt",
                         "demo.d/2.txt"};
    idx(c1, "demo.d");
}