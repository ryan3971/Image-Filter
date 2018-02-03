This program performs simple noise reduction on images in Portable Pixmap Format (PPM). 

Instructions to run (on Linux):
  1) Run the Makefile via the command terminal. This will generate an executable with name "denoise" in the current directory.
  2) Run the executable by typing
              ./denoise input.ppm output.ppm N F
      where,
      - input.ppm is the name of the input file
      - output.ppm is the name of the output file
      - N specifies the size of the window
      - F is the type of fltering and can have a value A meaning mean flter, or a value M meaning median filter
              
Note: A Link has been included to show the result of the filters
