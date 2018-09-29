#!/bin/bash

set -ex

scripts_dir=$(realpath $(dirname $0))
root_dir=$(dirname ${scripts_dir})
src_dir=${root_dir}/lib/protobuf
dest_dir=${scripts_dir}/pb

mkdir -p ${dest_dir}
protoc -I${src_dir} --python_out=${dest_dir} ${src_dir}/scene.proto
