# OneAPI `WordCount` 

## APP

- WordCount-OneAPI is a Intel OneAPI implementation of a word count program that counts unique words from an input text. The benefit of this implementation is that it utilizes the SYCL library, performing operations in parallel. 

## TODOs: 

- Everything is working thus far minus OpenSSL compatibility with SYCL compiler
   - Fixed: 04/23/2023
- Update 'how to run program' section in README
   - Planned fix: 04/23/2023

## Set Environment Variables

When working with the command-line interface (CLI), you should configure the oneAPI toolkits using environment variables. Set up your CLI environment by sourcing the `setvars` script every time you open a new terminal window. This practice ensures that your compiler, libraries, and tools are ready for development.

## Build the `WordCount-OneAPI` Sample

> **Note**: If you have not already done so, set up your CLI
> environment by sourcing  the `setvars` script in the root of your oneAPI installation.
>
> Linux*:
> - For system wide installations: `. /opt/intel/oneapi/setvars.sh`
> - For private installations: ` . ~/intel/oneapi/setvars.sh`
> - For non-POSIX shells, like csh, use the following command: `bash -c 'source <install-dir>/setvars.sh ; exec csh'`
>
> Windows*:
> - `C:\Program Files(x86)\Intel\oneAPI\setvars.bat`
> - Windows PowerShell*, use the following command: `cmd.exe "/K" '"C:\Program Files (x86)\Intel\oneAPI\setvars.bat" && powershell'`
>
> For more information on configuring environment variables, see [Use the setvars Script with Linux* or macOS*](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-programming-guide/top/oneapi-development-environment-setup/use-the-setvars-script-with-linux-or-macos.html) or [Use the setvars Script with Windows*](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-programming-guide/top/oneapi-development-environment-setup/use-the-setvars-script-with-windows.html).


### Using Visual Studio Code* (VS Code) (Optional)

You can use Visual Studio Code* (VS Code) extensions to set your environment,
create launch configurations, and browse and download samples.

The basic steps to build and run a sample using VS Code include:
1. Configure the oneAPI environment with the extension **Environment Configurator for Intel® oneAPI Toolkits**.
2. Download a sample using the extension **Code Sample Browser for Intel® oneAPI Toolkits**.
3. Open a terminal in VS Code (**Terminal > New Terminal**).
4. Run the sample in the VS Code terminal using the instructions below.

To learn more about the extensions and how to configure the oneAPI environment, see the 
[Using Visual Studio Code with Intel® oneAPI Toolkits User Guide](https://www.intel.com/content/www/us/en/develop/documentation/using-vs-code-with-intel-oneapi/top.html).

### On Linux*

#### Configure the build system

1. Change to the sample directory.
2. 
   Configure the project to use the buffer-based implementation.
   ```
   mkdir build
   cd build
   cmake ..
   ```
#### Build for CPU and GPU
    
1. Build the program.
   ```
   make cpu-gpu
   ```   
2. Clean the program. (Optional)
   ```
   make clean
   ```

### On Windows* `(NOT TESTED)`

#### Configure the build system

1. Change to the sample directory.
2. 
   Configure the project to use the buffer-based implementation.
   ```
   mkdir build
   cd build
   cmake -G "NMake Makefiles" ..
   ```

#### Build for CPU and GPU

1. Build the program.
   ```
   nmake cpu-gpu
   ```
2. Clean the program. (Optional)
   ```
   nmake clean
   ```
## Run the `WordCount` Program

### Configurable Parameters

The source file (`run.cpp`) allows you to input a txt file at runtime.

### On Linux

#### Run for CPU and GPU

1. Change to the output directory.

2. Run the program for buffers.
    ```
    ./word_count -i ../hamlet.txt
    ```

### On Windows `(NOT TESTED)`

#### Run for CPU and GPU

1. Change to the output directory.

2. Run the program for buffers.
    ```
    word_count.exe -i ../hamlet.txt
    ```