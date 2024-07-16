#include <gtest/gtest.h>

#include <sstream>
#include <algorithm>

#include "resource_manager.h"
#include "dep/test_common.h"

using namespace std;



TEST(ResourceSerializer, serialize_single_resource)
{
    Resource r("string", "java.util.string", "other", "1234567890abcdefg", 18);
    vector<Resource> v{r};

    auto dto = ResourceSerializer::serialize(v);

    auto result = ResourceSerializer::deserialize(dto.data, dto.size);
    ASSERT_EQ(v.size(), result.size());
    for (size_t i = 0; i < v.size(); i++)
    {
        ASSERT_EQ(v.at(i), result.at(i));
    }
}

TEST(ResourceSerializer, serialize_single_large_uint16t_resource)
{
    Resource r{genStr(65000), genStr(65000), genStr(65000), genStr(65000), 65000};
    vector<Resource> v{r};

    auto dto = ResourceSerializer::serialize(v);

    auto result = ResourceSerializer::deserialize(dto.data, dto.size);
    ASSERT_EQ(v.size(), result.size());
    for (size_t i = 0; i < v.size(); i++)
    {
        ASSERT_EQ(v.at(i), result.at(i));
    }
}

TEST(ResourceSerializer, serialize_single_large_uint32t_resource)
{
    Resource r{genStr(655350), genStr(655350), genStr(655350), genStr(655350), 655350};
    vector<Resource> v{r};

    auto dto = ResourceSerializer::serialize(v);

    auto result = ResourceSerializer::deserialize(dto.data, dto.size);
    ASSERT_EQ(v.size(), result.size());
    for (size_t i = 0; i < v.size(); i++)
    {
        ASSERT_EQ(v.at(i), result.at(i));
    }
}

TEST(ResourceSerializer, serialize_many_resource)
{
    vector<Resource> v{};

    auto vecsize = 100;

    for (size_t i = 0; i < vecsize; i++)
    {
        if (i % 2 == 0)
            v.push_back({genStr(655350), genStr(655350), genStr(655350), genStr(655350), 655350});
        else
            v.push_back({genStr(65000), genStr(65000), genStr(65000), genStr(65000), 65000});
    }

    auto dto = ResourceSerializer::serialize(v);

    auto result = ResourceSerializer::deserialize(dto.data, dto.size);
    ASSERT_EQ(vecsize, v.size());
    ASSERT_EQ(v.size(), result.size());
    for (size_t i = 0; i < v.size(); i++)
    {
        ASSERT_EQ(v.at(i), result.at(i));
    }
}