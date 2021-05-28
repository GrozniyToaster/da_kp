#! /usr/bin/env bash
set -o nounset
set -o pipefail


function log_info()
{
  log_ "info" "$@"
}

function log_error()
{
  log_ "error" "$@"
}

function log_()
{
  local type=$1
  local message=$2
  local date_str=; date_str=$(date +'%Y-%m-%d %H:%M:%S')
  echo "[${type}] [${date_str}] ${message}"
}
function main()
{



  local exutable_folder='./cmake-build-debug'
  local exutable="kp"
  test_dir="tests"
  for test_file in $( ls ${test_dir}/*.t ) ; do
    start=$SECONDS
    if !  ${exutable_folder}/${exutable} < ${test_file} > tmp; then
        log_error "Failed to run ${benchmark_bin} for ${test_file}."
        return 1
    fi
    count=$(wc -m tmp | cut -d ' ' -f1)
    log_info "${test_file} compressed by $(( SECONDS - start ))  seconds in  approx $(( count / 3 )) symbols"

  done


}

main $@