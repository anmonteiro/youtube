#!/usr/bin/env sh

set -eo pipefail

rm -rf bootstrap
docker build . --tag lambda
docker rm example || true
docker create --name example lambda
docker cp example:/app/bootstrap bootstrap

