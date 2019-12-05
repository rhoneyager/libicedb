## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
##
## # The following are required to submit to the CDash dashboard:
##   ENABLE_TESTING()
##   INCLUDE(CTest)

set(CTEST_PROJECT_NAME "icedb")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "cdash.honeyager.info")
set(CTEST_DROP_LOCATION "/submit.php?project=icedb")
set(CTEST_DROP_SITE_CDASH TRUE)

set(CTEST_USE_LAUNCHERS 1)
set(CTEST_LABELS_FOR_SUBPROJECTS HH icedb apps plugins algs io test)
set(ENV{CTEST_USE_LAUNCHERS_DEFAULT} 1)

