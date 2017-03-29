# photometric stereo

a program to reconstruct depth map from multiple image and light source information

> this program is an assignemnt of computer vision lectur in nctu

## Execute

```
reconstruct3d.exe [options] <path_to_data_folder>
```

- required parameter:
    - `path_to_data_folder`: path to the folder that contains data, which is expected to has `LightSource.txt` and pictures inside

- options:
    - `-o <filename>` assinging output filename, when not specific it use `result.plt` as default
    - `-s` enabling display images

## Environment

- Windows 10
- Visual Studio 2015 (C++14 std.)
- OpenCV 3.1.0 x64

# Environment Variable

- `%OpenCV310%`: path to OpenCV 3.1.0 build directory
