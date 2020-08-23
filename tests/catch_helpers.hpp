#define FMT_INFO(...) INFO(fmt::format(__VA_ARGS__))
#define FMT_UINFO(...) UNSCOPED_INFO(fmt::format(__VA_ARGS__))
#define CHECK_MESSAGE(cond, ...) do { FMT_INFO(__VA_ARGS__); CHECK(cond); } while((void)0, 0)
#define REQUIRE_MESSAGE(cond, ...) do { FMT_INFO(__VA_ARGS__); REQUIRE(cond); } while((void)0, 0)
