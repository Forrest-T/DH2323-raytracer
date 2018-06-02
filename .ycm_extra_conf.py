def FlagsForFile(filename, **kwargs):
    return {'flags': ['-x', 'c++', '-Wall', '-Wextra', '-std=c++17', '-lOpenCL']}
    return {'flags': ['-x', 'c', '-Wall', '-Wextra', '-std=c11', '-lOpenCL']}
