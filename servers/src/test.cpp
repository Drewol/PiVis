#include "test.hpp"
#include <test.grpc.pb.h>

using test::EmptyResult;
using test::TabIndex;
test::EmptyResult TestImpl::SetTab(test::ChangeTab changeTab) {
    printf("Changing %d->%d\n", currentTab, changeTab.tab());
    currentTab = changeTab.tab();
    return test::EmptyResult();
}
