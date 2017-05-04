#!/bin/bash -eux

git submodule update --init --recursive
# export ISCORE_PLUGINS_TO_BUILD=("iscore-addon-csp")
export ISCORE_PLUGINS_TO_BUILD=("iscore-addon-staticanalysis" "iscore-addon-remotecontrol")

# Set-up the plug-ins
(
cd base/addons
for plugin in "${ISCORE_PLUGINS_TO_BUILD[@]}"
do
   git clone "https://github.com/himito/$plugin"
done

for plugin in */
do
 (
   cd $plugin
   git submodule update --init --recursive
 )
done
)

# In this case everything is built in docker
if [[ "$CONF" == "linux-package-appimage" ]];
then
    exit 0
fi

# Install the deps
case "$TRAVIS_OS_NAME" in
  linux)
    sudo wget https://github.com/OSSIA/iscore-sdk/releases/download/sdk7/boost.tar.bz2 -O /opt/boost.tar.bz2 &

    wget https://cmake.org/files/v3.8/cmake-3.8.0-Linux-x86_64.tar.gz -O cmake-linux.tgz &

    echo 'deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-4.0 main' | sudo tee /etc/apt/sources.list.d/llvm.list
    sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 1397BC53640DB551

    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo add-apt-repository --yes ppa:beineri/opt-qt58-trusty

    sudo apt-get update -qq
    sudo apt-get install -qq --force-yes g++-6 binutils libasound-dev ninja-build gcovr lcov qt58-meta-minimal qt58svg qt58quickcontrols2 qt58websockets clang-4.0
    # lld-4.0 : too buggy yet

    wait wget || true
    (cd /opt; sudo tar xaf boost.tar.bz2; sudo mv boost_* boost ; sudo chmod -R a+rwx boost)

    tar xaf cmake-linux.tgz
    mv cmake-*-x86_64 cmake
  ;;
  osx)
    set +e

    brew uninstall --ignore-dependencies boost
    ARCHIVE=homebrew-cache.tar.xz
    brew install gnu-tar xz

    wget https://github.com/OSSIA/iscore-sdk/releases/download/sdk7/$ARCHIVE -O $ARCHIVE
    gtar xhaf $ARCHIVE --directory /usr/local/Cellar
    brew unlink cmake
    brew link --force boost cmake ninja qt5

    set -e
  ;;
esac
