Glassdoor Review Crawler

Developed in VS2022 with libcurl for Windows
libcurl installed using vcpkg: https://github.com/microsoft/vcpkg

1 Get latest vcpkg zip file from https://github.com/microsoft/vcpkg/releases
2 Open Developer Command Prompt for VS and cd to where it is
3 Run bootstrap-vcpkg.bat
4 Run vcpkg.exe integrate install
5 Run vcpkg.exe install curl
5.1 Run vcpkg.exe install curl --triplet=x64 (for x64)

In solution:
1 Ensure solution configuration (next to debug buttons) is targeting right triplet