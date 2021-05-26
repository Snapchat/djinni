#! /bin/bash
set -eu

# Locate the script file.  Cross symlinks if necessary.
loc="$0"
while [ -h "$loc" ]; do
    ls=$(ls -ld "$loc")
    link=$(expr "$ls" : '.*-> \(.*\)$')
    if expr "$link" : '/.*' > /dev/null; then
        loc="$link"  # Absolute link
    else
        loc="$(dirname "$loc")/$link"  # Relative link
    fi
done
base_dir=$(cd "$(dirname "$loc")" && pwd)

pushd "$base_dir"
${BAZEL_EXECUTABLE:-bazel} build //src:djinni
popd
