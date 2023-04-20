# OneAPI `WordCount` 

## Notices: 

- WordCount currently doesn't compile as we are sorting out an issue with strings not being accepted into SYCL buffers. Will attempt to have it working by the end of tomorrow night (04/21/23) - Sam
- Helper functions are written in the main function file as we also were having issues with linking. We plan to move them back to their separate files after successful compilation. 

### On Linux*
1. Change to the WordCount-OneAPI directory.
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
