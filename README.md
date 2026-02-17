# NA64-Gaudi-lib
Gaudi components for NA64 reconstruction

## Getting started

Build with
```
cmake -DCMAKE_INSTALL_PREFIX=/usr \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_CXX_STANDARD=20 \
      -DCPPGSL_ROOT_DIR=/opt/GSL/include \
      -DDATEMONITORING_DIR=/usr/lib64/cmake/date-monitoring \
      -DCoralDAQ_DIR=/usr/lib64/cmake/CoralDAQ \
      -DCMAKE_INSTALL_LIBDIR=/usr/lib64 \
      -DGAUDI_INSTALL_PYTHONDIR=/usr/lib/python3.12/site-packages/ \
      -Wno-dev ..
```

