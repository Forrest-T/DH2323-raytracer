def FlagsForFile(filename, **kwargs):
    return {'flags': ['-x', 'c++', '-Wall', '-Wextra', '-std=c++17', '-lOpenCL', '-isystem', '/usr/include/SDL']}
