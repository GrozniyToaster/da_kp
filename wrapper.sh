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



  local test_dir="tests"
  rm -rf ${test_dir}
  mkdir -p ${test_dir}
  local examples_cnt=1000
  for examples_cnt in '10' '100' '1000' '10000' '1000000' '10000000'
  do
    log_info "Generating tests (examples for each test=[${examples_cnt}])..."
    if ! python3.8 generator.py ${test_dir} ${examples_cnt} ; then
      log_error "ERROR: Failed to python generate tests."
      return 1
    fi
  done


}

main $@