#include <catch2/catch.hpp>

#include "msl_fss/Infrastructure/PartitionInfo.hpp"
#include "msl_fss/Infrastructure/SharedMHMHierarchy.hpp"
#include "msl_fss/Types/Config.hpp"

using namespace msl_fss;

TEST_CASE("PartitionInfo reports rank-local range", "[unit][partition]") {
    PartitionInfo pi;
    pi.numElements = 10;
    pi.elementStart = 2;
    pi.elementEnd = 6;
    pi.rank = 1;

    REQUIRE(pi.GetNumElements() == 4);
    REQUIRE(pi.GetRange() == std::make_pair(std::size_t{2}, std::size_t{6}));
}

TEST_CASE("SharedMHMHierarchy builds a partition and sub-meshes",
          "[unit][hierarchy]") {
    Config config;
    config.set("numelems", "8");
    config.set("ndof_local", "3");

    SharedMHMHierarchy hierarchy;
    REQUIRE(hierarchy.numSubMeshes() == 0);

    hierarchy.Build(config);

    REQUIRE(hierarchy.GetPartition().numElements == 8);
    REQUIRE(hierarchy.GetPartition().elementStart == 0);
    REQUIRE(hierarchy.GetPartition().elementEnd == 8);
    REQUIRE(hierarchy.numSubMeshes() == 8);
    REQUIRE(hierarchy.GetSubMeshDofs(0) == 3);
    REQUIRE(hierarchy.GetSubMeshDofs(7) == 3);
}