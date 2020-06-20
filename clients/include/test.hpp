#include <test.grpc.pb.h>
class TestImpl final : public test::TestService::Service {
public:
    test::EmptyResult SetTab(test::ChangeTab changeTab);
private:
    test::TabIndex currentTab;
};