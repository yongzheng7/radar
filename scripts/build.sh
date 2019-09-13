#!/bin/bash

cd ${OPEN_SSL_SRC_DIR}
./Configure shared android
make CALC_VERSIONS="SHLIB_COMPAT=; SHLIB_SOVER=" build_libs
