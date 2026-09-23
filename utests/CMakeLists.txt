include_directories(${CMAKE_SOURCE_DIR}/src)
enable_testing()
include(CTest)
include(Catch)

set(UTESTS 
  utest_Success
  utest_Failure
  utest_vec3
  utest_ray
  utest_framebuffer
  utest_sphere
)

foreach(uexec ${UTESTS})
  add_executable(${uexec} ${uexec}.cpp)
  target_link_libraries(${uexec}
    PRIVATE
    Catch2::Catch2WithMain
    cs4212-util
  )
  catch_discover_tests(${uexec})
endforeach(uexec)