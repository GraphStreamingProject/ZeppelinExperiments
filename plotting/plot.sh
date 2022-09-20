#!/bin/bash

mkdir latex/images 2> /dev/null

# Create images
cd R_scripts
Rscript parallel_plot.R
mv parallel.png ../latex/images/parallel.png

Rscript parallel_plot3.R
mv parallel3.png ../latex/images/parallel3.png

Rscript query.R
mv query.png ../latex/images/query.png

Rscript query_disk.R
mv query_disk.png ../latex/images/query_disk.png

Rscript space_plot.R
mv size.png ../latex/images/size.png

Rscript speed_plot.R
mv speed.png ../latex/images/speed.png

Rscript speed_plot2.R
mv speed_unlim.png ../latex/images/speed_unlim.png

# Create pdf document with images and tables
cd ../latex
pdflatex -synctex=1 -shell-escape -interaction=nonstopmode main.tex
mv main.pdf ../../figures.pdf

