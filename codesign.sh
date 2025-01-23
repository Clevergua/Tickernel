#!/bin/bash
PROJECT_ROOT=$(pwd)
ENTITLEMENTS_FILE="${PROJECT_ROOT}/Entitlements.plist"
BUILD_OUTPUT="/Users/forgemastergua/Documents/GitHub/Tickernel/build/Tickernel"
SIGNING_IDENTITY="3A5091E276D1B0C6E5000DBB270C5DCA464A4F4D"
# 签名文件和其依赖的 dylib
function sign_with_dependencies() {
    local file="$1"
    
    # 签名主文件
    /usr/bin/codesign --entitlements "${ENTITLEMENTS_FILE}" --sign "${SIGNING_IDENTITY}" --force "${file}"
    echo "Signed: ${file}"
    
    # 查找并签名依赖的 dylib
    otool -L "${file}" | grep -E '\s*/' | awk '{print $1}' | while read -r dylib; do
        if [ -f "${dylib}" ]; then
            /usr/bin/codesign --entitlements "${ENTITLEMENTS_FILE}" --sign "${SIGNING_IDENTITY}" --force "${dylib}"
            echo "Signed dependency: ${dylib}"
        fi
    done
}

# 签名主输出文件和依赖项
sign_with_dependencies "${BUILD_OUTPUT}"
echo "All signing completed."
