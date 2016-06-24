#!/bin/sh
source "$CONFIG_FOLDER/linux-source-qt.sh"

$CMAKE_BIN -DBOOST_ROOT="$BOOST_ROOT" -DISCORE_CONFIGURATION=linux-gcov ..
$CMAKE_BIN --build . --target all_unity -- -j2

echo "$PWD"
gem install coveralls-lcov
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start
sleep 3
cp -rf ../Tests/testdata .
LD_LIBRARY_PATH=/usr/lib64 $CMAKE_BIN --build . --target iscore_test_coverage_unity
# lcov --compat-libtool --directory .. --capture --output-file coverage.info --no-external
mv coverage.info.cleaned coverage.info
coveralls-lcov --repo-token jjoMcOyOg9R05XT3aVysqTcsL1gyAc9tF coverage.info
