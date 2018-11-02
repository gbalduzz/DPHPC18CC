
**** 
ACCORDING TO DPHPC2018 HP
*****
Compile as:
latex report
bibtex report
latex report
latex report

***
CHANGED
***
To get pdf: 
exec dvips -q -f report.dvi | ps2pdf - report.pdf

Or just: make all 
