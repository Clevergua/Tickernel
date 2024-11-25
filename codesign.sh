PROJECT_ROOT=$(pwd)
ENTITLEMENTS_FILE="${PROJECT_ROOT}/Entitlements.plist"
BUILD_OUTPUT="${PROJECT_ROOT}/out/build/Clang 16.0.0 arm64-apple-darwin24.1.0/Tickernel"
SIGNING_IDENTITY="Apple Development: clevergua@outlook.com (3CHVV8VMHN)"
/usr/bin/codesign --entitlements "${ENTITLEMENTS_FILE}" --sign "${SIGNING_IDENTITY}" --force "${BUILD_OUTPUT}"
