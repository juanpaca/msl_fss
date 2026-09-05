#include <catch2/catch.hpp>

#include "msl_fss/Infrastructure/OfflineStore.hpp"
#include "msl_fss/Infrastructure/Matrix.hpp"

using namespace msl_fss;

TEST_CASE("OfflineStore stores and fetches contributions by key",
          "[unit][offlinestore]") {
    OfflineStore store;
    REQUIRE(store.Size() == 0);

    DenseMatrix M(2, 2);
    M(0, 0) = 1.0; M(0, 1) = 2.0;
    M(1, 0) = 3.0; M(1, 1) = 4.0;

    store.Store(42, M);
    REQUIRE(store.Size() == 1);
    REQUIRE(store.Contains(42));

    const DenseMatrix& got = store.Fetch(42);
    REQUIRE(got(0, 0) == Approx(1.0));
    REQUIRE(got(1, 1) == Approx(4.0));

    store.Store(7, DenseMatrix(1, 1));
    REQUIRE(store.Size() == 2);
}

TEST_CASE("OfflineStore clear resets state", "[unit][offlinestore]") {
    OfflineStore store;
    store.Store(1, DenseMatrix(1, 1));
    store.Store(2, DenseMatrix(1, 1));
    REQUIRE(store.Size() == 2);
    store.Clear();
    REQUIRE(store.Size() == 0);
    REQUIRE_FALSE(store.Contains(1));
}

TEST_CASE("OfflineStore fetch of missing key throws out_of_range",
          "[unit][offlinestore]") {
    OfflineStore store;
    REQUIRE_THROWS_AS(store.Fetch(99), std::out_of_range);
}