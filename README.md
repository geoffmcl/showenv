# showenv Project

## Overview

Commenced cica 20140108, just to be able to check a single environment variable...

## Build

This project uses CMake to generate local build file system.

```
cd build
cmake .. [Options]
cmake --build . --config Release
```

Options include `-DCMAKE_INSTALL_PREFIX:PATH=/path/of/install`, the generator, like `-G "Visual Studio 16 2019" -A x64`, etc, etc...

## Licence

GNU GPL version 2

;eof
