cmake_minimum_required(VERSION 3.16)
project(HalfSlidingSketch)

set(CMAKE_CXX_STANDARD 14)

# Frequency
add_executable(ECM src/Frequency/ECM/demo/main.cpp)
target_link_libraries(ECM -lpthread)

add_executable(HalfSlidingSketch src/Frequency/HalfSlidingSketch/demo/main.cpp)
target_link_libraries(HalfSlidingSketch -lpthread)

add_executable(Sliding_Sketch src/Frequency/Sliding_Sketch/demo/main.cpp)
target_link_libraries(Sliding_Sketch -lpthread)

add_executable(SlidingSketch src/Frequency/SlidingSketch/demo/main.cpp)
target_link_libraries(SlidingSketch -lpthread)

add_executable(SWCM src/Frequency/SWCM/demo/main.cpp)
target_link_libraries(SWCM -lpthread)


# HeavyHitter
add_executable(HSS_HK src/HeavyHitter/HSS_HK/main.cpp)
target_link_libraries(HSS_HK -lpthread)

add_executable(lambda src/HeavyHitter/lambda_Algorithm/demo/main.cpp)
target_link_libraries(lambda -lpthread)

add_executable(SS_HK src/HeavyHitter/SS_HK/demo/main.cpp)
target_link_libraries(SS_HK -lpthread)

add_executable(WCSS src/HeavyHitter/WCSS/demo/recall.cpp)
target_link_libraries(WCSS -lpthread)


# Membership
add_executable(FBF src/Membership/FBF/demo/main.cpp)
target_link_libraries(FBF -lpthread)

add_executable(HSS-BF src/Membership/HSS-BF/demo/main.cpp)
target_link_libraries(HSS-BF -lpthread)

add_executable(HSS-BF-AQS src/Membership/HSS-BF-AQS/demo/main.cpp)
target_link_libraries(HSS-BF-AQS -lpthread)

add_executable(SS-BF src/Membership/SS-BF/demo/main.cpp)
target_link_libraries(SS-BF -lpthread)

add_executable(SW-BF src/Membership/SW-BF/demo/main.cpp)
target_link_libraries(SW-BF -lpthread)


