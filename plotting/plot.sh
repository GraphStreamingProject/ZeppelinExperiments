#!/bin/bash
cd R_scripts
Rscript parallel_plot.R
mv parallel.png ../parallel.png
Rscript parallel_plot3.R
mv parallel3.png ../parallel3.png
Rscript query.R
mv query.png ../query.png
Rscript query_disk.R
mv query_disk.png ../query_disk.png
Rscript space_plot.R
mv size.png ../size.png
Rscript speed_plot.R
mv speed.png ../speed.png
Rscript speed_plot2.R
mv speed_unlim.png ../speed_unlim.png

cd ../latex_tables
pdflatex -synctex=1 -shell-escape -interaction=nonstopmode main.tex
mv main.pdf ../l0tables.pdf

