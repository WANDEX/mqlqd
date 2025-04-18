#!/bin/sh
## build & optionally run tests. AUTHOR: WANDEX.
## Script is meant only for the project developer and as the reference.
## No guarantees that it will work on your OS/platform!
## Use standard cmake commands for building the project.
## Or adapt script steps yourself for your personal preference.
##
## NOTE: manually export following environment variables
## or pass -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++
## to build/test under multiple compilers before submitting code!
#
# export CC=gcc   CXX=g++
# export CC=clang CXX=clang++
#
## build:
# ./scripts/build.sh
## build & run tests:
# ./scripts/build.sh ctest
## make clean build & run tests:
# ./scripts/build.sh clean ctest
## make cleaner build (for edge cases) & run tests:
# ./scripts/build.sh cleaner ctest
## build & run filtered tests:
# ./scripts/build.sh ctest .*regex.*
## or
# ./scripts/build.sh gtest *wildcard*

set -e

if [ ! -r ./CMakeLists.txt ]; then
  printf "%s\n" "Current directory does not contain CMakeLists.txt, exit."
  exit 1
fi

at_path() { hash "$1" >/dev/null 2>&1 ;} # if $1 is found at $PATH -> return 0

notify() {
  at_path notify-send || return 0
  stag="string:x-dunst-stack-tag"
  bg="string:bgcolor:"
  fg="string:fgcolor:"
  arg="$2"
  case "$1" in
    error|ERROR)     urg="critical" ;;
    warning|WARNING) urg="normal" ;;
    *)
      urg="low"
      bg="string:bgcolor:#11FF44"
      fg="string:fgcolor:#000000"
      arg="$1"
    ;;
  esac
  notify-send -u "$urg" -h "$stag:$prj_name" -h "$stag:hi" -h "$bg" -h "$fg" "[$prj_name]" "$arg"
}

prj_name=""
prj_name_upper=""
if at_path rg; then
  prj_name=$(rg -Ue '\bproject\([\s]*[\n]*\b(\w+)\b' --trim -or '$1' ./CMakeLists.txt)
else
  prj_name=$(basename "$(git rev-parse --show-toplevel)")
fi
prj_name_upper=$(echo "$prj_name" | tr "[:lower:]" "[:upper:]")

PRJ_BUILD_TESTS="${BUILD_TESTS:-"${prj_name_upper}_BUILD_TESTS"}"
bt="${BUILD_TYPE:-Debug}"
generator="${GENERATOR:-Ninja}" # "MSYS Makefiles", "Unix Makefiles", Ninja
compiler="${CC:-_}"
verbose="${VERBOSE:-0}"
deploy="${DEPLOY:-0}"
cmbn=$(basename "$compiler") # get compiler basename in case declared via full path
bdir="build/dev-$bt-$cmbn"

beg_epoch=$(date +%s) # for the first call of the time_spent_on_step

_verbose=""
cmake_log_level=""
if [ "$verbose" = 1 ]; then
  _verbose="--verbose"
  cmake_log_level="--log-level=DEBUG"
fi

opt="$1"
test_filter="$2"
fresh=""
clean_first=""
case "$opt" in
  clean|c)
    opt="$2"
    test_filter="$3"
    fresh="--fresh"
    clean_first="--clean-first"
  ;;
  cleaner|cc) # for edge cases like corrupted GCDA files etc.
    opt="$2"
    test_filter="$3"
    [ -d "$bdir" ] && rm -rf "$bdir"
    mkdir -p "$bdir"
  ;;
esac
## for toggling building of the tests
if [ -n "$opt" ]; then
  tt=ON
else
  tt=OFF
fi

time_spent_on_step() {
  # HACK: because standard utility/command 'time' does not work.
  end_epoch=$(date +%s)
  diff_secs=$((end_epoch-beg_epoch))
  # small insignificant optimization
  [ "$diff_secs" = 0 ] && beg_epoch="$end_epoch" && return
  spent_time=$(date -d "@${diff_secs}" "+%Mm %Ss")
  beg_epoch=$(date +%s)
  printf "step took:%b %s.%b\n" "${BLD}" "$spent_time" "${END}"
}

und='=========================='
sep="${und}${und}${und}"
vsep() {
  time_spent_on_step
  printf "\n%s[%s]\n%s\n\n" "${2}" "${1}" "${sep}${END}"
}

vsep "CONFIGURE" "${BLU}"
cmake -S . -B "$bdir" -G "$generator" -D CMAKE_BUILD_TYPE="${bt}" -D "$PRJ_BUILD_TESTS=${tt}" \
-Wdev -Werror=dev ${fresh} ${cmake_log_level}

vsep "BUILD" "${CYN}"
cmake --build "$bdir" --config "${bt}" ${clean_first} ${_verbose}

notify "BUILT"

# shellcheck disable=SC2068 # Intentional - to re-split trailing arguments.
run_ctest() { ctest --output-on-failure --test-dir "$bdir" $@ ;} # shortcut

[ -n "$opt" ] && vsep "TESTS" "${RED}"
[ -n "$test_filter" ] || test_filter='.*'
case "$opt" in
  ctest|ct)
    ## optionally filter by regex
    run_ctest -R "$test_filter"
  ;;
  ctp)
    ## shortcut for terse progress output
    run_ctest --progress -R "$test_filter"
  ;;
  ctr)
    ## suppress output, errors, always return success (to not immediately exit because of 'set -e')
    run_ctest -R "$test_filter" >/dev/null 2>&1 || true
    ## rerun previously failed (gives live (filtered) results, with only failed tests)
    run_ctest --rerun-failed
  ;;
  gtest|gt)
    gtest_binary="./$bdir/bin/tests_units"
    if [ "$tt" = ON ] && [ ! -x "$gtest_binary" ]; then
      printf "%s\n^ %s\n" "$gtest_binary" "File not found or not executable, exit."
      exit 5
    fi
    ## optionally filter by wildcard ':'-separated patterns '*', '?' ('-' negative)
    [ "$test_filter" = '.*' ] && test_filter='*'
    "$gtest_binary" --gtest_filter="$test_filter"
  ;;
esac

if [ "$deploy" = 1 ]; then
  vsep "DEPLOY" "${MAG}"
  cmake --install "$bdir" --config "${bt}" --prefix "$bdir/deploy"
fi

[ -n "$opt" ] && vsep "COMPLETED" "${GRN}"

