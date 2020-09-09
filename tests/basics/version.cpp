#include <iostream>
#include <lyrahgames/version.hpp>
using namespace std;

int main() {
  cout << "LYRAHGAMES_VERSION = " << LYRAHGAMES_VERSION << '\n'
       << "LYRAHGAMES_VERSION_STR = " << LYRAHGAMES_VERSION_STR << '\n'
       << "LYRAHGAMES_VERSION_ID = " << LYRAHGAMES_VERSION_ID << '\n'
       << "LYRAHGAMES_VERSION_FULL = " << LYRAHGAMES_VERSION_FULL << '\n'
       << "LYRAHGAMES_VERSION_MAJOR = " << LYRAHGAMES_VERSION_MAJOR << '\n'
       << "LYRAHGAMES_VERSION_MINOR = " << LYRAHGAMES_VERSION_MINOR << '\n'
       << "LYRAHGAMES_VERSION_PATCH = " << LYRAHGAMES_VERSION_PATCH << '\n'
       << "LYRAHGAMES_PRE_RELEASE = " << LYRAHGAMES_PRE_RELEASE << '\n'
       << "LYRAHGAMES_SNAPSHOT_SN = " << LYRAHGAMES_SNAPSHOT_SN << '\n'
       << "LYRAHGAMES_SNAPSHOT_ID = " << LYRAHGAMES_SNAPSHOT_ID << '\n';
}