#include <stdexcept>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "behavior_tree/btree.hpp"

namespace {

BTree ParseJson(const char* raw) {
    return BTree::Parse(nlohmann::json::parse(raw));
}

}

TEST(BTree, ParseSequenceMoveTo) {
    const char* json = R"({
        "type": "sequence",
        "children": [
            {"type": "action", "move_to": {"lat": 48.1, "lon": 11.6, "alt_m": 50.0}},
            {"type": "action", "move_to": {"lat": 48.2, "lon": 11.7, "alt_m": 60.0}}
        ]
    })";

    BTree btree {ParseJson(json)};

    ASSERT_EQ(btree.GetMissionItems().size(), 2);
    EXPECT_DOUBLE_EQ(btree.GetMissionItems()[0].latitude_deg, 48.1);
    EXPECT_DOUBLE_EQ(btree.GetMissionItems()[0].longitude_deg, 11.6);
    EXPECT_FLOAT_EQ(btree.GetMissionItems()[0].relative_altitude_m, 50.0f);
    EXPECT_DOUBLE_EQ(btree.GetMissionItems()[1].latitude_deg, 48.2);
    EXPECT_DOUBLE_EQ(btree.GetMissionItems()[1].longitude_deg, 11.7);
}

TEST(BTree, ValidateInvalidTree) {
    const char* json = R"({
        "type": "sequence",
        "children": [
            {"type": "action", "compute_fibonacci": {"n": 50}},
            {"type": "action", "move_to": {"lat": 45.0, "lon": -73.0, "alt_m": 30.0}},
            {"type": "action", "compute_fibonacci": {"n": 0}}
        ]
    })";

    BTree btree {ParseJson(json)};
    EXPECT_FALSE(btree.Validate());
}

TEST(BTree, NestedParallelParseValidate) {
    const char* json = R"({
        "type": "sequence",
        "children": [
            {"type": "action", "move_to": {"lat": 10.0, "lon": 20.0, "alt_m": 30.0}},
            {
                "type": "parallel",
                "success_threshold": 1,
                "children": [
                    {"type": "action", "move_to": {"lat": 11.0, "lon": 21.0, "alt_m": 31.0}},
                    {"type": "action", "compute_fibonacci": {"n": 50}}
                ]
            }
        ]
    })";

    BTree btree {ParseJson(json)};

    ASSERT_EQ(btree.GetMissionItems().size(), 2);
    EXPECT_TRUE(btree.Validate());
    EXPECT_DOUBLE_EQ(btree.GetMissionItems()[0].latitude_deg, 10.0);
    EXPECT_DOUBLE_EQ(btree.GetMissionItems()[1].latitude_deg, 11.0);
}

TEST(BTree, UnknownTaskThrows) {
    const char* json = R"({"type": "action", "makeajoke": 10})";
    EXPECT_THROW(ParseJson(json), std::runtime_error);
}
